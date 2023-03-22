#include "SUdSDKAssetsPanel.h"
#include "UdSDKEditorStyle.h"
#include "Editor.h"
#include "EditorModeManager.h"
#include "EngineUtils.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "SUdSDKLoginPanel.h"
#include "SUdSDKQuickAddPanel.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SHeader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Views/SListView.h"
#include "Utils/JsonLoader.h"
#include "UdSDKFunctionLibrary.h"
#include <functional>
#include <map>

static FName ColumnName_Name = "Name";
static FName ColumnName_Type = "Type";
static FName ColumnName_DateAdded = "DateAdded";


SUdSDKAssetsPanel::SUdSDKAssetsPanel() : LoginDelegateHandle(), ExitDelegateHandle(), PListView(nullptr), Assets(), PSelection(nullptr) 
{
    this->SortColumnName = ColumnName_DateAdded;
    this->SortMode = EColumnSortMode::Type::Descending;

    LoginDelegateHandle = UUdSDKFunctionLibrary::AddLoginDelegateLambda([this] {
        this->Refresh();
    });

    ExitDelegateHandle = UUdSDKFunctionLibrary::AddExitDelegateLambda([this] {
        this->Refresh();
    });
}

SUdSDKAssetsPanel::~SUdSDKAssetsPanel()
{
    UUdSDKFunctionLibrary::DeleteLoginDelegateHandle(LoginDelegateHandle);
    UUdSDKFunctionLibrary::DeleteExitDelegateHandle(ExitDelegateHandle);
}

void SUdSDKAssetsPanel::Construct(const FArguments& InArgs) 
{
  auto sortModeLambda = [this](const FName& columnName) 
  {
    return [this, columnName]() {
      if (SortColumnName != columnName) {
        return EColumnSortMode::None;
      }
      return SortMode;
    };
  };

  this->PListView =
      SNew(SListView<TSharedPtr<FUdAsset>>)
          .ListItemsSource(&this->Assets)
          .OnMouseButtonDoubleClick(this, &SUdSDKAssetsPanel::AddAsset)
          .OnGenerateRow(this, &SUdSDKAssetsPanel::CreateAssetRow)
          .OnSelectionChanged(this, &SUdSDKAssetsPanel::AssetSelected)
          .HeaderRow(
              SNew(SHeaderRow) +
              SHeaderRow::Column(ColumnName_Name)
                  .DefaultLabel(FText::FromString(TEXT("Name")))
                  .SortMode_Lambda(sortModeLambda(ColumnName_Name))
                  .OnSort(FOnSortModeChanged::CreateSP(
                      this,
                      &SUdSDKAssetsPanel::OnSortChange)) +
              SHeaderRow::Column(ColumnName_Type)
                  .DefaultLabel(FText::FromString(TEXT("Type")))
                  .SortMode_Lambda(sortModeLambda(ColumnName_Type))
                  .OnSort(FOnSortModeChanged::CreateSP(
                      this,
                      &SUdSDKAssetsPanel::OnSortChange)) +
              SHeaderRow::Column(ColumnName_DateAdded)
                  .DefaultLabel(FText::FromString(TEXT("Date added")))
                  .SortMode_Lambda(sortModeLambda(ColumnName_DateAdded))
                  .OnSort(FOnSortModeChanged::CreateSP(
                      this,
                      &SUdSDKAssetsPanel::OnSortChange)));

  TSharedPtr<SWidget> pDetails = this->AssetDetails();

  ChildSlot[
    SNew(SSplitter).Orientation(EOrientation::Orient_Horizontal) +
      SSplitter::Slot().Value(0.66f)
        [
          SNew(SVerticalBox) +
            SVerticalBox::Slot().AutoHeight()
            [
              SNew(SHorizontalBox) +
                SHorizontalBox::Slot().HAlign(HAlign_Left).Padding(5.0f)
                [
                  SNew(SButton)
                    .ButtonStyle(FUdSDKEditorStyle::Get(), "UdSDKButton")
                    .TextStyle(
                      FUdSDKEditorStyle::Get(), "UdSDKButtonText")
                    .ContentPadding(FMargin(1.0, 1.0))
                    .HAlign(EHorizontalAlignment::HAlign_Center)
                    .Text(FText::FromString(TEXT("Refresh")))
                    .ToolTipText(FText::FromString(TEXT("Refresh the asset list")))
                    .OnClicked_Lambda([this]() {
                      Refresh();
                      return FReply::Handled();
                    })
                    [
                      SNew(SImage).Image(
                        FUdSDKEditorStyle::Get()->GetBrush(
                          TEXT("UdSDK.Common.Refresh")))
                    ]
                ] +
                SHorizontalBox::Slot().HAlign(HAlign_Right).Padding(5.0f)
                [
                  SAssignNew(SearchBox, SSearchBox).OnTextChanged(
                      this, &SUdSDKAssetsPanel::OnSearchTextChange)
                    .MinDesiredWidth(200.f)
                ]
            ] +
            SVerticalBox::Slot()
            [
              this->PListView.ToSharedRef()
            ]
          ] +
          SSplitter::Slot().Value(0.34f)
          [
            SNew(SBorder).Padding(10)
            [
              pDetails.ToSharedRef()
            ]
          ]
        ];

}

