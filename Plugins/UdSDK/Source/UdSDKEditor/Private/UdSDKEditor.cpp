// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDKEditor.h"
#include "UdSDKEditorStyle.h"
#include "UdSDKEditorCommands.h"
#include "SUdSDKMainPanel.h"
#include "SUdSDKAssetsPanel.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Settings/ObjectStorageSettings.h"
#include "ObjectStorageSettings/ObjectStorageSettingsDetails.h"
#include "ISettingsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Framework/Docking/LayoutExtender.h"
#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "LevelEditor.h"
#include "UdSDKEditor.h"


static const FName UdSDKEditorTabName("UdSDKEditor");

#define LOCTEXT_NAMESPACE "FUdSDKEditorModule"

void FUdSDKEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FUdSDKEditorStyle::Initialize();
	FUdSDKEditorStyle::ReloadTextures();

	FUdSDKEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FUdSDKEditorModule::RegisterMenus));

	FGlobalTabmanager::Get()
		->RegisterNomadTabSpawner(
			TEXT("UdSDK"),
			FOnSpawnTab::CreateRaw(this, &FUdSDKEditorModule::SpawnUdSDKTab))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
		.SetDisplayName(FText::FromString(TEXT("UdSDK")))
		.SetTooltipText(FText::FromString(TEXT("UdSDK")))
		.SetIcon(FSlateIcon(FUdSDKEditorStyle::GetStyleSetName(), TEXT("UdSDK.MenuIcon")));


	FLevelEditorModule* pLevelEditorModule = 
		FModuleManager::GetModulePtr<FLevelEditorModule>(
			FName(TEXT("LevelEditor")));
	if (pLevelEditorModule) 
	{
		pLevelEditorModule->OnRegisterLayoutExtensions().AddLambda(
			[](FLayoutExtender& extender) {
			extender.ExtendLayout(
				FTabId("PlacementBrowser"),
				ELayoutExtensionPosition::After,
				FTabManager::FTab(FName("UdSDK"), ETabState::OpenedTab));
		});

		PluginCommands->MapAction(
			FUdSDKEditorCommands::Get().OpenMainPanel,
			FExecuteAction::CreateLambda([]() {
			FLevelEditorModule* pLevelEditorModule =
				FModuleManager::GetModulePtr<FLevelEditorModule>(
					FName(TEXT("LevelEditor")));
			TSharedPtr<FTabManager> pTabManager = 
				pLevelEditorModule 
				? pLevelEditorModule->GetLevelEditorTabManager() 
				: FGlobalTabmanager::Get();
			pTabManager->TryInvokeTab(FTabId(TEXT("UdSDK")));
		}));
	}
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		SettingsModule->RegisterSettings("Project", "UdSDK Configuration", "GetLicenseInfomation",
			LOCTEXT("GetLicenseInfomation", "Get License Infomation"),
			LOCTEXT("GetLicenseInfomationTip", "Package settings for the current platform you have set."),
			GetMutableDefault<UObjectStorageSettings>());
	}

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout(UObjectStorageSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FObjectStorageSettingsDetails::MakeInstance));

}

void FUdSDKEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FUdSDKEditorStyle::Shutdown();

	FUdSDKEditorCommands::Unregister();

	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.UnregisterCustomClassLayout(UObjectStorageSettings::StaticClass()->GetFName());

}

void FUdSDKEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FUdSDKEditorCommands::Get().OpenMainPanel, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FUdSDKEditorCommands::Get().OpenMainPanel));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

TSharedRef<SDockTab> FUdSDKEditorModule::SpawnUdSDKTab(const FSpawnTabArgs& TabSpawnArgs)
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab).TabRole(ETabRole::NomadTab)[SNew(SUdSDKMainPanel)];
	return SpawnedTab;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKEditorModule, UdSDKEditor)