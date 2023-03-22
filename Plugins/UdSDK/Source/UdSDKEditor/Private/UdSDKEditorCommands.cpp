// Copyright Epic Games, Inc. All Rights Reserved.

#include "UdSDKEditorCommands.h"

#define LOCTEXT_NAMESPACE "FUdSDKEditorModule"

void FUdSDKEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenMainPanel,
        "UdSDK", 
        "Open the main panel",
        EUserInterfaceActionType::Button,
        FInputGesture());

    UI_COMMAND(
        SignOut,
        "Sign Out",
        "Sign out of UdSDK",
        EUserInterfaceActionType::Button,
        FInputChord());
}

#undef LOCTEXT_NAMESPACE
