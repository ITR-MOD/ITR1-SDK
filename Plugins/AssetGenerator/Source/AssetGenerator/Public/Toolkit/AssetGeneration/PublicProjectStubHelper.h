#pragma once
#include "CoreMinimal.h"
#include "UE427AssetPath.h"

class ASSETGENERATOR_API FStubFileInfo {
	FString FullFilePath;
	FString FileHash;
public:
	FStubFileInfo(const FString& FileName);

	FORCEINLINE FString GetFullFilePath() const { return FullFilePath; }
	FORCEINLINE FString GetFileHash() const { return FileHash; }
};

template<typename T>
class TStubAssetInfo {
	FString PackagePath;
	mutable TWeakObjectPtr<T> Object;
public:
	FORCEINLINE TStubAssetInfo(const TCHAR* PackageName) : PackagePath(PackageName) {}
	FORCEINLINE T* GetObject() const {
		T* Result = Object.Get();
		if (!Result) {
			Result = LoadObject<T>(nullptr, *PackagePath);
			checkf(Result, TEXT("Failed to load stub %s asset from %s"), *T::StaticClass()->GetName(), *PackagePath);
			Object = Result;
		}
		return Result;
	}
};

class ASSETGENERATOR_API FPublicProjectStubHelper {
public:
	static FString ResolveStubFilePath(const FString& Filename);

	static FStubFileInfo EditorCube;
	static FStubFileInfo DefaultTexture;
	static FStubFileInfo DefaultSkeletalMesh;
	static TStubAssetInfo<USkeleton> DefaultSkeletalMeshSkeleton;
};
