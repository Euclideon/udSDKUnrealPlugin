#pragma once

#include "Dialogs/Dialogs.h"
#include "Widgets/SCompoundWidget.h"

class FArguments;

class SUdSDKMainPanel : public SCompoundWidget {
  SLATE_BEGIN_ARGS(SUdSDKMainPanel) {}
  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs);

  virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
  TSharedRef<SWidget> Toolbar();
  TSharedRef<SWidget> LoginPanel();
  TSharedRef<SWidget> BasicQuickAddPanel();

  void addFromAssets();
  void signOut();
};
