// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "UdSDKEditorStyle.h"

class FUdSDKEditorCommands : public TCommands<FUdSDKEditorCommands>
{
public:

	FUdSDKEditorCommands()
		: TCommands<FUdSDKEditorCommands>(TEXT("UdSDK.Common"), NSLOCTEXT("Common", "Common", ""), NAME_None, FUdSDKEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenMainPanel;
	TSharedPtr<FUICommandInfo> SignOut;
};
