#include "CoreMinimal.h"
#if ENGINE_MAJOR_VERSION >= 5
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UE427AssetPath.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Internationalization/StringTableCore.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCASStringTableCsvTest, "AssetGenerator.Localization.StringTableCsv", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCASStringTableCsvTest::RunTest(const FString& Parameters)
{
	FString Filename;
	if (!FParse::Value(FCommandLine::Get(), TEXT("CASStringTableCsv="), Filename))
	{
		AddInfo(TEXT("Supply -CASStringTableCsv=<CAS localization test CSV> to run the native import check."));
		return true;
	}
	FStringTableRef Table = MakeShared<FStringTable, ESPMode::ThreadSafe>();
	Table->SetNamespace(TEXT("One"));
	if (!TestTrue(TEXT("Native UE String Table CSV importer accepts CAS output"), Table->ImportStrings(Filename))) return false;
	FString Value;
	TestTrue(TEXT("Greeting key exists"), Table->GetSourceString(TEXT("Greeting"), Value));
	TestEqual(TEXT("Unicode, quotes, comma, newline, tabs, CRLF and backslashes survive"), Value,
		FString(TEXT("Hello, \"world\"!\nNext\tline\r\nC:\\Games\\ITR2\n日本語 Привет 😀")));
	TestTrue(TEXT("Quoted comma-containing key exists"), Table->GetSourceString(TEXT("Quoted,key"), Value));
	TestEqual(TEXT("Apostrophe survives"), Value, FString(TEXT("It's ready")));
	TestTrue(TEXT("Empty string key exists"), Table->GetSourceString(TEXT("Empty"), Value));
	TestTrue(TEXT("Empty string remains empty"), UE427IsEmpty(Value));
	return true;
}
#endif

#endif
