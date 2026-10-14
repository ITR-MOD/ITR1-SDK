#include "Toolkit/AssetTypeGenerator/SoundCueGenerator.h"
#include "UE427AssetPath.h"

#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EdGraph/EdGraphNode.h"
#include "Modules/ModuleManager.h"
#include "SoundCueGraph/SoundCueGraphNode.h"
#include "SoundCueGraph/SoundCueGraphNode_Root.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundNode.h"
#include "ScopedTransaction.h"
#include "GraphEditor.h"
#include "EdGraphUtilities.h"
#include "SNodePanel.h"
#include "Editor.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Widgets/Docking/SDockTab.h"

#include "Dom/JsonObject.h"
#include "Internationalization/Regex.h"

UClass* USoundCueGenerator::GetAssetObjectClass()  const {
	return USoundCue::StaticClass();
}

FTopLevelAssetPath USoundCueGenerator::GetAssetClass() {
	return FTopLevelAssetPath(USoundCue::StaticClass());
}

void USoundCueGenerator::CreateAssetPackage() {
	UPackage* NewPackage = CreatePackage(*GetPackageName().ToString());
	USoundCue* SoundCue = NewObject<USoundCue>(NewPackage, GetAssetName(), RF_Public | RF_Standalone);
	SetPackageAndAsset(NewPackage, SoundCue);

	RebuildSoundCue(SoundCue);
}

void USoundCueGenerator::OnExistingPackageLoaded() {
	RebuildSoundCue(GetAsset<USoundCue>());
}

