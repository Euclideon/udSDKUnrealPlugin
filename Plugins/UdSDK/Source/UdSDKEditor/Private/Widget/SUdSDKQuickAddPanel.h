#pragma once

// #include "Dialogs/CustomDialog.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableRow.h"
#include <string>
#include <unordered_set>

class FArguments;

enum class QuickAddItemType { EPointCloud, EPointOfInterest, EPolygon };

struct QuickAddItem
{
  QuickAddItemType type;
  std::string name{};
  std::string description;
  std::string url;
};

class SUdSDKQuickAddPanel : public SCompoundWidget {
  SLATE_BEGIN_ARGS(SUdSDKQuickAddPanel) {}
  SLATE_ARGUMENT(FText, Title)
  SLATE_END_ARGS()

  void Construct(const FArguments& InArgs);

  void AddItem(const QuickAddItem& item);

private:
  TSharedRef<SWidget> QuickAddList();
  TSharedRef<ITableRow> CreateQuickAddItemRow(
      TSharedRef<QuickAddItem> item,
      const TSharedRef<STableViewBase>& list);

  void AddItemToLevel(TSharedRef<QuickAddItem> item);
  void AddBlankPointCloudToLevel();
  void AddPointCloudToLevel(TSharedRef<QuickAddItem> item);
  void AddPointOfInterestToLevel();
  void AddPolygonToLevel();

  TArray<TSharedRef<QuickAddItem>> QuickAddItems;
  std::unordered_set<std::string> ItemsBeingAdded;
};