void SUdSDKAssetsPanel::OnSortChange(const EColumnSortPriority::Type SortPriority, const FName& ColumnName, const EColumnSortMode::Type Mode)
{
    if (SortColumnName == ColumnName)
    {
        if (SortMode == EColumnSortMode::Type::None)
        {
            SortMode = EColumnSortMode::Type::Ascending;
        }
        else if (SortMode == EColumnSortMode::Type::Ascending) 
        {
            SortMode = EColumnSortMode::Type::Descending;
        }
        else 
        {
            SortMode = EColumnSortMode::Type::None;
        }
    }
    else 
    {
        SortColumnName = ColumnName;
        SortMode = EColumnSortMode::Type::Ascending;
    }
    Refresh();
}

void SUdSDKAssetsPanel::OnSearchTextChange(const FText& SearchText)
{
    SearchString = SearchText.ToString().TrimStartAndEnd();
    Refresh();
}

static bool isSupportedPointCloud(const TSharedPtr<FUdAsset>& pAsset) 
{
    return pAsset && (pAsset->type == "UDS");
}

TSharedRef<SWidget> SUdSDKAssetsPanel::AssetDetails() 
{
  return SNew(SScrollBox).Visibility_Lambda([this]() {
    return this->PSelection ? EVisibility::Visible : EVisibility::Collapsed;
  }) +
         SScrollBox::Slot().Padding(
             10,
             10,
             10,
             0)[SNew(STextBlock)
                    .AutoWrapText(true)
                    .TextStyle(FUdSDKEditorStyle::Get(), "Heading")
                    .Text_Lambda([this]() {
                      return FText::FromString(
                          this->PSelection->name);
                    })] +
         SScrollBox::Slot()
             .Padding(10, 5, 10, 10)
             .HAlign(EHorizontalAlignment::HAlign_Fill)
                 [SNew(STextBlock).Text_Lambda([this]() {
                   return FText::FromString(UTF8_TO_TCHAR(
                       (std::string("(ID: ") +
                        std::to_string(this->PSelection->id) + ")")
                           .c_str()));
                 })] +
         SScrollBox::Slot().Padding(10).HAlign(
             EHorizontalAlignment::HAlign_Center)
             [SNew(SButton)
                  .ButtonStyle(FUdSDKEditorStyle::Get(), "UdSDKButton")
                  .TextStyle(
                      FUdSDKEditorStyle::Get(),
                      "UdSDKButtonText")
                  .Visibility_Lambda([this]() {
                    return isSupportedPointCloud(this->PSelection)
                               ? EVisibility::Visible
                               : EVisibility::Collapsed;
                  })
                  .HAlign(EHorizontalAlignment::HAlign_Center)
                  .Text(FText::FromString(TEXT("Add to Level")))
                  .OnClicked_Lambda([this]() {
                    this->AddAssetToLevel(this->PSelection);
                    return FReply::Handled();
                  })] +
         SScrollBox::Slot().Padding(10).HAlign(
             EHorizontalAlignment::HAlign_Fill)
             [SNew(STextBlock)
                  .TextStyle(
                      FUdSDKEditorStyle::Get(),
                      "AssetDetailsFieldHeader")
                  .Text(FText::FromString(TEXT("Description")))] +
         SScrollBox::Slot().Padding(
             10,
             0)[SNew(STextBlock)
                    .AutoWrapText(true)
                    .TextStyle(
                        FUdSDKEditorStyle::Get(),
                        "AssetDetailsFieldValue")
                    .Text_Lambda([this]() {
                      return FText::FromString(
                          this->PSelection->description);
                    })] +
         SScrollBox::Slot().Padding(10).HAlign(
             EHorizontalAlignment::HAlign_Fill)
             [SNew(STextBlock)
                  .TextStyle(
                      FUdSDKEditorStyle::Get(),
                      "AssetDetailsFieldHeader")
                  .Text(FText::FromString(TEXT("Attribution")))] +
         SScrollBox::Slot().Padding(
             10,
             0)[SNew(STextBlock)
                    .AutoWrapText(true)
                    .TextStyle(
                        FUdSDKEditorStyle::Get(),
                        "AssetDetailsFieldValue")
                    .Text_Lambda([this]() {
                        return FText::FromString(
                            *this->PSelection->attribution);
                    })];
}

