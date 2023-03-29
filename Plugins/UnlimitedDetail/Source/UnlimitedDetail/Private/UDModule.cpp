// Copyright Epic Games, Inc. All Rights Reserved.

#include "UDModule.h"

#include <udContext.h>

#include "Interfaces/IPluginManager.h"
#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FUDModule"

// Called when the module is loaded by the engine
void FUDModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | StartupModule()"));
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UnlimitedDetail"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugins/UnlimitedDetail"), PluginShaderDir);
}

void FUDModule::ShutdownModule()
{
	UE_LOG(LogTemp, Display, TEXT("UnlimitedDetail | ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUDModule, UnlimitedDetail)