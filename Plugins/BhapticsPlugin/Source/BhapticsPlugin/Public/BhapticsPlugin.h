//Copyright bHaptics Inc. All Rights Reserved 2022~

#pragma once

#include "Modules/ModuleManager.h"

class FBhapticsPluginModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void* BhapticsLibraryHandle = nullptr;
	bool LoadDll();
};
