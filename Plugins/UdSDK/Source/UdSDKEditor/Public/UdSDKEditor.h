// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "AssetTools/Public/IAssetTools.h"

class FToolBarBuilder;
class FMenuBuilder;



class FUdSDKEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void RegisterMenus();

	TSharedRef<class SDockTab> SpawnUdSDKTab(const class FSpawnTabArgs& TabSpawnArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
