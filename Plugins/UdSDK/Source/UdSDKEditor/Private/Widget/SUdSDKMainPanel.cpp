#include "SUdSDKMainPanel.h"
#include "UdSDKEditorStyle.h"
#include "UdSDKEditorCommands.h"
#include "SUdSDKAssetsPanel.h"
#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SUdSDKLoginPanel.h"
#include "SUdSDKQuickAddPanel.h"
#include "LevelEditor.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SScrollBox.h"
#include "UDSubsystem.h"

void SUdSDKMainPanel::Construct(const FArguments& InArgs) {
  ChildSlot
      [SNew(SVerticalBox) + SVerticalBox::Slot().AutoHeight()[Toolbar()] +
       SVerticalBox::Slot().VAlign(VAlign_Fill)
           [SNew(SScrollBox) + SScrollBox::Slot()[BasicQuickAddPanel()] +
            SScrollBox::Slot()[LoginPanel()]
           ]];
}

void SUdSDKMainPanel::Tick(
    const FGeometry& AllottedGeometry,
    const double InCurrentTime,
    const float InDeltaTime) {
  SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

static bool isSignedIn() {
    UUDSubsystem* MySubsystem = GEngine->GetEngineSubsystem<UUDSubsystem>();
    return MySubsystem->IsLogin();
}

TSharedRef<SWidget> SUdSDKMainPanel::Toolbar() {
  TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();

  commandList->MapAction(
      FUdSDKEditorCommands::Get().SignOut,
      FExecuteAction::CreateSP(this, &SUdSDKMainPanel::signOut),
      FCanExecuteAction::CreateStatic(isSignedIn));

  FToolBarBuilder builder(commandList, FMultiBoxCustomization::None);
  builder.AddToolBarButton(FUdSDKEditorCommands::Get().SignOut);

  return builder.MakeWidget();
}

TSharedRef<SWidget> SUdSDKMainPanel::LoginPanel() {
  return SNew(SUdSDKLoginPanel).Visibility_Lambda([]() {
    return isSignedIn() ? EVisibility::Collapsed : EVisibility::Visible;
  });
}


TSharedRef<SWidget> SUdSDKMainPanel::BasicQuickAddPanel() {
  TSharedPtr<SUdSDKQuickAddPanel> quickAddPanel =
      SNew(SUdSDKQuickAddPanel).Title(FText::FromString("Add Actors to the level"));
  quickAddPanel->AddItem(
      {QuickAddItemType::EPointCloud,
       "UDS",
       "A Euclideon Unlimited Detail Point Cloud file.",
       ""
      });
  quickAddPanel->AddItem(
      {QuickAddItemType::EPointOfInterest,
       "POI",
       "A point, line or region describing a location of interest.",
       ""
      });
  quickAddPanel->AddItem(QuickAddItem{
      QuickAddItemType::EPolygon,
      "Polygon",
      "A polygon model, usually an OBJ or FBX.",
      ""
      });
  return quickAddPanel.ToSharedRef();
}

void SUdSDKMainPanel::addFromAssets() {
  FLevelEditorModule* pLevelEditorModule =
      FModuleManager::GetModulePtr<FLevelEditorModule>(
          FName(TEXT("LevelEditor")));
  TSharedPtr<FTabManager> pTabManager =
      pLevelEditorModule ? pLevelEditorModule->GetLevelEditorTabManager()
                         : FGlobalTabmanager::Get();
  pTabManager->TryInvokeTab(FTabId(TEXT("UdSDKAssets")));
}

void SUdSDKMainPanel::signOut() {
    // Do nothing
}