void USoundCueGenerator::RebuildSoundCue(USoundCue* SoundCue) {
	SoundCue->CreateGraph();
	SoundCue->GetGraph()->Modify();
	SoundCue->Modify();

	FString TextToImport;
    if (!GetAssetData()->TryGetStringField(TEXT("SoundCueGraph"), TextToImport)) {
        FailGeneration(FString::Printf(TEXT("Missing SoundCueGraph for %s"), *GetPackageName().ToString()));
        return;
    }
    TArray<FString> GraphLines;
    TextToImport.ReplaceInline(TEXT("\r"), TEXT(""));
    TextToImport.ParseIntoArrayLines(GraphLines, false);
    GraphLines.RemoveAll([](const FString& Line) {
        const FString Property = Line.TrimStart();
        return Property.StartsWith(TEXT("ChildNodes(")) || Property.StartsWith(TEXT("ChildNodes=")) || Property.StartsWith(TEXT("GraphNode="));
    });
    struct FImportedPin { FString NodeName; FString Name; FGuid Id; bool bOutput; TArray<FGuid> Links; };
    TArray<FImportedPin> ImportedPins;
    FString GraphNodeName;
    const FRegexPattern NodePattern(TEXT("Name=\"([^\"]+)\""));
    const FRegexPattern IdPattern(TEXT("PinId=([A-Fa-f0-9]{32})"));
    const FRegexPattern NamePattern(TEXT("PinName=\"([^\"]*)\""));
    const FRegexPattern LinkPattern(TEXT("SoundCueGraphNode_[0-9]+ ([A-Fa-f0-9]{32})"));
    for (const FString& Line : GraphLines)
    {
        if (Line.StartsWith(TEXT("Begin Object")))
        {
            FRegexMatcher Match(NodePattern, Line);
            if (Match.FindNext()) GraphNodeName = Match.GetCaptureGroup(1);
        }
        if (!Line.TrimStart().StartsWith(TEXT("CustomProperties Pin"))) continue;
        FImportedPin Pin;
        Pin.NodeName = GraphNodeName;
        Pin.bOutput = Line.Contains(TEXT("EGPD_Output"));
        FRegexMatcher IdMatch(IdPattern, Line), NameMatch(NamePattern, Line);
        if (!IdMatch.FindNext() || !NameMatch.FindNext() || !FGuid::ParseExact(IdMatch.GetCaptureGroup(1), EGuidFormats::Digits, Pin.Id))
        { FailGeneration(TEXT("Invalid SoundCue pin definition.")); return; }
        Pin.Name = NameMatch.GetCaptureGroup(1);
        FRegexMatcher LinkMatch(LinkPattern, Line);
        while (LinkMatch.FindNext())
        {
            FGuid Id;
            if (!FGuid::ParseExact(LinkMatch.GetCaptureGroup(1), EGuidFormats::Digits, Id)) { FailGeneration(TEXT("Invalid SoundCue pin link.")); return; }
            Pin.Links.Add(Id);
        }
        ImportedPins.Add(MoveTemp(Pin));
    }
    GraphLines.RemoveAll([](const FString& Line) { return Line.TrimStart().StartsWith(TEXT("CustomProperties Pin")); });
    TextToImport = FString::Join(GraphLines, TEXT("\n"));
    if (!UE427IsEmpty(TextToImport) && !FEdGraphUtilities::CanImportNodesFromText(SoundCue->GetGraph(), TextToImport)) {
        FailGeneration(FString::Printf(TEXT("Invalid SoundCueGraph for %s"), *GetPackageName().ToString()));
        return;
    }
    const TArray<UEdGraphNode*> ExistingNodes = SoundCue->GetGraph()->Nodes;
    for (UEdGraphNode* Node : ExistingNodes) {
        if (!Cast<USoundCueGraphNode_Root>(Node)) Node->DestroyNode();
        else Node->BreakAllNodeLinks();
    }
    SoundCue->AllNodes.Reset();
    SoundCue->FirstNode = nullptr;
	
	// Import the nodes
	TSet<UEdGraphNode*> PastedNodes;
	FEdGraphUtilities::ImportNodesFromText(SoundCue->GetGraph(), TextToImport, /*out*/ PastedNodes);
    TMap<FGuid, UEdGraphPin*> Pins;
    for (const FImportedPin& Definition : ImportedPins)
    {
        UEdGraphNode* Owner = nullptr;
        for (UEdGraphNode* Node : PastedNodes) if (Node->GetName() == Definition.NodeName) { Owner = Node; break; }
        if (!Owner) { FailGeneration(TEXT("Missing imported SoundCue node: ") + Definition.NodeName); return; }
        UEdGraphPin* Pin = Owner->CreatePin(Definition.bOutput ? EGPD_Output : EGPD_Input, TEXT("SoundNode"), *Definition.Name);
        Pin->PinId = Definition.Id;
        Pins.Add(Definition.Id, Pin);
    }
    for (const FImportedPin& Definition : ImportedPins)
        for (const FGuid& Target : Definition.Links)
        {
            UEdGraphPin* Other = Pins.FindRef(Target);
            if (!Other) { FailGeneration(TEXT("Missing imported SoundCue link target.")); return; }
            Pins[Definition.Id]->MakeLinkTo(Other);
        }




	for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It)
	{
		UEdGraphNode* Node = *It;

		if (USoundCueGraphNode* SoundGraphNode = Cast<USoundCueGraphNode>(Node))
		{
			if (SoundGraphNode->SoundNode) {
				SoundGraphNode->SoundNode->GraphNode = SoundGraphNode;
				SoundCue->AllNodes.AddUnique(SoundGraphNode->SoundNode);
			}
		}

		if (Node->NodePosX == 0 && Node->NodePosY == 0) {
			for (UEdGraphNode* node : SoundCue->GetGraph()->Nodes)
			{
				if (USoundCueGraphNode_Root* root = Cast<USoundCueGraphNode_Root>(node)) {
					if (UEdGraphPin* Output = Node->FindPin(TEXT("Output"), EGPD_Output); Output && root->Pins.Num() > 0) Output->MakeLinkTo(root->Pins[0]);
				}
			}
			
		}
		Node->NodePosX = Node->NodePosX - 300;
		Node->SnapToGrid(SNodePanel::GetSnapGridSize());

		// Give new node a different Guid from the old one
		Node->CreateNewGuid();
	}

	// Force new pasted SoundNodes to have same connections as graph nodes
	SoundCue->CompileSoundNodesFromGraphNodes();

	// Update UI
	//SoundCueGraphEditor->NotifyGraphChanged();

	PopulateSimpleAssetWithData(SoundCue);
	SoundCue->PostEditChange();
	SoundCue->MarkPackageDirty();


	MarkAssetChanged();
}

void USoundCueGenerator::PopulateStageDependencies(TArray<FPackageDependency>& OutDependencies) const {
    Super::PopulateStageDependencies(OutDependencies);
    if (GetCurrentStage() != EAssetGenerationStage::CONSTRUCTION) return;
    FString Graph;
    if (!GetAssetData()->TryGetStringField(TEXT("SoundCueGraph"), Graph)) return;
    const FRegexPattern Pattern(TEXT("(/[A-Za-z0-9_]+/[^\\s\\\"'(),=]+)\\.[^\\s\\\"'(),=]+"));
    FRegexMatcher Matcher(Pattern, Graph);
    TSet<FName> Packages;
    while (Matcher.FindNext()) {
        const FString Package = Matcher.GetCaptureGroup(1);
        if (!Package.StartsWith(TEXT("/Script/")) && Package != GetPackageName().ToString()) Packages.Add(FName(*Package));
    }
    for (FName Package : Packages) OutDependencies.Add(FPackageDependency{Package, EAssetGenerationStage::CDO_FINALIZATION});
}
