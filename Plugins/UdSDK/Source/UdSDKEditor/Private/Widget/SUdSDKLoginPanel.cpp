#include "SUdSDKLoginPanel.h"
#include "UdSDKEditorStyle.h"
#include "Settings/ObjectStorageSettings.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Misc/App.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SEditableText.h"
#include "Widgets/Layout/SScaleBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SGridPanel.h"

#include "Settings/ObjectStorageSettings.h"
#include "UdSDKFunctionLibrary.h"

template<class T>
void SaveObjectConfig()
{
    if (T* InSettingsEditor = const_cast<T*>(GetDefault<T>()))
    {
        InSettingsEditor->SaveObjectStorageConfig();
    }
}

template<class T>
void LoadObjectConfig()
{
    if (T* InSettingsEditor = const_cast<T*>(GetDefault<T>()))
    {
        InSettingsEditor->LoadObjectStorageConfig();
    }
}

// The main widget constructor for the login menu
void SUdSDKLoginPanel::Construct(const FArguments& InArgs)
{
    LoadObjectConfig<UObjectStorageSettings>();

    ServerName = GetMutableDefault<UObjectStorageSettings>()->ServerPath.ToString(); // Make sure to enforce the default of storage object to udcloud
    UserName = GetMutableDefault<UObjectStorageSettings>()->Username.ToString();
    Password = GetMutableDefault<UObjectStorageSettings>()->Password.ToString();
    Offline = GetMutableDefault<UObjectStorageSettings>()->Offline;

    CheckBoxs_LineState.SetNum(2); // Not sure why hardcoded
    // CAUTION - These might sometimes fail to update correctly when the user inputs new login data  
    if (Options_Server.Num() <= 0) {
        Options_Server.Add(MakeShareable(new FString(TEXT("https://udstream.euclideon.com.cn")))); // Should disable?
        Options_Server.Add(MakeShareable(new FString(TEXT("https://udstream.euclideon.com")))); // Should disable?
        Options_Server.Add(MakeShareable(new FString(TEXT("https://udcloud.euclideon.com")))); // UDCloud is new.
    }


  TSharedPtr<SGridPanel> loginInfomation = SNew(SGridPanel)
            .FillColumn(0, 1).FillColumn(1, 3)
            .FillRow(0, 1).FillRow(1, 1).FillRow(2, 1)
            + SGridPanel::Slot(0, 0).HAlign(HAlign_Left).VAlign(VAlign_Center).Padding(5)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("Server")))
            ]
            + SGridPanel::Slot(0, 1).HAlign(HAlign_Left).VAlign(VAlign_Center).Padding(5)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("Email")))
            ]
            + SGridPanel::Slot(0, 2).HAlign(HAlign_Left).VAlign(VAlign_Center).Padding(5)
            [
                SNew(STextBlock).Text(FText::FromString(TEXT("Password")))
            ]
            + SGridPanel::Slot(1, 0).HAlign(HAlign_Fill).VAlign(VAlign_Center).Padding(5)
            [
                SNew(SComboBox<TSharedPtr<FString>>)
                .OnGenerateWidget_Lambda(
                [](TSharedPtr<FString> Item)
                {
                    return SNew(STextBlock).Text(FText::FromString(*Item));
                })
                .OnSelectionChanged_Lambda(
                [this](TSharedPtr<FString> Item, ESelectInfo::Type Type)
                {
                    if (ComboText_Server.IsValid()) {
                        ComboText_Server->SetText(FText::FromString(*Item));
                        ServerName = *Item;
                    }
                })
                .OptionsSource(&Options_Server)
                [
                    SAssignNew(ComboText_Server, STextBlock).
                    Text(FText::FromString(*ServerName))
                ]
            ]
            + SGridPanel::Slot(1, 1).HAlign(HAlign_Fill).VAlign(VAlign_Center).Padding(5)
            [
                SNew(SEditableTextBox)
                .OnTextChanged_Lambda([this](const FText& NewText) {
                    UserName = NewText.ToString();
                })
                .Text(FText::FromString(*UserName))
            ]
            + SGridPanel::Slot(1, 2).HAlign(HAlign_Fill).VAlign(VAlign_Center).Padding(5)
            [
                SNew(SEditableTextBox)
                .OnTextChanged_Lambda([this](const FText& NewText) {
                    Password = NewText.ToString();
                })
                .Text(FText::FromString(*Password))
                    .IsPassword_Lambda([]()->bool {
                    return true;
                })
            ];
           

  TSharedPtr<SVerticalBox> connectionWidget =
      SNew(SVerticalBox) +
      SVerticalBox::Slot()
          .VAlign(VAlign_Fill)
          .HAlign(HAlign_Fill)
          .Padding(0)
          .AutoHeight()
      [
          loginInfomation.ToSharedRef()
      ]+
      SVerticalBox::Slot()
          .VAlign(VAlign_Top)
          .HAlign(HAlign_Fill)
          .Padding(5)
          .AutoHeight()
              [SNew(SButton)
                   .ButtonStyle(FUdSDKEditorStyle::Get(), "UdSDKButton")
                   .TextStyle(
                       FUdSDKEditorStyle::Get(),
                       "UdSDKButtonText")
                   .OnClicked(this, &SUdSDKLoginPanel::SignIn)
                   .HAlign(HAlign_Center)
                   .Text(FText::FromString(TEXT("Login")))
                   .IsEnabled_Lambda([this]() {
                     return true;
                   })];

  ChildSlot
      [SNew(SScrollBox) +
       SScrollBox::Slot()
           .VAlign(VAlign_Top)
           .HAlign(HAlign_Center)
           .Padding(0, 0, 0, 0)
               [SNew(SScaleBox)
                    .Stretch(EStretch::ScaleToFit)
                    .HAlign(HAlign_Center)
                    .VAlign(VAlign_Top)[SNew(SImage).Image(
                        FUdSDKEditorStyle::Get()->GetBrush(
                            TEXT("UdSDK.Logo")))]] +
     SScrollBox::Slot()
           .VAlign(VAlign_Top)
           .Padding(0)[connectionWidget.ToSharedRef()]];
}

FReply SUdSDKLoginPanel::SignIn() 
{
    UE_LOG(LogTemp, Display, TEXT("Attemping SignIn ..."));
    GetMutableDefault<UObjectStorageSettings>()->ServerPath = FName(*ServerName);
    GetMutableDefault<UObjectStorageSettings>()->Username = FName(*UserName);
    GetMutableDefault<UObjectStorageSettings>()->Password = FName(*Password);
    GetMutableDefault<UObjectStorageSettings>()->Offline = Offline;
    SaveObjectConfig<UObjectStorageSettings>();

   
    if (!UUdSDKFunctionLibrary::IsLogin())
    {
        
        UUdSDKFunctionLibrary::Login();
    }

    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UUdSDKFunctionLibrary::IsLogin() returning true"));
    }
        

  return FReply::Handled();
}

FReply SUdSDKLoginPanel::CopyAuthorizeUrlToClipboard() {
  FText url = FText::FromString(TEXT("CopyAuthorizeUrlToClipboard"));
  FPlatformApplicationMisc::ClipboardCopy(*url.ToString());
  return FReply::Handled();
}

void SUdSDKLoginPanel::LaunchBrowserAgain() {
  FPlatformProcess::LaunchURL(
      TEXT("LaunchBrowserAgain"),
      NULL,
      NULL);
}
