// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDK.h"

#include <udContext.h>

#include "Interfaces/IPluginManager.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FUdSDKModule"

// Called when the module is loaded by the engine
void FUdSDKModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: StartupModule()"));
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UdSDK"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/UdSDK"), PluginShaderDir);
}

void FUdSDKModule::ShutdownModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKModule, UdSDK)