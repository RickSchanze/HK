#include "StringView.h"
#include "String.h"

// FStringView 的 FString 构造函数实现
FStringView::FStringView(const FString& InStr) noexcept : View(InStr.CStr(), InStr.Size()) {}
