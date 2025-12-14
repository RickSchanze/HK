#include "spdlog/spdlog.h"
#include <algorithm>
#include <iostream>
#include <ranges>
#include <thread>

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

// Event includes
#include "Core/Event/Delegate.h"
#include "Core/Event/Event.h"

// Utility includes
#include "Core/Utility/Expected.h"
#include "Core/Utility/Optional.h"
#include "Core/Utility/Ref.h"
#include "Core/Utility/SharedPtr.h"
#include "Core/Utility/UniquePtr.h"

// Logging includes
#include "Core/Logging/Logger.h"

// Time includes
#include "Core/Time/Time.h"

// Test function for Event
void StaticHandler(int x)
{
    std::cout << "Static handler: " << x << std::endl;
}

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
        std::cout << "Name1 String: " << Name1.GetStdString() << std::endl;
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

    // Test TDelegate
    {
        spdlog::info("测试 TDelegate");

        // Test Delegate<void> - 无参数无返回值
        {
            TDelegate<void> VoidDelegate;
            int CallCount = 0;
            VoidDelegate.Bind([&CallCount]() { CallCount++; });
            VoidDelegate.Invoke();
            std::cout << "Delegate<void> call count: " << CallCount << std::endl;
            VoidDelegate.Clear();
        }

        // Test Delegate<int, int> - 有返回值有参数
        {
            TDelegate<int, int> IntDelegate;
            IntDelegate.Bind([](int x) { return x * 2; });
            int Result = IntDelegate.Invoke(5);
            std::cout << "Delegate<int, int> result: " << Result << std::endl;
        }

        // Test Bind with function pointer
        {
            auto StaticFunc = [](int x) -> int { return x + 10; };
            TDelegate<int, int> FuncPtrDelegate;
            FuncPtrDelegate.Bind(StaticFunc);
            std::cout << "Delegate with function pointer: " << FuncPtrDelegate.Invoke(20) << std::endl;
        }

        // Test Bind with member function
        {
            struct TestClass
            {
                int Value = 100;
                int Multiply(int x)
                {
                    return Value * x;
                }
                int Add(int x) const
                {
                    return Value + x;
                }
            };
            TestClass Obj;
            TDelegate<int, int> MemberDelegate;
            MemberDelegate.Bind(&Obj, &TestClass::Multiply);
            std::cout << "Delegate with member function: " << MemberDelegate.Invoke(3) << std::endl;

            TDelegate<int, int> ConstMemberDelegate;
            ConstMemberDelegate.Bind(&Obj, &TestClass::Add);
            std::cout << "Delegate with const member function: " << ConstMemberDelegate.Invoke(50) << std::endl;
        }

        // Test operator()
        {
            TDelegate<int, int> OpDelegate;
            OpDelegate.Bind([](int x) { return x * x; });
            std::cout << "Delegate operator(): " << OpDelegate(7) << std::endl;
        }
    }

    // Test TEvent
    {
        spdlog::info("测试 TEvent");

        // Test Event<void> - 无参数
        {
            TEvent<> VoidEvent;
            int CallCount1 = 0, CallCount2 = 0;
            auto Handle1 = VoidEvent.AddBind([&CallCount1]() { CallCount1++; });
            auto Handle2 = VoidEvent.AddBind([&CallCount2]() { CallCount2 += 2; });
            (void)Handle2; // 用于测试，实际使用中会保存
            VoidEvent.Invoke();
            std::cout << "Event<void> call count1: " << CallCount1 << ", count2: " << CallCount2 << std::endl;

            VoidEvent.RemoveBind(Handle1);
            VoidEvent.Invoke();
            std::cout << "After remove handle1 - count1: " << CallCount1 << ", count2: " << CallCount2 << std::endl;

            VoidEvent.Clear();
            std::cout << "Event bind count after clear: " << VoidEvent.GetBindCount() << std::endl;
        }

        // Test Event<int, int> - 有参数
        {
            TEvent<int, int> IntEvent;
            int Sum = 0;
            auto Handle1 = IntEvent.AddBind([&Sum](int a, int b) { Sum += a + b; });
            auto Handle2 = IntEvent.AddBind([&Sum](int a, int b) { Sum += a * b; });
            (void)Handle1; // 用于测试，实际使用中会保存
            (void)Handle2; // 用于测试，实际使用中会保存
            IntEvent.Invoke(3, 4);
            std::cout << "Event<int, int> sum: " << Sum << std::endl;
        }

        // Test AddBind with function pointer
        {
            TEvent<int> FuncEvent;
            auto Handle = FuncEvent.AddBind(StaticHandler);
            (void)Handle; // 用于测试，实际使用中会保存
            FuncEvent.Invoke(42);
        }

        // Test AddBind with member function
        {
            struct EventHandler
            {
                int Counter = 0;
                void Handle(int x)
                {
                    Counter += x;
                }
                void HandleConst(int x) const
                {
                    std::cout << "Const handler: " << x << std::endl;
                }
            };
            EventHandler Handler;
            TEvent<int> MemberEvent;
            auto Handle1 = MemberEvent.AddBind(&Handler, &EventHandler::Handle);
            auto Handle2 = MemberEvent.AddBind(&Handler, &EventHandler::HandleConst);
            (void)Handle1; // 用于测试，实际使用中会保存
            (void)Handle2; // 用于测试，实际使用中会保存
            MemberEvent.Invoke(10);
            std::cout << "Event with member function, counter: " << Handler.Counter << std::endl;
        }

        // Test operator()
        {
            TEvent<int> OpEvent;
            int Value = 0;
            OpEvent.AddBind([&Value](int x) { Value = x; });
            OpEvent(999);
            std::cout << "Event operator() value: " << Value << std::endl;
        }
    }

    // Test GetHashCode with TMap
    {
        spdlog::info("测试 GetHashCode 与 TMap");

        // Test FName as Map key (uses GetHashCode)
        {
            TMap<FName, int> NameMap;
            FName Key1("TestKey1");
            FName Key2("TestKey2");
            FName Key3("TestKey1"); // Same as Key1

            NameMap.Add(Key1, 100);
            NameMap.Add(Key2, 200);
            NameMap.Add(Key3, 300); // Should overwrite Key1

            std::cout << "NameMap size: " << NameMap.Size() << std::endl;
            auto* Value1 = NameMap.Find(Key1);
            if (Value1 != nullptr)
            {
                std::cout << "Key1 value: " << *Value1 << std::endl;
            }
            auto* Value2 = NameMap.Find(Key2);
            if (Value2 != nullptr)
            {
                std::cout << "Key2 value: " << *Value2 << std::endl;
            }

            // Test GetHashCode directly
            std::cout << "Key1 GetHashCode: " << Key1.GetHashCode() << std::endl;
            std::cout << "Key2 GetHashCode: " << Key2.GetHashCode() << std::endl;
            std::cout << "Key3 GetHashCode: " << Key3.GetHashCode() << std::endl;
            std::cout << "Key1 == Key3: " << (Key1 == Key3) << std::endl;
        }

        // Test custom type with GetHashCode
        {
            struct CustomKey
            {
                int ID;
                std::string Name;

                bool operator==(const CustomKey& Other) const
                {
                    return ID == Other.ID && Name == Other.Name;
                }

                size_t GetHashCode() const noexcept
                {
                    return std::hash<int>{}(ID) ^ (std::hash<std::string>{}(Name) << 1);
                }
            };

            TMap<CustomKey, std::string> CustomMap;
            CustomKey Key1{1, "First"};
            CustomKey Key2{2, "Second"};
            CustomKey Key3{1, "First"}; // Same as Key1

            CustomMap.Add(Key1, "Value1");
            CustomMap.Add(Key2, "Value2");
            CustomMap.Add(Key3, "Value3"); // Should overwrite Key1

            std::cout << "CustomMap size: " << CustomMap.Size() << std::endl;
            auto* Value1 = CustomMap.Find(Key1);
            if (Value1 != nullptr)
            {
                std::cout << "CustomKey1 value: " << *Value1 << std::endl;
            }
            std::cout << "CustomKey1 GetHashCode: " << Key1.GetHashCode() << std::endl;
            std::cout << "CustomKey2 GetHashCode: " << Key2.GetHashCode() << std::endl;
        }
    }

    // Test Utility Classes
    {
        spdlog::info("测试 Utility 工具类");

        // Test TRef - 引用包装和多态
        {
            spdlog::info("测试 TRef");
            struct Base
            {
                virtual ~Base() = default;
                virtual int GetValue() const
                {
                    return 1;
                }
            };
            struct Derived : public Base
            {
                int GetValue() const override
                {
                    return 2;
                }
            };

            Derived DerivedObj;
            Base& BaseRef = DerivedObj;
            TRef<Base> Ref = MakeRef(BaseRef);

            std::cout << "TRef<Base> GetValue (多态): " << Ref.Get().GetValue() << std::endl;
            std::cout << "TRef operator*: " << (*Ref).GetValue() << std::endl;
            std::cout << "TRef operator->: " << Ref->GetValue() << std::endl;

            // 测试 const 引用
            const Base& ConstBaseRef = DerivedObj;
            TRef<const Base> ConstRef = MakeRef(ConstBaseRef);
            std::cout << "TRef<const Base> GetValue: " << ConstRef.Get().GetValue() << std::endl;
        }

        // Test TSharedPtr - 共享指针和多态
        {
            spdlog::info("测试 TSharedPtr");
            struct Base
            {
                virtual ~Base() = default;
                virtual std::string GetName() const
                {
                    return "Base";
                }
            };
            struct Derived : public Base
            {
                std::string GetName() const override
                {
                    return "Derived";
                }
            };

            // 测试 MakeShared
            TSharedPtr<Derived> DerivedPtr = MakeShared<Derived>();
            std::cout << "TSharedPtr<Derived> GetName: " << DerivedPtr->GetName() << std::endl;

            // 测试多态转换
            TSharedPtr<Base> BasePtr = DerivedPtr;
            std::cout << "TSharedPtr<Base> GetName (多态): " << BasePtr->GetName() << std::endl;
            std::cout << "Use count: " << BasePtr.UseCount() << std::endl;

            // 测试拷贝
            TSharedPtr<Base> BasePtr2 = BasePtr;
            std::cout << "After copy, use count: " << BasePtr.UseCount() << std::endl;

            // 测试重置
            BasePtr2.Reset();
            std::cout << "After reset, use count: " << BasePtr.UseCount() << std::endl;

            // 测试空指针
            TSharedPtr<Base> NullPtr;
            std::cout << "Null pointer is valid: " << (bool)NullPtr << std::endl;
        }

        // Test TUniquePtr - 唯一指针和多态
        {
            spdlog::info("测试 TUniquePtr");
            struct Base
            {
                virtual ~Base() = default;
                virtual int GetValue() const
                {
                    return 10;
                }
            };
            struct Derived : public Base
            {
                int GetValue() const override
                {
                    return 20;
                }
            };

            // 测试 MakeUnique
            TUniquePtr<Derived> DerivedPtr = MakeUnique<Derived>();
            std::cout << "TUniquePtr<Derived> GetValue: " << DerivedPtr->GetValue() << std::endl;

            // 测试多态转换（移动）
            TUniquePtr<Base> BasePtr = std::move(DerivedPtr);
            std::cout << "TUniquePtr<Base> GetValue (多态): " << BasePtr->GetValue() << std::endl;
            std::cout << "DerivedPtr after move is valid: " << (bool)DerivedPtr << std::endl;

            // 测试释放
            Base* ReleasedPtr = BasePtr.Release();
            std::cout << "Released raw pointer GetValue: " << ReleasedPtr->GetValue() << std::endl;
            delete ReleasedPtr;

            // 测试自定义删除器
            struct CustomDeleter
            {
                void operator()(Base* Ptr)
                {
                    std::cout << "Custom deleter called" << std::endl;
                    delete Ptr;
                }
            };
            TUniquePtr<Base> TempPtr = MakeUnique<Base>();
            Base* CustomRawPtr = TempPtr.Release();
            TUniquePtr<Base, CustomDeleter> CustomPtr(CustomRawPtr, CustomDeleter{});
        }

        // Test TOptional
        {
            spdlog::info("测试 TOptional");
            TOptional<int> OptInt;
            std::cout << "Empty optional IsSet: " << OptInt.IsSet() << std::endl;

            OptInt = 42;
            std::cout << "Optional with value: " << OptInt.GetValue() << std::endl;
            std::cout << "Optional operator*: " << *OptInt << std::endl;
            std::cout << "Optional operator->: " << OptInt.operator->() << std::endl;

            // 测试 MakeOptional
            auto OptStr = MakeOptional(std::string("Hello"));
            std::cout << "MakeOptional string: " << OptStr.GetValue() << std::endl;

            // 测试 ValueOr
            TOptional<int> EmptyOpt;
            std::cout << "Empty optional ValueOr(100): " << EmptyOpt.ValueOr(100) << std::endl;

            // 测试 Emplace
            TOptional<std::string> EmplaceOpt;
            EmplaceOpt.Emplace("Emplaced");
            std::cout << "Emplaced value: " << EmplaceOpt.GetValue() << std::endl;
        }

        // Test TExpected
        {
            spdlog::info("测试 TExpected");
            enum class ErrorCode
            {
                None,
                InvalidInput,
                OutOfRange
            };

            // 测试成功值
            TExpected<int, ErrorCode> Success = MakeExpected<int, ErrorCode>(42);
            std::cout << "Expected success HasValue: " << Success.HasValue() << std::endl;
            std::cout << "Expected success value: " << Success.Value() << std::endl;
            std::cout << "Expected success operator*: " << *Success << std::endl;

            // 测试错误值
            TExpected<int, ErrorCode> Error = MakeExpectedError<int, ErrorCode>(ErrorCode::InvalidInput);
            std::cout << "Expected error HasError: " << Error.HasError() << std::endl;
            std::cout << "Expected error IsOk: " << Error.IsOk() << std::endl;

            // 测试 ValueOr
            std::cout << "Error ValueOr(100): " << Error.ValueOr(100) << std::endl;

            // 测试 ValueOrElse
            auto Result = Error.ValueOrElse(
                [](ErrorCode Code) -> int
                {
                    std::cout << "Error code: " << (int)Code << std::endl;
                    return 200;
                });
            std::cout << "ValueOrElse result: " << Result << std::endl;

            // 测试字符串错误类型
            TExpected<std::string, std::string> StringError =
                MakeExpectedError<std::string, std::string>("Error message");
            std::cout << "String error: " << StringError.Error() << std::endl;
        }

        // Test TRef with Event and Delegate (测试引用必须用 Ref 包裹)
        {
            spdlog::info("测试 TRef 与 Event/Delegate 的集成");
            // 测试 Delegate 接受 const 引用
            TDelegate<void, const int&> ConstRefDelegate;
            ConstRefDelegate.Bind([](const int& Value)
                                  { std::cout << "Delegate const ref value: " << Value << std::endl; });
            int Value = 100;
            ConstRefDelegate.Invoke(Value);

            // 测试 Event 接受 TRef
            TEvent<TRef<int>> RefEvent;
            RefEvent.AddBind(
                [](TRef<int> Ref)
                {
                    Ref.Get() = 999;
                    std::cout << "Event modified ref value: " << Ref.Get() << std::endl;
                });
            int ModifiableValue = 0;
            RefEvent.Invoke(MakeRef(ModifiableValue));
            std::cout << "Original value after event: " << ModifiableValue << std::endl;

            // 测试多态与 TRef
            struct Base
            {
                virtual ~Base() = default;
                virtual void DoSomething() const
                {
                    std::cout << "Base::DoSomething" << std::endl;
                }
            };
            struct Derived : public Base
            {
                void DoSomething() const override
                {
                    std::cout << "Derived::DoSomething" << std::endl;
                }
            };

            Derived DerivedObj;
            const Base& ConstBaseRef = DerivedObj;
            TEvent<TRef<const Base>> PolyEvent;
            PolyEvent.AddBind([](TRef<const Base> Ref) { Ref->DoSomething(); });
            TRef<const Base> BaseRefWrapper = MakeRef(ConstBaseRef);
            PolyEvent.Invoke(BaseRefWrapper);
        }

        // Test 多态组合场景
        {
            spdlog::info("测试多态组合场景");
            struct Animal
            {
                virtual ~Animal() = default;
                virtual std::string MakeSound() const = 0;
            };
            struct Dog : public Animal
            {
                std::string MakeSound() const override
                {
                    return "Woof!";
                }
            };
            struct Cat : public Animal
            {
                std::string MakeSound() const override
                {
                    return "Meow!";
                }
            };

            // 使用 TSharedPtr 存储多态对象
            TSharedPtr<Animal> DogPtr = MakeShared<Dog>();
            TSharedPtr<Animal> CatPtr = MakeShared<Cat>();

            std::cout << "Dog sound: " << DogPtr->MakeSound() << std::endl;
            std::cout << "Cat sound: " << CatPtr->MakeSound() << std::endl;

            // 使用 TUniquePtr 存储多态对象（通过 Release 和 Reset 转换）
            TUniquePtr<Animal> UniqueDogRaw = MakeUnique<Cat>();
            TUniquePtr<Animal> UniqueDog;
            UniqueDog.Reset(UniqueDogRaw.Release());

            TUniquePtr<Cat> UniqueCatRaw = MakeUnique<Cat>();
            TUniquePtr<Animal> UniqueCat;
            UniqueCat.Reset(UniqueCatRaw.Release());

            std::cout << "Unique Dog sound: " << UniqueDog->MakeSound() << std::endl;
            std::cout << "Unique Cat sound: " << UniqueCat->MakeSound() << std::endl;

            // 使用 TOptional 存储多态指针
            TSharedPtr<Animal> AnimalPtr = MakeShared<Dog>();
            TOptional<TSharedPtr<Animal>> OptionalAnimal = MakeOptional(AnimalPtr);
            if (OptionalAnimal.IsSet())
            {
                std::cout << "Optional Animal sound: " << (*OptionalAnimal)->MakeSound() << std::endl;
            }
        }
    }

    spdlog::info("所有测试完成！");

    // Test Logger
    {
        std::cout << "\n=== Logger Test ===" << std::endl;

        // 测试各种日志级别
        HK_LOG_DEBUG(ELogcat::Engine, "Debug message: {}", 42);
        HK_LOG_INFO(ELogcat::Engine, "Info message: {}", "Hello");
        HK_LOG_WARN(ELogcat::Engine, "Warning message: {}", 3.14);
        HK_LOG_ERROR(ELogcat::Engine, "Error message: {}", true);
        HK_LOG_FATAL(ELogcat::Engine, "Fatal message: {}", 100);

        // 测试FString, FName, FStringView格式化
        FString TestStr("TestString");
        FName TestName("TestName");
        FStringView TestView("TestView");

        HK_LOG_INFO(ELogcat::Engine, "FString: {}, FName: {}, FStringView: {}", TestStr, TestName, TestView);

        // 测试OnLog事件
        int LogCount = 0;
        auto Handle = GLogger.OnLog.AddBind(
            [&LogCount](const FLogContent& Content)
            {
                LogCount++;
                std::cout << "OnLog event triggered: " << Content.Message.GetStdString() << std::endl;
            });

        HK_LOG_INFO(ELogcat::Engine, "This should trigger OnLog event");
        std::cout << "Log count: " << LogCount << std::endl;

        GLogger.OnLog.RemoveBind(Handle);
    }

    // Test Time
    {
        std::cout << "\n=== Time Test ===" << std::endl;

        // 测试Now()
        FTimePoint Now1 = FTimePoint::Now();
        std::cout << "Current time (seconds): " << Now1.ToSeconds() << std::endl;
        std::cout << "Current time (milliseconds): " << Now1.ToMilliseconds() << std::endl;
        std::cout << "Current time formatted: " << Now1.Format() << std::endl;
        std::cout << "Current time ISO8601: " << Now1.FormatISO8601() << std::endl;

        // 测试时间差
        FTimePoint Start = FTimePoint::Now();
        // 模拟一些工作
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        FTimePoint End = FTimePoint::Now();

        FTimeDuration Duration = End - Start;
        std::cout << "Duration (milliseconds): " << Duration.AsMilliseconds() << std::endl;
        std::cout << "Duration AsDouble<FSeconds>: " << Duration.AsDouble<FSeconds>() << std::endl;
        std::cout << "Duration AsFloat<FMilliseconds>: " << Duration.AsFloat<FMilliseconds>() << std::endl;
        std::cout << "Duration AsDouble<FMinutes>: " << Duration.AsDouble<FMinutes>() << std::endl;
        std::cout << "Duration As<float, FSeconds>: " << Duration.As<float, FSeconds>() << std::endl;
        std::cout << "Duration As<double, FMilliseconds>: " << Duration.As<double, FMilliseconds>() << std::endl;

        // 测试GetStdTimePoint
        auto StdTimePoint = Now1.GetStdTimePoint();
        std::cout << "GetStdTimePoint works: " << (StdTimePoint.time_since_epoch().count() > 0) << std::endl;

        // 测试GetStdDuration
        auto StdDuration = Duration.GetStdDuration();
        std::cout << "GetStdDuration works: " << (StdDuration.count() > 0) << std::endl;
    }

    // Test Assert (only in Debug)
    {
        std::cout << "\n=== Assert Test ===" << std::endl;

        // 这些在Release模式下不会执行
        bool Condition = true;
        HK_ASSERT(Condition);
        HK_ASSERT_MSG(Condition, "This should not fail: {}", 42);

        std::cout << "Assert tests passed (or skipped in Release mode)" << std::endl;
    }

    std::cout << "\n=== All Tests Complete ===" << std::endl;
    return 0;
}