#pragma once
#include "Interfaces/IPluginManager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
inline FString AlpakitDescriptorText(const FPluginDescriptor& Descriptor)
{
    TSharedRef<FJsonObject> Json = MakeShared<FJsonObject>();
    if (Descriptor.CachedJson.IsValid()) Json->Values = Descriptor.CachedJson->Values;
    Descriptor.UpdateJson(*Json);
    FString Text;
    FJsonSerializer::Serialize(Json, TJsonWriterFactory<>::Create(&Text));
    return Text;
}
inline bool AlpakitUpdateDescriptor(TSharedRef<IPlugin> Plugin, const FPluginDescriptor& Descriptor, FText& Error)
{
    if (!FFileHelper::SaveStringToFile(AlpakitDescriptorText(Descriptor), *Plugin->GetDescriptorFileName()))
    {
        Error = FText::FromString(TEXT("Could not save the plugin descriptor."));
        return false;
    }
    return Plugin->UpdateDescriptor(Descriptor, Error);
}
