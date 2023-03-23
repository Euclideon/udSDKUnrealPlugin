// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDK.h"

#include <udContext.h>

#include "CoreMinimal.h"

#define LOCTEXT_NAMESPACE "FUdSDKModule"

// Called when the module is loaded by the engine
void FUdSDKModule::StartupModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: StartupModule()"));
}

void FUdSDKModule::ShutdownModule()
{
	UE_LOG(LogTemp, Display, TEXT("UDSDK: ShutdownModule()"));
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FUdSDKModule, UdSDK)