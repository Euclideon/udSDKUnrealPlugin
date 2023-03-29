// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDKEditor.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "UDSettings.h"
#include "ObjectStorageSettingsDetails.h"
#include "ISettingsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "UdSDKEditor.h"


static const FName UdSDKEditorTabName("UnlimitedDetailEditor");

#define LOCTEXT_NAMESPACE "FUdSDKEditorModule"

void FUdSDKEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "udSDK / Unlimited Detail",
			LOCTEXT("udSDKInformation", "Unlimited Detail (udSDK)"),
			LOCTEXT("udSDKInformationTip", "Settings for the Unlimited Detail Plugin"),
			GetMutableDefault<UUDSettings>());
	}

	//FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomClassLayout(UObjectStorageSettings::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FObjectStorageSettingsDetails::MakeInstance));
}

void FUdSDKEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading, we call this function before unloading the module.

	//FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.UnregisterCustomClassLayout(UObjectStorageSettings::StaticClass()->GetFName());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKEditorModule, UdSDKEditor)