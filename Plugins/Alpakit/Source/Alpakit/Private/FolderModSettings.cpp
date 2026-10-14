#include "FolderModSettings.h"
#include "Interfaces/IPluginManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "IUATHelperModule.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "HAL/FileManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Widgets/Input/SComboBox.h"

void UFolderModSettings::SaveSelection()
{
    if (!Configurations.IsValidIndex(SelectedConfiguration))
        SelectedConfiguration = Configurations.AddDefaulted();
    auto& Configuration = Configurations[SelectedConfiguration];
    Configuration.ModName = ModName;
    Configuration.Folders = Folders;
    Configuration.Assets = Assets;
    Configuration.bPatchSuffix = bPatchSuffix;
    Configuration.bAutoInstall = bAutoInstall;
    Configuration.InstallDirectory = InstallDirectory;
    SaveConfig();
}

void UFolderModSettings::LoadSelection(int32 Index)
{
    if (!Configurations.IsValidIndex(Index)) return;
    SelectedConfiguration = Index;
    const auto& Configuration = Configurations[Index];
    ModName = Configuration.ModName;
    Folders = Configuration.Folders;
    Assets = Configuration.Assets;
    bPatchSuffix = Configuration.bPatchSuffix;
    bAutoInstall = Configuration.bAutoInstall;
    InstallDirectory = Configuration.InstallDirectory;
    SaveConfig();
}

void ShowFolderModWindow()
{
    UFolderModSettings* Settings = GetMutableDefault<UFolderModSettings>();
    if (Settings->Configurations.Num() == 0) Settings->SaveSelection();
    Settings->LoadSelection(Settings->SelectedConfiguration);
    FDetailsViewArgs Args;
    Args.bHideSelectionTip = true;
    auto& Module = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    TSharedRef<IDetailsView> Details = Module.CreateDetailView(Args);
    Details->SetObject(Settings);
    auto Options = MakeShared<TArray<TSharedPtr<FString>>>();
    for (const auto& Configuration : Settings->Configurations) Options->Add(MakeShared<FString>(Configuration.ModName));
    TSharedPtr<SComboBox<TSharedPtr<FString>>> Picker;
    auto Dropdown = MakeShared<TWeakPtr<SComboBox<TSharedPtr<FString>>>>();
    auto Refresh = [Settings, Options, Details, Dropdown]()
    {
        Options->Reset();
        for (const auto& Configuration : Settings->Configurations) Options->Add(MakeShared<FString>(Configuration.ModName));
        if (auto Combo = Dropdown->Pin()) Combo->RefreshOptions();
        Details->ForceRefresh();
    };
    TSharedRef<SWindow> Window = SNew(SWindow).Title(FText::FromString(TEXT("Alpakit Non-Plugin Mod"))).ClientSize(FVector2D(760, 600));
    Window->SetContent(SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(10)
        [SNew(SHorizontalBox)
            + SHorizontalBox::Slot().FillWidth(1)
            [SAssignNew(Picker, SComboBox<TSharedPtr<FString>>).OptionsSource(&Options.Get())
                .OnGenerateWidget_Lambda([](TSharedPtr<FString> Item) { return SNew(STextBlock).Text(FText::FromString(*Item)); })
                .OnSelectionChanged_Lambda([Settings, Options, Details](TSharedPtr<FString> Item, ESelectInfo::Type)
                {
                    if (!Item) return;
                    const int32 Index = Options->IndexOfByKey(Item);
                    Settings->SaveSelection();
                    Settings->LoadSelection(Index);
                    Details->ForceRefresh();
                })
                [SNew(STextBlock).Text_Lambda([Settings]() { return FText::FromString(Settings->ModName); })]]
            + SHorizontalBox::Slot().AutoWidth().Padding(6, 0)
            [SNew(SButton).Text(FText::FromString(TEXT("New"))).OnClicked_Lambda([Settings, Refresh]()
                {
                    Settings->SaveSelection();
                    FNonPluginModConfiguration Configuration;
                    int32 Number = Settings->Configurations.Num() + 1;
                    do { Configuration.ModName = FString::Printf(TEXT("NonPluginMod_%d"), Number++); }
                    while (Settings->Configurations.ContainsByPredicate([&](const auto& Existing) { return Existing.ModName == Configuration.ModName; }));
                    Settings->LoadSelection(Settings->Configurations.Add(Configuration));
                    Refresh();
                    return FReply::Handled();
                })]
            + SHorizontalBox::Slot().AutoWidth()
            [SNew(SButton).Text(FText::FromString(TEXT("Save"))).OnClicked_Lambda([Settings, Refresh]()
                { Settings->SaveSelection(); Refresh(); return FReply::Handled(); })]]
        + SVerticalBox::Slot().FillHeight(1)[Details]
        + SVerticalBox::Slot().AutoHeight().Padding(10)
        [SNew(SButton).Text(FText::FromString(TEXT("Package"))).OnClicked_Lambda([Settings]()
        {
            Settings->SaveSelection();
            TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
            Json->SetStringField(TEXT("ModName"), Settings->ModName);
            TArray<TSharedPtr<FJsonValue>> Folders;
            for (const FDirectoryPath& Folder : Settings->Folders) Folders.Add(MakeShared<FJsonValueString>(Folder.Path));
            Json->SetArrayField(TEXT("Folders"), Folders);
            TArray<TSharedPtr<FJsonValue>> Assets;
            for (const auto& Asset : Settings->Assets)
                if (!Asset.IsNull()) Assets.Add(MakeShared<FJsonValueString>(Asset.ToSoftObjectPath().GetLongPackageName()));
            Json->SetArrayField(TEXT("Assets"), Assets);
            Json->SetBoolField(TEXT("PatchSuffix"), Settings->bPatchSuffix);
            Json->SetBoolField(TEXT("AutoInstall"), Settings->bAutoInstall);
            Json->SetStringField(TEXT("InstallDirectory"), Settings->InstallDirectory.Path);
            FString Text;
            FJsonSerializer::Serialize(Json, TJsonWriterFactory<>::Create(&Text));
            FString Root = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("AlpakitNonPluginMods"));
            IFileManager::Get().MakeDirectory(*Root, true);
            FString Manifest = Root / (FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".json"));
            if (!FFileHelper::SaveStringToFile(Text, *Manifest))
            {
                FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT("Could not save the selection.")));
                return FReply::Handled();
            }
            const FString Project = FPaths::ConvertRelativePathToFull(FPaths::GetProjectFilePath());
            FString Command = FString::Printf(TEXT("-ScriptsForProject=\"%s\" ITR2PackageFolders -project=\"%s\" -Selection=\"%s\""), *Project, *Project, *Manifest);
            Command += FString::Printf(TEXT(" -ScriptDir=\"%s\""), *(IPluginManager::Get().FindPlugin(TEXT("Alpakit"))->GetBaseDir() / TEXT("Source")));
            IUATHelperModule::Get().CreateUatTask(Command, FText::FromString(TEXT("Windows")), FText::FromString(TEXT("Package mod")), FText::FromString(TEXT("Non-Plugin Mod")), nullptr);
            return FReply::Handled();
        })]);
    *Dropdown = Picker;
    FSlateApplication::Get().AddWindow(Window);
}
