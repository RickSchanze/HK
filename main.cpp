#include "spdlog/spdlog.h"
#include <algorithm>
#include <iostream>
#include <ranges>

#ifdef HK_WINDOWS
#include <windows.h>
#endif

// Container includes
#include "Core/Container/Array.h"
#include "Core/Container/FixedArray.h"
#include "Core/Container/Map.h"
#include "Core/Container/OrderedMap.h"
#include "Core/Container/Span.h"
#include "Core/Container/Tuple.h"
#include "Core/Container/Variant.h"

// String includes
#include "Core/String/Name.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

int main()
{
#ifdef HK_WINDOWS
    // 设置控制台为UTF-8编码以支持中文显示
    SetConsoleOutputCP(65001); // UTF-8 code page
    SetConsoleCP(65001);       // UTF-8 code page
#endif

    std::cout << "=== HKEngine Container and String Test ===" << std::endl;
    spdlog::info("开始测试...");

    // Test TArray
    {
        spdlog::info("测试 TArray");
        TArray<int> Arr;
        Arr.Add(1);
        Arr.Add(2);
        Arr.Add(3);
        Arr.Add(4);
        Arr.Add(5);

        std::cout << "Array size: " << Arr.Size() << std::endl;
        std::cout << "Array[0]: " << Arr[0] << std::endl;

        auto Found = Arr.Find(3);
        if (Found != static_cast<size_t>(-1))
        {
            std::cout << "Found 3 at index: " << Found << std::endl;
        }

        auto Slice = Arr.Slice(1, 3);
        std::cout << "Slice size: " << Slice.Size() << std::endl;

        // Test C++20 ranges
        auto EvenNumbers = Arr | std::views::filter([](int n) { return n % 2 == 0; });
        std::cout << "Even numbers: ";
        for (auto n : EvenNumbers)
        {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }

    // Test TFixedArray
    {
        spdlog::info("测试 TFixedArray");
        TFixedArray<int, 5> FixedArr{1, 2, 3, 4, 5};
        std::cout << "FixedArray size: " << FixedArr.Size() << std::endl;
        std::cout << "FixedArray[2]: " << FixedArr[2] << std::endl;

        auto Found = FixedArr.Find(4);
        if (Found != static_cast<size_t>(-1))
        {
            std::cout << "Found 4 at index: " << Found << std::endl;
        }

        auto Slice = FixedArr.Slice(1, 3);
        std::cout << "Slice size: " << Slice.Size() << std::endl;
    }

    // Test TSpan with native array
    {
        spdlog::info("测试 TSpan");
        int NativeArray[] = {10, 20, 30, 40, 50};
        TSpan<int> Span(NativeArray);
        std::cout << "Span size: " << Span.Size() << std::endl;
        std::cout << "Span[1]: " << Span[1] << std::endl;

        // Test ranges with Span
        auto Doubled = Span | std::views::transform([](int n) { return n * 2; });
        std::cout << "Doubled values: ";
        for (auto n : Doubled)
        {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }

    // Test TMap
    {
        spdlog::info("测试 TMap");
        TMap<std::string, int> Map;
        Map.Add("one", 1);
        Map.Add("two", 2);
        Map.Add("three", 3);

        auto* Value = Map.Find("two");
        if (Value != nullptr)
        {
            std::cout << "Found 'two': " << *Value << std::endl;
        }

        auto It = Map.FindIterator("three");
        if (It != Map.end())
        {
            std::cout << "Found iterator: " << It->first << " = " << It->second << std::endl;
        }

        std::cout << "Map size: " << Map.Size() << std::endl;
    }

    // Test TOrderedMap
    {
        spdlog::info("测试 TOrderedMap");
        TOrderedMap<int, std::string> OrderedMap;
        OrderedMap.Add(3, "three");
        OrderedMap.Add(1, "one");
        OrderedMap.Add(2, "two");

        std::cout << "OrderedMap contents (ordered): ";
        for (const auto& Pair : OrderedMap)
        {
            std::cout << Pair.first << ":" << Pair.second << " ";
        }
        std::cout << std::endl;
    }

    // Test TTuple
    {
        spdlog::info("测试 TTuple");
        TTuple<int, std::string, double> Tuple(42, "hello", 3.14);
        std::cout << "Tuple[0]: " << Tuple.Get<0>() << std::endl;
        std::cout << "Tuple[1]: " << Tuple.Get<1>() << std::endl;
        std::cout << "Tuple[2]: " << Tuple.Get<2>() << std::endl;

        std::cout << "Tuple Get<int>: " << Tuple.Get<int>() << std::endl;
        std::cout << "Tuple Get<std::string>: " << Tuple.Get<std::string>() << std::endl;
    }

    // Test TVariant
    {
        spdlog::info("测试 TVariant");
        TVariant<int, std::string, double> Variant;
        Variant = 42;

        auto* IntPtr = Variant.Get<int>();
        if (IntPtr != nullptr)
        {
            std::cout << "Variant holds int: " << *IntPtr << std::endl;
        }

        Variant = std::string("hello");
        auto* StrPtr = Variant.Get<std::string>();
        if (StrPtr != nullptr)
        {
            std::cout << "Variant holds string: " << *StrPtr << std::endl;
        }

        std::cout << "Variant index: " << Variant.Index() << std::endl;
    }

    // Test FStringView
    {
        spdlog::info("测试 FStringView");
        FStringView View("Hello, World!");
        std::cout << "StringView: " << std::string(View.Data(), View.Size()) << std::endl;
        std::cout << "StringView size: " << View.Size() << std::endl;
        std::cout << "Contains 'World': " << View.Contains("World") << std::endl;
        std::cout << "Starts with 'Hello': " << View.StartsWith("Hello") << std::endl;

        auto SubView = View.SubStr(7, 5);
        std::cout << "SubStr(7, 5): " << std::string(SubView.Data(), SubView.Size()) << std::endl;
    }

    // Test FString
    {
        spdlog::info("测试 FString");
        FString Str("  Hello, World!  ");
        std::cout << "Original: '" << Str.GetStdString() << "'" << std::endl;

        auto Trimmed = Str.Trim();
        std::cout << "Trimmed (returns StringView): '" << std::string(Trimmed.Data(), Trimmed.Size()) << "'"
                  << std::endl;

        Str.TrimInPlace();
        std::cout << "Trimmed in place: '" << Str.GetStdString() << "'" << std::endl;

        auto Replaced = Str.Replace("World", "Universe");
        std::cout << "Replaced: '" << Replaced.GetStdString() << "'" << std::endl;

        auto Found = Str.Find("Hello");
        if (Found != static_cast<size_t>(-1))
        {
            std::cout << "Found 'Hello' at: " << Found << std::endl;
        }

        FStringView View("View");
        FString StrFromView(View);
        std::cout << "String from StringView: '" << StrFromView.GetStdString() << "'" << std::endl;
    }

    // Test FName
    {
        spdlog::info("测试 FName");
        FName Name1("TestName");
        FName Name2("TestName");
        FName Name3("AnotherName");

        std::cout << "Name1 ID: " << Name1.GetID() << std::endl;
        std::cout << "Name1 String: " << Name1.GetString() << std::endl;
        std::cout << "Name2 ID: " << Name2.GetID() << std::endl;
        std::cout << "Name3 ID: " << Name3.GetID() << std::endl;

        std::cout << "Name1 == Name2: " << (Name1 == Name2) << std::endl;
        std::cout << "Name1 == Name3: " << (Name1 == Name3) << std::endl;

        std::cout << "Name table size: " << FName::GetNameTableSize() << std::endl;
    }

    // Test C++20 algorithms with containers
    {
        spdlog::info("测试 C++20 算法兼容性");
        TArray<int> Numbers{5, 2, 8, 1, 9, 3};

        // Use std::ranges::sort
        std::ranges::sort(Numbers);
        std::cout << "Sorted numbers: ";
        for (auto n : Numbers)
        {
            std::cout << n << " ";
        }
        std::cout << std::endl;

        // Use std::ranges::count_if
        auto Count = std::ranges::count_if(Numbers, [](int n) { return n > 5; });
        std::cout << "Count of numbers > 5: " << Count << std::endl;

        // Use std::ranges::transform with vector
        std::vector<int> DoubledVec;
        std::ranges::transform(Numbers, std::back_inserter(DoubledVec), [](int n) { return n * 2; });
        TArray<int> Doubled(DoubledVec.begin(), DoubledVec.end());
        std::cout << "Doubled numbers: ";
        for (auto n : Doubled)
        {
            std::cout << n << " ";
        }
        std::cout << std::endl;
    }

    spdlog::info("所有测试完成！");
    return 0;
}