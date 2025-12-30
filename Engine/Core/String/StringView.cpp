#include "StringView.h"
#include "String.h"

// FStringView 的 FString 构造函数实现
FStringView::FStringView(const FString& InStr) noexcept : View(InStr.CStr(), InStr.Size()) {}

// 与 FString 的比较操作符实现
bool FStringView::operator==(const FString& Other) const noexcept
{
    return View == Other.GetStdString();
}

bool FStringView::operator!=(const FString& Other) const noexcept
{
    return View != Other.GetStdString();
}

bool FStringView::operator<(const FString& Other) const noexcept
{
    return View < Other.GetStdString();
}

bool FStringView::operator>(const FString& Other) const noexcept
{
    return View > Other.GetStdString();
}

bool FStringView::operator<=(const FString& Other) const noexcept
{
    return View <= Other.GetStdString();
}

bool FStringView::operator>=(const FString& Other) const noexcept
{
    return View >= Other.GetStdString();
}
