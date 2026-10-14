#pragma once
#include "CoreMinimal.h"
#include "UObject/Script.h"
#include "UE427AssetPath.generated.h"
USTRUCT()
struct FTopLevelAssetPath
{
    GENERATED_BODY()
    UPROPERTY() FName PackageName;
    UPROPERTY() FName AssetName;
    FTopLevelAssetPath() = default;
    FTopLevelAssetPath(FName Name) { Set(Name.ToString()); }
    FTopLevelAssetPath(std::nullptr_t) {}
    FTopLevelAssetPath(FName Package, FName Asset) : PackageName(Package), AssetName(Asset) {}
    FTopLevelAssetPath(const UObject* Object) { if (Object) Set(Object->GetPathName()); }
    FTopLevelAssetPath(const FString& Path) { Set(Path); }
    FTopLevelAssetPath(const TCHAR* Path) { Set(Path); }
    void Set(const FString& Path) { FString Package, Asset; if (Path.Split(TEXT("."), &Package, &Asset, ESearchCase::CaseSensitive, ESearchDir::FromEnd)) { PackageName = *Package; AssetName = *Asset; } else AssetName = *Path; }
    bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
    {
        while (FChar::IsWhitespace(*Buffer)) ++Buffer;
        if (*Buffer == TEXT('('))
        {
            const TCHAR* End = FCString::Strchr(Buffer, TEXT(')'));
            if (!End) return false;
            FString Fields(End - Buffer, Buffer), Package, Asset;
            FParse::Value(*Fields, TEXT("PackageName="), Package);
            FParse::Value(*Fields, TEXT("AssetName="), Asset);
            PackageName = *Package; AssetName = *Asset;
            Buffer = End + 1;
            return true;
        }
        FString Path;
        if (*Buffer == TEXT('"'))
        {
            ++Buffer;
            const TCHAR* Start = Buffer;
            while (*Buffer && *Buffer != TEXT('"')) ++Buffer;
            Path = FString(Buffer - Start, Start);
            if (*Buffer) ++Buffer;
        }
        else
        {
            const TCHAR* Start = Buffer;
            while (*Buffer && *Buffer != TEXT(',') && *Buffer != TEXT(')') && !FChar::IsWhitespace(*Buffer)) ++Buffer;
            Path = FString(Buffer - Start, Start);
        }
        Set(Path);
        if (PackageName.IsNone())
            if (UClass* Class = FindObject<UClass>(ANY_PACKAGE, *Path)) Set(Class->GetPathName());
        return !Path.IsEmpty();
    }
    bool ExportTextItem(FString& Value, const FTopLevelAssetPath& Default, UObject* Parent, int32 PortFlags, UObject* Scope) const
    {
        Value += TEXT("\"") + ToString() + TEXT("\"");
        return true;
    }
    FName GetAssetName() const { return AssetName; }
    FName GetPackageName() const { return PackageName; }
    FString ToString() const { return PackageName.IsNone() ? AssetName.ToString() : PackageName.ToString() + TEXT(".") + AssetName.ToString(); }
    bool IsNull() const { return AssetName.IsNone(); }
    bool IsValid() const { return !IsNull(); }
    bool operator==(const FTopLevelAssetPath& Other) const { return PackageName == Other.PackageName && AssetName == Other.AssetName; }
    bool operator!=(const FTopLevelAssetPath& Other) const { return !(*this == Other); }
    friend uint32 GetTypeHash(const FTopLevelAssetPath& Path) { return HashCombine(GetTypeHash(Path.PackageName), GetTypeHash(Path.AssetName)); }
};
template<> struct TStructOpsTypeTraits<FTopLevelAssetPath> : TStructOpsTypeTraitsBase2<FTopLevelAssetPath>
{
    enum { WithImportTextItem = true, WithExportTextItem = true, WithIdenticalViaEquality = true };
};
using FVector3f = FVector;
using FVector3d = FVector;
using FVector2f = FVector2D;
using FVector4d = FVector4;
using FQuat4f = FQuat;
using FQuat4d = FQuat;
using FTransform3f = FTransform;
using FTransform3d = FTransform;

inline bool UE427IsEmpty(const FText& Value) { return Value.IsEmpty(); }
inline bool UE427IsEmpty(const FString& Value) { return Value.IsEmpty(); }
template<class T> bool UE427IsEmpty(const T& Value) { return Value.Num() == 0; }


constexpr EExprToken EX_DoubleConst = static_cast<EExprToken>(0x37);
constexpr EExprToken EX_Cast = EX_PrimitiveCast;

#pragma warning(disable: 4063)
