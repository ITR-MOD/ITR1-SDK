#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"
#include "Engine/DeveloperSettings.h"
#include "AssetGeneratorLocalSettings.generated.h"

UCLASS(Config = Editor, meta = (DisplayName = "Asset Generator Local Settings"))
class ASSETGENERATOR_API UAssetGeneratorLocalSettings : public UDeveloperSettings {
	GENERATED_BODY()
public:
	FORCEINLINE static UAssetGeneratorLocalSettings* Get() {
		return GetMutableDefault<UAssetGeneratorLocalSettings>();
	};
public:
	UAssetGeneratorLocalSettings() :
		MaxAssetsToAdvancePerTick(4),
		bRefreshExistingAssets(true),
		bGeneratePublicProject(false) {
	}
	
	/** Path to the asset dump last generated from, or empty string */
	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	FString CurrentAssetDumpPath;

	/** Asset categories that have been whitelisted for the generation. Editable through asset generator UI */
	UPROPERTY(VisibleAnywhere, Config, Category = "Asset Generator")
	TSet<FTopLevelAssetPath> WhitelistedAssetCategories;

	/** Maximum amount of asset generators to advance in one tick */
	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	int32 MaxAssetsToAdvancePerTick;
	
	/** Whenever existing assets need to be refreshed */
	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	bool bRefreshExistingAssets;

	/** Whenever we are generating a public project */
	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	bool bGeneratePublicProject;

	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	bool bAggressiveMacroDetection = false;

	UPROPERTY(EditAnywhere, Config, Category = "Asset Generator")
	bool bMeshesAndTexturesOnly = false;

	static bool IsMeshTextureOrMaterial(const FName ClassName) {
		const FString Name = ClassName.ToString();
		return Name.Contains(TEXT("Texture")) || Name.StartsWith(TEXT("Material")) || Name.StartsWith(TEXT("PhysicalMaterial")) ||
			Name == TEXT("StaticMesh") || Name == TEXT("SkeletalMesh") || Name == TEXT("Skeleton") || Name == TEXT("PhysicsAsset") ||
			Name == TEXT("MediaTexture") || Name == TEXT("VolumeTexture") || Name == TEXT("CurveLinearColorAtlas") || Name == TEXT("SubsurfaceProfile") ||
			Name == TEXT("RuntimeVirtualTexture") || Name == TEXT("SparseVolumeTexture") || Name == TEXT("StaticSparseVolumeTexture") || Name == TEXT("AnimatedSparseVolumeTexture");
	}
};
