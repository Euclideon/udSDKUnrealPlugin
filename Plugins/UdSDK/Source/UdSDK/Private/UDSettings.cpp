// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "UDSettings.h"
#include "UDSubsystem.h"

void UUDSettings::SaveObjectStorageConfig()
{
	FString SettingsEditorConfigPath = FPaths::ProjectConfigDir() / TEXT("Default") + GetClass()->ClassConfigName.ToString() + TEXT(".ini");
	SaveConfig(CPF_Config, *SettingsEditorConfigPath);
}

void UUDSettings::LoadObjectStorageConfig()
{
	FString SettingsEditorConfigPath = FPaths::ProjectConfigDir() / TEXT("Default") + GetClass()->ClassConfigName.ToString() + TEXT(".ini");
	LoadConfig(GetClass(), *SettingsEditorConfigPath);
}
