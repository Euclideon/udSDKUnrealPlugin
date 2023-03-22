
#include "SUdSDKQuickAddPanel.h"
#include "UdSDKEditorStyle.h"
#include "Editor.h"
#include "PropertyCustomizationHelpers.h"
#include "Styling/SlateStyle.h"
#include "Widgets/Images/SThrobber.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"

void SUdSDKQuickAddPanel::Construct(const FArguments& InArgs) {
  ChildSlot
      [SNew(SVerticalBox) +
       SVerticalBox::Slot()
           .VAlign(VAlign_Top)
           .AutoHeight()
           .Padding(FMargin(5.0f, 20.0f, 5.0f, 10.0f))
               [SNew(SHeader).Content()
                    [SNew(STextBlock)
                         .TextStyle(FUdSDKEditorStyle::Get(), "Heading")
                         .Text(InArgs._Title)]] +
       SVerticalBox::Slot()
           .VAlign(VAlign_Top)
           .Padding(FMargin(5.0f, 0.0f, 5.0f, 20.0f))[this->QuickAddList()]];
}

void SUdSDKQuickAddPanel::AddItem(const QuickAddItem& item) {
  QuickAddItems.Add(MakeShared<QuickAddItem>(item));
}

TSharedRef<SWidget> SUdSDKQuickAddPanel::QuickAddList() {
  return SNew(SListView<TSharedRef<QuickAddItem>>)
      .SelectionMode(ESelectionMode::None)
      .ListItemsSource(&QuickAddItems)
      .OnMouseButtonDoubleClick(this, &SUdSDKQuickAddPanel::AddItemToLevel)
      .OnGenerateRow(this, &SUdSDKQuickAddPanel::CreateQuickAddItemRow);
}

TSharedRef<ITableRow> SUdSDKQuickAddPanel::CreateQuickAddItemRow(TSharedRef<QuickAddItem> item, const TSharedRef<STableViewBase>& list)
{
  return SNew(STableRow<TSharedRef<QuickAddItem>>, list).Content()
  [
    SNew(SBox)
      .HAlign(EHorizontalAlignment::HAlign_Fill)
      .HeightOverride(40.0f)
      .Content()
      [
        SNew(SHorizontalBox) + 
          SHorizontalBox::Slot()
            .FillWidth(1.0f)
            .Padding(5.0f)
            .VAlign(EVerticalAlignment::VAlign_Center)
            [
              SNew(STextBlock)
                .AutoWrapText(true)
                .Text(FText::FromString(UTF8_TO_TCHAR(item->name.c_str())))
                .ToolTipText(FText::FromString(UTF8_TO_TCHAR(item->description.c_str())))
            ] +
          SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign(EVerticalAlignment::VAlign_Center)
            [
              PropertyCustomizationHelpers::MakeNewBlueprintButton(
                FSimpleDelegate::CreateLambda(
                  [this, item]() { this->AddItemToLevel(item); }),
                FText::FromString(
                  TEXT("Add this item to the level")),
                  TAttribute<bool>::Create([this, item]() {
                    return this->ItemsBeingAdded.find(item->name) ==
                            this->ItemsBeingAdded.end();
                  })
              )
            ]
      ]
  ];
}

namespace {

void showAssetDepotConfirmWindow(
    const FString& itemName,
    int64_t missingAsset) {

  TSharedRef<SWindow> AssetDepotConfirmWindow =
    SNew(SWindow)
      .Title(FText::FromString(TEXT("Asset is not available in My Assets")))
      .ClientSize(FVector2D(400.0f, 200.0f))
      .Content()
      [
        SNew(SVerticalBox) +
          SVerticalBox::Slot().AutoHeight().Padding(10.0f)
          [
            SNew(STextBlock)
              .AutoWrapText(true)
              .Text(FText::FromString(TEXT("Before " + itemName +
                " can be added to your level, it must be added to \"My Assets\" in your UdSDK account.")))
          ] +
          SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(EHorizontalAlignment::HAlign_Left)
            .Padding(10.0f, 5.0f)
          [
            SNew(SHyperlink)
              .OnNavigate_Lambda([missingAsset]() {
                FPlatformProcess::LaunchURL(
                    UTF8_TO_TCHAR(
                        ("https://UdSDK.com/ion/assetdepot/" +
                          std::to_string(missingAsset))
                            .c_str()),
                    NULL,
                    NULL);
              })
              .Text(FText::FromString(TEXT(
                  "Open this asset in the UdSDK Asset Depot")))
          ] +
          SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(EHorizontalAlignment::HAlign_Left)
            .Padding(10.0f, 5.0f)
          [
            SNew(STextBlock).Text(FText::FromString(TEXT(
              "Click \"Add to my assets\" in the UdSDK web page")))
          ] +
          SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(EHorizontalAlignment::HAlign_Left)
            .Padding(10.0f, 5.0f)
          [
            SNew(STextBlock)
              .Text(FText::FromString(TEXT(
                "Return to UdSDK for Unreal and try adding this asset again")))
          ] +
          SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(EHorizontalAlignment::HAlign_Center)
            .Padding(10.0f, 25.0f)
          [
            SNew(SButton)
              .OnClicked_Lambda(
                  [&AssetDepotConfirmWindow]() {
                    AssetDepotConfirmWindow
                        ->RequestDestroyWindow();
                    return FReply::Handled();
                  })
              .Text(FText::FromString(TEXT("Close")))
          ]
      ];

  GEditor->EditorAddModalWindow(AssetDepotConfirmWindow);
}
} // namespace

void SUdSDKQuickAddPanel::AddPointCloudToLevel(TSharedRef<QuickAddItem> item) {

}

void SUdSDKQuickAddPanel::AddBlankPointCloudToLevel()
{
  UWorld* pCurrentWorld = GEditor->GetEditorWorldContext().World();
  ULevel* pCurrentLevel = pCurrentWorld->GetCurrentLevel();
  
  //GEditor->AddActor(pCurrentLevel, AUdPointCloud::StaticClass(), FTransform(), false, RF_Public | RF_Transactional);
}

void SUdSDKQuickAddPanel::AddItemToLevel(TSharedRef<QuickAddItem> item)
{
    if (this->ItemsBeingAdded.find(item->name) != this->ItemsBeingAdded.end())
    {
        return;
    }
    this->ItemsBeingAdded.insert(item->name);

    if (item->type == QuickAddItemType::EPointCloud)
    {
        bool isBlankPointCloud = item->type == QuickAddItemType::EPointCloud && item->url.empty();
        if (isBlankPointCloud)
        {
            AddBlankPointCloudToLevel();
            this->ItemsBeingAdded.erase(item->name);
        }
        else
        {
            AddPointCloudToLevel(item);
        }
    }
}