void SUdSDKAssetsPanel::ApplyFilter() 
{
	
  if (SearchString.IsEmpty()) 
  {
    return;
  }
  this->Assets = this->Assets.FilterByPredicate([this](const TSharedPtr<FUdAsset>& Asset){
      FString Name = Asset->name;
      if (Name.Contains(SearchString))
      {
          return true;
      }
      FString Description = Asset->description;
      if (Description.Contains(SearchString))
      {
          return true;
      }
      return false;
  });
}


static std::function<bool(const TSharedPtr<FUdAsset>&, const TSharedPtr<FUdAsset>&)>comparatorFor(const FName& columnName)
{
    if (columnName == ColumnName_Type)
    {
        return [](const TSharedPtr<FUdAsset>& a0, const TSharedPtr<FUdAsset>& a1)
        {
            return a0->type < a1->type;
        };
    }
    if (columnName == ColumnName_DateAdded)
    {
        return [](const TSharedPtr<FUdAsset>& a0, const TSharedPtr<FUdAsset>& a1)
        {
            return a0->dateAdded < a1->dateAdded;
        };
    }
    return [](const TSharedPtr<FUdAsset>& a0, const TSharedPtr<FUdAsset>& a1)
    {
        return a0->name < a1->name;
    };
}

void SUdSDKAssetsPanel::ApplySorting() 
{
    if (SortMode == EColumnSortMode::Type::None) 
    {
        return;
    }
    auto baseComparator = comparatorFor(SortColumnName);
    if (SortMode == EColumnSortMode::Type::Ascending) 
    {
        this->Assets.Sort(baseComparator);
    }
    else {
        this->Assets.Sort(
            [&baseComparator](
                const TSharedPtr<FUdAsset>& a0,
                const TSharedPtr<FUdAsset>& a1) { return baseComparator(a1, a0); });
    }
}

void SUdSDKAssetsPanel::Refresh() 
{
    if (UUdSDKFunctionLibrary::IsLogin())
    {
        FJsonLoaderOptions Options;
        Options.Source = EJsonLoaderSource::E_URL;
        Options.From = "https://service.eulee.cn/UdsdkForUnrealEngine/Array.json";
        CJsonLoader::LoadJson(Options, [this](UJsonValueWrapper* JsonRoot, const FString& ErrorReason) {
            if (JsonRoot)
            {
                auto func_tmp = [](UJsonValueWrapper* Root, FString Pattern, UJsonValueWrapper*& Value) {
                    if (Root)
                    {
                        Root->FindField(Pattern, Value);
                    }
                };

                TArray<FUdAsset> assets;
                TArray<UJsonValueWrapper*> JsonValueArray;
                JsonRoot->GetArrayValue(JsonValueArray);
                for (auto* JsonItem : JsonValueArray)
                {
                    FUdAsset a;

                    UJsonValueWrapper* JsonAttribution = nullptr;
                    func_tmp(JsonItem, "attribution", JsonAttribution);
                    if (JsonAttribution)
                        JsonAttribution->GetStringValue(a.attribution);

                    UJsonValueWrapper* JsonDateAdded = nullptr;
                    func_tmp(JsonItem, "dateAdded", JsonDateAdded);
                    if (JsonDateAdded)
                        JsonDateAdded->GetStringValue(a.dateAdded);

                    UJsonValueWrapper* JsonDescription = nullptr;
                    func_tmp(JsonItem, "description", JsonDescription);
                    if (JsonDescription)
                        JsonDescription->GetStringValue(a.description);

                    UJsonValueWrapper* JsonId = nullptr;
                    func_tmp(JsonItem, "id", JsonId);
                    if (JsonId)
                        JsonId->GetInt64Value(a.id);

                    UJsonValueWrapper* JsonName = nullptr;
                    func_tmp(JsonItem, "name", JsonName);
                    if (JsonName)
                        JsonName->GetStringValue(a.name);

                    UJsonValueWrapper* JsonType = nullptr;
                    func_tmp(JsonItem, "type", JsonType);
                    if (JsonType)
                        JsonType->GetStringValue(a.type);

                    UJsonValueWrapper* JsonUrl = nullptr;
                    func_tmp(JsonItem, "url", JsonUrl);
                    if (JsonUrl)
                        JsonUrl->GetStringValue(a.url);

                    assets.Add(a);

                }
                this->PListView->ClearSelection();
                this->Assets.Empty();
                this->Assets.SetNum(assets.Num());
                for (size_t i = 0; i < assets.Num(); ++i)
                {
                    this->Assets[i] = MakeShared<FUdAsset>(assets[i]);
                }
                ApplyFilter();
                ApplySorting();
                this->PListView->RequestListRefresh();
            }
        });
    }
    else
    {
        this->PListView->ClearSelection();
        this->Assets.Empty();
        this->PListView->RequestListRefresh();
    }
    
}

