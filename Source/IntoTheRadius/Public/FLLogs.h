#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FLLogs.generated.h"

class UObject;

UCLASS(Blueprintable)
class INTOTHERADIUS_API UFLLogs : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFLLogs();

    UFUNCTION(BlueprintCallable)
    static void SwitchWorldDumpLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchSpawnerLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchSpawnerDebug();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchSingletonsLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchSavesLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchPoolLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchLootTablesLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchLogsEnabled();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchLevelsLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchInventoryLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchINILogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchGameReferencesLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchGameModeLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchGameDataLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchDetailedEventsLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchDataParsingLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchConditionsLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchClusterLogs();
    
    UFUNCTION(BlueprintCallable)
    static void SwitchBasicActorLogs();
    
    UFUNCTION(BlueprintCallable)
    static void PRLogStacktrace(bool Warning, const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    static void PRLogNormal(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    static void PRLogLines(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    static void PRLogError(const FString& Message);
    
    UFUNCTION(BlueprintCallable)
    static void PRLogBanner(const FString& Symbol, const FString& Message);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintWorldDumpLogs();
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void PrintWorldDump(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintSpawnerLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintSingletonsLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintSavesLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintPoolLogs();
    
    UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
    static void PrintNPCDump(const UObject* WorldContextObject);
    
    UFUNCTION(BlueprintCallable)
    static void PrintMissionsDataDump();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintLootTablesLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintLevelsLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintInventoryLogs();
    
    UFUNCTION(BlueprintCallable)
    static void PrintInventoryDataDump();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintINILogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintGameReferencesLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintGameModeLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintGameDataLogs();
    
    UFUNCTION(BlueprintCallable)
    static void PrintGameDataDump();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintDetailedEventsLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintDataParsingLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintConditionsLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintClusterLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool PrintBasicActorLogs();
    
    UFUNCTION(BlueprintCallable)
    static void LogToFile(const FString& Message);
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool LogsEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEnabledTeleportDebug();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEnabledPlayerStatsLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEnabledBTRDebug();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsEnabledAnomaliesLogs();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsAIStateDebugEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool IsAICoverDebugEnabled();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static float GetTeleportDebugTime();
    
    UFUNCTION(BlueprintCallable, BlueprintPure)
    static bool DrawSpawnerDebug();
    
};

