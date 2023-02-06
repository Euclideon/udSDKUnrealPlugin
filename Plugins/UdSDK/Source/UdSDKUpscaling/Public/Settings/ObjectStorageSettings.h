// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/ObjectStorageSettingsBase.h"
#include "ObjectStorageSettings.generated.h"

UCLASS(config = ObjectStorageSettings)
class UDSDKUPSCALING_API UObjectStorageSettings : public UObjectStorageSettingsBase
{
	GENERATED_BODY()

public:
	UObjectStorageSettings();

	UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	FName ServerPath;

	UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	FName Username;

	UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	FName Password;

	UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	FColor SelectColor;

	//UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	//FName ModelPath;

	UPROPERTY(config, EditAnywhere, Category = "UdSDK login information", meta = (ToolTip = ""))
	bool Offline;

	
	void RefreshData();
	//UPROPERTY(config,VisibleDefaultsOnly, Category = GeneralSettings, meta = (ToolTip = "Specify the address to upload."))
	//FName LinkURL;
	//
	//UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Specify the address to upload."))
	//bool bInstallContentPak;
	//
	///*ÏîÄ¿Â·¾¶ + ×Ô¶¨ÒåµÄÂ·¾¶*/
	//UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Specify the address to upload.", EditCondition = "!bInstallContentPak"))
	//FString RelativeInstallationLocation;

};