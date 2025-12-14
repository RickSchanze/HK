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

// Reflection includes
#include "Core/Reflection/Any.h"
#include "Core/Reflection/AnyRef.h"
#include "Core/Reflection/TypeManager.h"

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

    // Test Reflection System
    {
        std::cout << "\n=== Reflection System Test ===" << std::endl;

        // 初始化基础类型
        FTypeManager::Get().InitializeAllTypes();

        // 测试TypeOf和基础类型注册
        {
            spdlog::info("测试 TypeOf 和基础类型");
            FType Int32Type = TypeOf<Int32>();
            if (Int32Type != nullptr)
            {
                std::cout << "Int32 type name: " << Int32Type->Name.GetStdString() << std::endl;
                std::cout << "Int32 type size: " << Int32Type->Size << std::endl;
                std::cout << "Int32 is enum: " << Int32Type->IsEnum() << std::endl;
            }

            FType FloatType = TypeOf<Float>();
            if (FloatType != nullptr)
            {
                std::cout << "Float type name: " << FloatType->Name.GetStdString() << std::endl;
            }
        }

        // 测试Enum类型
        {
            spdlog::info("测试 Enum 类型");
            enum class ETestEnum : Int32
            {
                Value1 = 1,
                Value2 = 2,
                Value3 = 3
            };

            FTypeMutable EnumType = FTypeManager::Register<ETestEnum>("ETestEnum");
            if (EnumType != nullptr)
            {
                std::cout << "ETestEnum type name: " << EnumType->Name.GetStdString() << std::endl;
                std::cout << "ETestEnum is enum: " << EnumType->IsEnum() << std::endl;
                if (EnumType->IsEnum())
                {
                    FType UnderlyingType = EnumType->GetUnderlyingType();
                    if (UnderlyingType != nullptr)
                    {
                        std::cout << "ETestEnum underlying type: " << UnderlyingType->Name.GetStdString() << std::endl;
                    }
                }

                // 注册所有枚举成员
                EnumType->RegisterEnumMember(ETestEnum::Value1, "Value1")
                    ->RegisterEnumMember(ETestEnum::Value2, "Value2")
                    ->RegisterEnumMember(ETestEnum::Value3, "Value3");

                std::cout << "ETestEnum has " << EnumType->Properties.Size() << " enum members" << std::endl;
                for (const auto& Prop : EnumType->Properties)
                {
                    if (Prop != nullptr && Prop->IsEnum() && Prop->Name.IsValid())
                    {
                        std::cout << "  Enum member: " << Prop->Name.GetStdString()
                                  << ", Value: " << Prop->GetEnumValue() << std::endl;
                        auto EnumValue = Prop->GetEnumPropertyValue<ETestEnum>();
                        if (EnumValue.IsSet())
                        {
                            std::cout << "    Typed value: " << static_cast<Int32>(EnumValue.GetValue()) << std::endl;
                        }
                    }
                }
            }
        }

        // 测试类注册和属性
        {
            spdlog::info("测试类注册和属性");
            struct A
            {
                Int32 B;
            };

            FTypeMutable AType = FTypeManager::Register<A>("A");
            if (AType != nullptr)
            {
                std::cout << "Registered type A, size: " << AType->Size << std::endl;
                AType->RegisterProperty(&A::B, "B");
                // 获取刚注册的属性
                if (AType->Properties.Size() > 0)
                {
                    FProperty AProperty = AType->Properties[0];
                    if (AProperty != nullptr)
                    {
                        std::cout << "Registered property B, offset: " << AProperty->Offset << std::endl;
                        std::cout << "Property B is enum: " << AProperty->IsEnum() << std::endl;
                        std::cout << "Property B owner type: " << AProperty->OwnerType->Name.GetStdString()
                                  << std::endl;
                    }
                }
            }
            if (AType != nullptr)
            {
                std::cout << "Type A has " << AType->Properties.Size() << " properties" << std::endl;
                for (const auto& Prop : AType->Properties)
                {
                    if (Prop != nullptr && Prop->Name.IsValid())
                    {
                        std::cout << "  Property: " << Prop->Name.GetStdString();
                        if (Prop->Type != nullptr && Prop->Type->Name.IsValid())
                        {
                            std::cout << ", Type: " << Prop->Type->Name.GetStdString();
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }

        // 测试Cast函数
        {
            spdlog::info("测试 Cast 函数");
            Int32 Int32Value = 42;
            auto Int64Result = Cast<Int64>(Int32Value);
            if (Int64Result.IsSet())
            {
                std::cout << "Cast Int32->Int64: " << Int32Value << " -> " << Int64Result.GetValue() << std::endl;
            }

            UInt32 UInt32Value = 100;
            auto UInt64Result = Cast<UInt64>(UInt32Value);
            if (UInt64Result.IsSet())
            {
                std::cout << "Cast UInt32->UInt64: " << UInt32Value << " -> " << UInt64Result.GetValue() << std::endl;
            }

            auto Int64FromUInt32 = Cast<Int64>(UInt32Value);
            if (Int64FromUInt32.IsSet())
            {
                std::cout << "Cast UInt32->Int64: " << UInt32Value << " -> " << Int64FromUInt32.GetValue() << std::endl;
            }

            // 测试失败的转换
            auto Int32FromUInt32 = Cast<Int32>(UInt32Value);
            if (!Int32FromUInt32.IsSet())
            {
                std::cout << "Cast UInt32->Int32 failed (as expected)" << std::endl;
            }

            // 测试Bool转换
            Bool BoolValue = true;
            auto IntFromBool = Cast<Int32>(BoolValue);
            if (IntFromBool.IsSet())
            {
                std::cout << "Cast Bool->Int32: " << BoolValue << " -> " << IntFromBool.GetValue() << std::endl;
            }

            // 测试CastFast
            Int64 Int64Value = 12345;
            Int32 FastResult = CastFast<Int32>(Int64Value);
            std::cout << "CastFast Int64->Int32: " << FastResult << std::endl;
        }

        // 测试FAny
        {
            spdlog::info("测试 FAny");
            Int32 IntValue = 123;
            FAny AnyValue(IntValue);
            if (AnyValue.IsValid())
            {
                std::cout << "FAny is valid, type: " << AnyValue.GetType()->Name.GetStdString() << std::endl;
                Int32* RetrievedValue = AnyValue.Get<Int32>();
                if (RetrievedValue != nullptr)
                {
                    std::cout << "FAny value: " << *RetrievedValue << std::endl;
                }
            }
        }

        // 测试FAnyRef
        {
            spdlog::info("测试 FAnyRef");
            Int32 IntValue = 456;
            FAnyRef AnyRefValue(IntValue);
            if (AnyRefValue.IsValid())
            {
                std::cout << "FAnyRef is valid, type: " << AnyRefValue.GetType()->Name.GetStdString() << std::endl;
                Int32* RetrievedValue = AnyRefValue.Get<Int32>();
                if (RetrievedValue != nullptr)
                {
                    std::cout << "FAnyRef value: " << *RetrievedValue << std::endl;
                    *RetrievedValue = 789;
                    std::cout << "Modified through FAnyRef, original value: " << IntValue << std::endl;
                }
            }

            FAnyRef EmptyRef;
            std::cout << "Empty FAnyRef is valid: " << EmptyRef.IsValid() << ", is empty: " << EmptyRef.IsEmpty()
                      << std::endl;
        }

        // 测试GetValueRef和TryGetValue
        {
            spdlog::info("测试 GetValueRef 和 TryGetValue");
            Int32 IntValue = 999;
            FAnyRef AnyRef(IntValue);

            auto RetrievedRef = FTypeManager::GetValueRef<Int32>(AnyRef);
            if (RetrievedRef.IsValid())
            {
                Int32* ValuePtr = RetrievedRef.Get<Int32>();
                if (ValuePtr != nullptr)
                {
                    std::cout << "GetValueRef result: " << *ValuePtr << std::endl;
                }
            }

            Int32 OutValue = 0;
            bool Success = FTypeManager::TryGetValue<Int32>(IntValue, OutValue);
            if (Success)
            {
                std::cout << "TryGetValue success: " << OutValue << std::endl;
            }

            // 测试失败的转换
            Float FloatValue = 3.14f;
            Int32 IntOut = 0;
            bool Failed = FTypeManager::TryGetValue<Int32>(FloatValue, IntOut);
            if (!Failed)
            {
                std::cout << "TryGetValue Float->Int32 failed (as expected)" << std::endl;
            }
        }

        // 测试枚举成员注册
        {
            spdlog::info("测试枚举成员注册");
            enum class ETestEnum2 : Int32
            {
                None = 0,
                Option1 = 10,
                Option2 = 20,
                Option3 = 30
            };

            FTypeMutable EnumType2 = FTypeManager::Register<ETestEnum2>("ETestEnum2");
            if (EnumType2 != nullptr)
            {
                std::cout << "Registered ETestEnum2" << std::endl;

                // 注册所有枚举成员
                EnumType2->RegisterEnumMember(ETestEnum2::None, "None")
                    ->RegisterEnumMember(ETestEnum2::Option1, "Option1")
                    ->RegisterEnumMember(ETestEnum2::Option2, "Option2")
                    ->RegisterEnumMember(ETestEnum2::Option3, "Option3");

                std::cout << "ETestEnum2 has " << EnumType2->Properties.Size() << " enum members" << std::endl;
                for (const auto& Prop : EnumType2->Properties)
                {
                    if (Prop != nullptr && Prop->IsEnum() && Prop->Name.IsValid())
                    {
                        std::cout << "  Enum member: " << Prop->Name.GetStdString()
                                  << ", Value: " << Prop->GetEnumValue() << std::endl;
                        auto EnumValue = Prop->GetEnumPropertyValue<ETestEnum2>();
                        if (EnumValue.IsSet())
                        {
                            std::cout << "    Typed value: " << static_cast<Int32>(EnumValue.GetValue()) << std::endl;
                        }
                    }
                }
            }
        }

        // 测试多个属性注册
        {
            spdlog::info("测试多个属性注册");
            struct TestClass
            {
                Int32 IntField;
                Float FloatField;
                Bool BoolField;
            };

            FTypeMutable TestType = FTypeManager::Register<TestClass>("TestClass");
            if (TestType != nullptr)
            {
                TestType->RegisterProperty(&TestClass::IntField, "IntField")
                    ->RegisterProperty(&TestClass::FloatField, "FloatField")
                    ->RegisterProperty(&TestClass::BoolField, "BoolField");

                std::cout << "TestClass has " << TestType->Properties.Size() << " properties" << std::endl;
                for (const auto& Prop : TestType->Properties)
                {
                    if (Prop != nullptr && Prop->Name.IsValid())
                    {
                        std::cout << "  Property: " << Prop->Name.GetStdString() << ", Offset: " << Prop->Offset;
                        if (Prop->OwnerType != nullptr && Prop->OwnerType->Name.IsValid())
                        {
                            std::cout << ", Owner: " << Prop->OwnerType->Name.GetStdString();
                        }
                        std::cout << std::endl;
                    }
                }

                // 测试GetAllProperties
                auto AllProps = TestType->GetAllProperties();
                std::cout << "GetAllProperties returned " << AllProps.Size() << " properties" << std::endl;
            }
        }

        // 测试继承和GetAllProperties
        {
            spdlog::info("测试继承和GetAllProperties");
            struct BaseClass
            {
                Int32 BaseField;
            };

            struct DerivedClass : public BaseClass
            {
                Float DerivedField;
            };

            FTypeMutable BaseType = FTypeManager::Register<BaseClass>("BaseClass");
            FTypeMutable DerivedType = FTypeManager::Register<DerivedClass>("DerivedClass");

            if (BaseType != nullptr && DerivedType != nullptr)
            {
                BaseType->RegisterProperty(&BaseClass::BaseField, "BaseField");
                DerivedType->RegisterProperty(&DerivedClass::DerivedField, "DerivedField")->RegisterParent(BaseType);

                std::cout << "BaseClass has " << BaseType->Properties.Size() << " properties" << std::endl;
                std::cout << "DerivedClass has " << DerivedType->Properties.Size() << " properties" << std::endl;

                auto AllDerivedProps = DerivedType->GetAllProperties();
                std::cout << "DerivedClass GetAllProperties returned " << AllDerivedProps.Size() << " properties"
                          << std::endl;
                for (const auto& Prop : AllDerivedProps)
                {
                    if (Prop != nullptr && Prop->Name.IsValid())
                    {
                        std::cout << "  Property: " << Prop->Name.GetStdString();
                        if (Prop->OwnerType != nullptr && Prop->OwnerType->Name.IsValid())
                        {
                            std::cout << ", Owner: " << Prop->OwnerType->Name.GetStdString();
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }

        // 测试实例创建和销毁
        {
            spdlog::info("测试实例创建和销毁");
            struct TestStruct
            {
                Int32 Value;
                TestStruct() : Value(42) {}
            };

            FTypeMutable TestType = FTypeManager::Register<TestStruct>("TestStruct");
            if (TestType != nullptr && TestType->CanCreateInstance())
            {
                // 测试CreateInstance
                void* Instance = TestType->CreateInstance();
                if (Instance != nullptr)
                {
                    TestStruct* TypedInstance = static_cast<TestStruct*>(Instance);
                    std::cout << "CreateInstance success, value: " << TypedInstance->Value << std::endl;
                    TestType->DestroyInstance(Instance);
                    std::cout << "DestroyInstance success" << std::endl;
                }

                // 测试CreateInstanceT
                TestStruct* TypedInstance = TestType->CreateInstanceT<TestStruct>();
                if (TypedInstance != nullptr)
                {
                    std::cout << "CreateInstanceT success, value: " << TypedInstance->Value << std::endl;
                    delete TypedInstance;
                }

                // 测试CreateUnique
                auto UniqueInstance = TestType->CreateUnique<TestStruct>();
                if (UniqueInstance != nullptr)
                {
                    std::cout << "CreateUnique success, value: " << UniqueInstance->Value << std::endl;
                }

                // 测试CreateShared
                auto SharedInstance = TestType->CreateShared<TestStruct>();
                if (SharedInstance != nullptr)
                {
                    std::cout << "CreateShared success, value: " << SharedInstance->Value << std::endl;
                }
            }
        }

        // 测试Abstract和Interface类型
        {
            spdlog::info("测试Abstract和Interface类型");
            struct AbstractClass
            {
                Int32 Value;
            };

            struct InterfaceClass
            {
                Int32 Value;
            };

            FTypeMutable AbstractType = FTypeManager::Register<AbstractClass>("AbstractClass");
            FTypeMutable InterfaceType = FTypeManager::Register<InterfaceClass>("InterfaceClass");

            if (AbstractType != nullptr)
            {
                AbstractType->RegisterAttribute(FName("Abstract"), FName("true"));
                std::cout << "AbstractClass IsAbstract: " << AbstractType->IsAbstract() << std::endl;
                std::cout << "AbstractClass CanCreateInstance: " << AbstractType->CanCreateInstance() << std::endl;
            }

            if (InterfaceType != nullptr)
            {
                InterfaceType->RegisterAttribute(FName("Interface"), FName("true"));
                std::cout << "InterfaceClass IsInterface: " << InterfaceType->IsInterface() << std::endl;
                std::cout << "InterfaceClass CanCreateInstance: " << InterfaceType->CanCreateInstance() << std::endl;
            }
        }

        // 测试属性GetValue和SetValue
        {
            spdlog::info("测试属性GetValue和SetValue");
            struct TestClass
            {
                Int32 IntValue;
                Float FloatValue;
            };

            FTypeMutable TestType = FTypeManager::Register<TestClass>("TestClass2");
            if (TestType != nullptr)
            {
                TestType->RegisterProperty(&TestClass::IntValue, "IntValue")
                    ->RegisterProperty(&TestClass::FloatValue, "FloatValue");

                TestClass TestObj;
                TestObj.IntValue = 100;
                TestObj.FloatValue = 3.14f;

                for (const auto& Prop : TestType->Properties)
                {
                    if (Prop != nullptr)
                    {
                        if (Prop->Name.IsValid() && Prop->Name.GetStdString() == "IntValue")
                        {
                            auto IntVal = Prop->GetValue<Int32>(&TestObj);
                            if (IntVal.IsSet())
                            {
                                std::cout << "GetValue IntValue: " << IntVal.GetValue() << std::endl;
                            }

                            // 测试SetValue
                            Prop->SetValue<Int32>(&TestObj, 200);
                            std::cout << "After SetValue, IntValue: " << TestObj.IntValue << std::endl;
                        }
                        else if (Prop->Name.IsValid() && Prop->Name.GetStdString() == "FloatValue")
                        {
                            auto FloatVal = Prop->GetValue<Float>(&TestObj);
                            if (FloatVal.IsSet())
                            {
                                std::cout << "GetValue FloatValue: " << FloatVal.GetValue() << std::endl;
                            }
                        }
                    }
                }
            }
        }

        // 测试延迟注册（先注册注册函数，使用时再执行注册）
        {
            spdlog::info("测试延迟注册功能");

            // 定义一个测试类型
            struct LazyTestStruct
            {
                Int32 Value;
                Float FloatValue;
            };

            // 注册函数：定义如何注册这个类型（使用 lambda）
            auto RegisterLazyTestStruct = []()
            {
                std::cout << "  [延迟注册] 正在注册 LazyTestStruct..." << std::endl;
                FTypeMutable LazyType = FTypeManager::Register<LazyTestStruct>("LazyTestStruct");
                if (LazyType != nullptr)
                {
                    LazyType->RegisterProperty(&LazyTestStruct::Value, "Value")
                        ->RegisterProperty(&LazyTestStruct::FloatValue, "FloatValue");
                    std::cout << "  [延迟注册] LazyTestStruct 注册完成，大小: " << LazyType->Size << std::endl;
                }
            };

            // 先注册注册函数，但不立即执行
            FTypeManager::RegisterTypeRegisterer<LazyTestStruct>(RegisterLazyTestStruct);
            std::cout << "已注册 LazyTestStruct 的注册函数" << std::endl;

            // 此时类型应该还未注册
            FType LazyTypeBefore = TypeOf<LazyTestStruct>();
            if (LazyTypeBefore == nullptr)
            {
                std::cout << "LazyTestStruct 尚未注册（符合预期）" << std::endl;
            }

            // 使用 TypeOf 触发延迟注册
            std::cout << "调用 TypeOf<LazyTestStruct>() 触发延迟注册..." << std::endl;
            FType LazyTypeAfter = TypeOf<LazyTestStruct>();
            if (LazyTypeAfter != nullptr)
            {
                std::cout << "LazyTestStruct 已通过延迟注册成功注册" << std::endl;
                std::cout << "类型名: " << LazyTypeAfter->Name.GetStdString() << std::endl;
                std::cout << "类型大小: " << LazyTypeAfter->Size << std::endl;
                std::cout << "属性数量: " << LazyTypeAfter->Properties.Size() << std::endl;
                for (const auto& Prop : LazyTypeAfter->Properties)
                {
                    if (Prop != nullptr && Prop->Name.IsValid())
                    {
                        std::cout << "  属性: " << Prop->Name.GetStdString() << std::endl;
                    }
                }
            }
            else
            {
                std::cout << "延迟注册失败" << std::endl;
            }

            // 再次调用 TypeOf，应该直接返回已注册的类型（不会再次调用注册函数）
            std::cout << "再次调用 TypeOf<LazyTestStruct>()..." << std::endl;
            FType LazyTypeAgain = TypeOf<LazyTestStruct>();
            if (LazyTypeAgain != nullptr && LazyTypeAgain == LazyTypeAfter)
            {
                std::cout << "返回已注册的类型（未重复注册）" << std::endl;
            }
        }

        // 测试多个类型的延迟注册
        {
            spdlog::info("测试多个类型的延迟注册");

            struct TypeA
            {
                Int32 A;
            };

            struct TypeB
            {
                Float B;
            };

            struct TypeC
            {
                Bool C;
            };

            // 注册多个类型的注册函数
            FTypeManager::RegisterTypeRegisterer<TypeA>(
                []()
                {
                    std::cout << "  [延迟注册] 注册 TypeA" << std::endl;
                    FTypeManager::Register<TypeA>("TypeA")->RegisterProperty(&TypeA::A, "A");
                });

            FTypeManager::RegisterTypeRegisterer<TypeB>(
                []()
                {
                    std::cout << "  [延迟注册] 注册 TypeB" << std::endl;
                    FTypeManager::Register<TypeB>("TypeB")->RegisterProperty(&TypeB::B, "B");
                });

            FTypeManager::RegisterTypeRegisterer<TypeC>(
                []()
                {
                    std::cout << "  [延迟注册] 注册 TypeC" << std::endl;
                    FTypeManager::Register<TypeC>("TypeC")->RegisterProperty(&TypeC::C, "C");
                });

            std::cout << "已注册 3 个类型的注册函数" << std::endl;

            // 按需触发注册
            std::cout << "触发 TypeA 的延迟注册..." << std::endl;
            FType TypeAType = TypeOf<TypeA>();
            if (TypeAType != nullptr)
            {
                std::cout << "TypeA 注册成功: " << TypeAType->Name.GetStdString() << std::endl;
            }

            std::cout << "触发 TypeB 的延迟注册..." << std::endl;
            FType TypeBType = TypeOf<TypeB>();
            if (TypeBType != nullptr)
            {
                std::cout << "TypeB 注册成功: " << TypeBType->Name.GetStdString() << std::endl;
            }

            // TypeC 不触发注册，验证它确实没有被注册
            FType TypeCType = TypeOf<TypeC>();
            if (TypeCType == nullptr)
            {
                std::cout << "TypeC 未注册（符合预期，因为未调用 TypeOf）" << std::endl;
            }
            else
            {
                std::cout << "TypeC 已注册: " << TypeCType->Name.GetStdString() << std::endl;
            }
        }
    }

    std::cout << "\n=== All Tests Complete ===" << std::endl;
    return 0;
}