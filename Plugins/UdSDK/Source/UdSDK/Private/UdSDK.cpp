// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDK.h"

#include <udContext.h>

#include "CoreMinimal.h"
#include "UdSDKFunctionLibrary.h"

#define LOCTEXT_NAMESPACE "FUdSDKModule"

// Called when the module is loaded by the engine
void FUdSDKModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: StartupModule()"));

	// FPlatformProcess::GetDllHandle() - Upon calling GetDllHandle() the engine will load the DLL.
	// *DLLHandle = FPlatformProcess::GetDllHandle(Path);
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	if (!GEngine->IsEditor())
	{
		UUdSDKFunctionLibrary::Login();
	}

	// delete me 
	// FString debugString = "asdasdasd";

	// auto test = udContext_ConnectLegacy(nullptr, TCHAR_TO_UTF8(*debugString), "UE5_Client", TCHAR_TO_UTF8(*debugString), TCHAR_TO_UTF8(*debugString));
}

void FUdSDKModule::ShutdownModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: ShutdownModule()"));

	// Make sure to call FPlatformProcess::FreeDllHandle(DLLHandle); here
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	if (!GEngine->IsEditor())
		UUdSDKFunctionLibrary::Exit();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKModule, UdSDK)