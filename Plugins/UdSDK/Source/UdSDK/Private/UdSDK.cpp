// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDK.h"
#include "CoreMinimal.h"
#include "UdSDKFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "FUdSDKModule"

void FUdSDKModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: StartupModule()"));
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (!GEngine->IsEditor())
		UUdSDKFunctionLibrary::Login();
}

void FUdSDKModule::ShutdownModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: ShutdownModule()"));
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (!GEngine->IsEditor())
		UUdSDKFunctionLibrary::Exit();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKModule, UdSDK)