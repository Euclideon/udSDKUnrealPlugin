#pragma once

#include "UdSDKDefine.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/SHeaderRow.h"

class FArguments;
class ITableRow;
class STableViewBase;

template <typename ItemType> class SListView;

class SUdSDKAssetsPanel : public SCompoundWidget {
  SLATE_BEGIN_ARGS(SUdSDKAssetsPanel) {}
  SLATE_END_ARGS()
};
