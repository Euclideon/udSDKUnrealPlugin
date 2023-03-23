// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ObjectStorageSettingsBase.h"
#include "ObjectStorageSettings.generated.h"

UCLASS(config = ObjectStorageSettings)
class UDSDKUPSCALING_API UObjectStorageSettings : public UObjectStorageSettingsBase
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "UnlimitedDetail|Licensing", meta = (ToolTip = ""))
	FName ServerPath = FName("");

	UPROPERTY(config, EditAnywhere, Category = "UnlimitedDetail|Licensing", meta = (ToolTip = ""))
	FName Password = FName("");
};