// Copyright Epic Games, Inc. All Rights Reserved.

#include "ObjectStorageSettingsDetails.h"
#include "UDSettings.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"

#define LOCTEXT_NAMESPACE "CryptoKeysSettingsDetails"

TSharedRef<IDetailCustomization> FObjectStorageSettingsDetails::MakeInstance()
{
	return MakeShareable(new FObjectStorageSettingsDetails);
}

void FObjectStorageSettingsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);
	check(ObjectsBeingCustomized.Num() == 1);
	TWeakObjectPtr<UUDSettings> Settings = Cast<UUDSettings>(ObjectsBeingCustomized[0].Get());

	IDetailCategoryBuilder& EncryptionCategory = DetailBuilder.EditCategory("UdSDK login information");

	EncryptionCategory.AddCustomRow(LOCTEXT("EncryptionKeyGenerator", "EncryptionKeyGenerator"))
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(5)
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("RefreshData", "Refresh Data"))
				.ToolTipText(LOCTEXT("RefreshData_Tooltip", "Update data to the scene"))
				.OnClicked_Lambda([this, Settings]()
				{
					return(FReply::Handled());
				})
			]
		];
}

#undef LOCTEXT_NAMESPACE