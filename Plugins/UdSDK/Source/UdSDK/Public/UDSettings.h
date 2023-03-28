// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UDSettings.generated.h"

UCLASS(config = ObjectStorageSettings)
class UDSDK_API UUDSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "UnlimitedDetail", meta = (ToolTip = ""))
	FName ServerPath = FName("");

	UPROPERTY(config, EditAnywhere, Category = "UnlimitedDetail", meta = (ToolTip = ""))
	FName APIKey = FName("");

	virtual void SaveObjectStorageConfig();
	virtual void LoadObjectStorageConfig();
};