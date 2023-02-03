#pragma once

// #include "Dialogs/CustomDialog.h"
#include "Widgets/SCompoundWidget.h"

class FArguments;

class SUdSDKLoginPanel : public SCompoundWidget {
  SLATE_BEGIN_ARGS(SUdSDKLoginPanel) {}
  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs);

private:
  void LaunchBrowserAgain();

  FReply SignIn();
  FReply CopyAuthorizeUrlToClipboard();

  TArray<TSharedPtr<class SCheckBox>> CheckBoxs_LineState;

  TSharedPtr<class STextBlock> ComboText_Server;

  TArray<TSharedPtr<FString>> Options_Server;

  FString ServerName;
  FString UserName;
  FString Password;
  bool Offline;

};