void SUdSDKAssetsPanel::Tick(
    const FGeometry& AllottedGeometry,
    const double InCurrentTime,
    const float InDeltaTime)
{
  SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

void SUdSDKAssetsPanel::AssetSelected(
    TSharedPtr<FUdAsset> item,
    ESelectInfo::Type selectionType)
{
    this->PSelection = item;
}

void SUdSDKAssetsPanel::AddAsset(TSharedPtr<FUdAsset> item)
{
    if (isSupportedPointCloud(item))
    {
        this->AddAssetToLevel(item);
    }
}

void SUdSDKAssetsPanel::AddAssetToLevel(TSharedPtr<FUdAsset> item) 
{
    UWorld* pCurrentWorld = GEditor->GetEditorWorldContext().World();
    ULevel* pCurrentLevel = pCurrentWorld->GetCurrentLevel();


    //AActor* pNewActor = GEditor->AddActor(pCurrentLevel, AUdPointCloud::StaticClass(), FTransform(), false, RF_Public | RF_Transactional);
    //
    //AUdPointCloud* pPointCloud = Cast<AUdPointCloud>(pNewActor);
    //pPointCloud->SetUrl(item->url);
    //pPointCloud->RefreshPointCloud();
    //pPointCloud->RerunConstructionScripts();
}

namespace {


FString assetTypeToString(const FString& assetType) 
{
    static TMap<FString, FString> lookup = {
      {"UDS", "PointCloud"},
      {"POI", "PointOfInterest"},
      {"Polygon", "PolygonModel"}
    };
    auto it = lookup.Find(assetType);
    if (it != nullptr) {
        return *it;
    }
    return "(Unknown)";
}

FString formatDate(const FString& assetDate) 
{
    FString unrealDateString = assetDate;
    FDateTime dateTime;
    bool success = FDateTime::ParseIso8601(*unrealDateString, dateTime);
    if (!success) {
        return assetDate;
    }
    return dateTime.ToString(TEXT("%Y-%m-%d"));
}

class AssetsTableRow : public SMultiColumnTableRow<TSharedPtr<FUdAsset>> 
{
public:
  void Construct(
      const FArguments& InArgs,
      const TSharedRef<STableViewBase>& InOwnerTableView,
      const TSharedPtr<FUdAsset>& pItem)
  {
      this->_pItem = pItem;
      SMultiColumnTableRow<TSharedPtr<FUdAsset>>::Construct(
          InArgs,
          InOwnerTableView);
  }

  virtual TSharedRef<SWidget>
  GenerateWidgetForColumn(const FName& InColumnName) override
  {
      if (InColumnName == ColumnName_Name) {
          return SNew(STextBlock)
              .Text(FText::FromString(
                  *_pItem->name));
      }
      else if (InColumnName == ColumnName_Type) {
          return SNew(STextBlock)
              .Text(FText::FromString(
                  assetTypeToString(_pItem->type)));
      }
      else if (InColumnName == ColumnName_DateAdded) {
          return SNew(STextBlock)
              .Text(FText::FromString(formatDate(_pItem->dateAdded)));
      }
      else {
          return SNew(STextBlock);
      }
  }

private:
  TSharedPtr<FUdAsset> _pItem;
};

} // namespace

TSharedRef<ITableRow> SUdSDKAssetsPanel::CreateAssetRow(TSharedPtr<FUdAsset> item, const TSharedRef<STableViewBase>& list)
{
    return SNew(AssetsTableRow, list, item);
}
