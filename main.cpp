#ifdef HK_WINDOWS
#include <windows.h>
#endif
#include "Core/Logging/Logger.h"
#include "Core/Reflection/Property.h"
#include "Core/Reflection/TypeManager.h"
#include "Core/Serialization/BinaryArchive.h"
#include "Core/Serialization/JsonArchive.h"
#include "Core/Serialization/XMLArchive.h"
#include "Math/Color.h"
#include "Math/TestClass.h"
#include "Math/TestStruct.h"

#include <fstream>
#include <iomanip>
#include <iostream>

void TestReflection()
{
    HK_LOG_INFO(ELogcat::Reflection, "========== 反射测试 ==========");

    // 初始化类型系统
    FTypeManager::Get().InitializeAllTypes();

    // 测试 TestStruct
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FBaseStruct ---");
    FType BaseStructType = FTypeManager::TypeOf<FBaseStruct>();
    if (BaseStructType != nullptr)
    {
        HK_LOG_INFO(ELogcat::Reflection, "类型名称: {}", BaseStructType->Name.GetStdString());
        TArray<FProperty> Props = BaseStructType->GetAllProperties();
        HK_LOG_INFO(ELogcat::Reflection, "属性数量: {}", Props.Size());
        for (FProperty Prop : Props)
        {
            HK_LOG_INFO(ELogcat::Reflection, "  - 属性: {}", Prop->Name.GetStdString());
        }

        // 创建对象并测试属性访问
        FBaseStruct BaseStruct;
        BaseStruct.BaseValue = 100;

        for (FProperty Prop : Props)
        {
            if (Prop->Name.GetStdString() == "BaseValue")
            {
                TOptional<Int32> Value = Prop->GetValue<Int32>(&BaseStruct);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  BaseValue = {}", Value.GetValue());
                }

                // 测试设置值
                Prop->SetValue<Int32>(&BaseStruct, 200);
                Value = Prop->GetValue<Int32>(&BaseStruct);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  设置后 BaseValue = {}", Value.GetValue());
                }
            }
        }
    }

    // 测试 DerivedStruct
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FDerivedStruct ---");
    FType DerivedStructType = FTypeManager::TypeOf<FDerivedStruct>();
    if (DerivedStructType != nullptr)
    {
        HK_LOG_INFO(ELogcat::Reflection, "类型名称: {}", DerivedStructType->Name.GetStdString());

        // 检查父类
        TArray<FType> Bases = DerivedStructType->Bases;
        HK_LOG_INFO(ELogcat::Reflection, "父类数量: {}", Bases.Size());
        for (FType Base : Bases)
        {
            if (Base != nullptr)
            {
                HK_LOG_INFO(ELogcat::Reflection, "  - 父类: {}", Base->Name.GetStdString());
            }
        }

        // 获取所有属性（包括父类的）
        TArray<FProperty> AllProps = DerivedStructType->GetAllProperties();
        HK_LOG_INFO(ELogcat::Reflection, "所有属性数量（包括父类）: {}", AllProps.Size());
        for (FProperty Prop : AllProps)
        {
            HK_LOG_INFO(ELogcat::Reflection, "  - 属性: {}", Prop->Name.GetStdString());
        }

        // 创建对象并测试属性访问
        FDerivedStruct DerivedStruct;
        DerivedStruct.BaseValue = 10;
        DerivedStruct.DerivedValue = 3.14f;

        for (FProperty Prop : AllProps)
        {
            if (Prop->Name.GetStdString() == "BaseValue")
            {
                TOptional<Int32> Value = Prop->GetValue<Int32>(&DerivedStruct);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  BaseValue = {}", Value.GetValue());
                }
            }
            else if (Prop->Name.GetStdString() == "DerivedValue")
            {
                TOptional<Float> Value = Prop->GetValue<Float>(&DerivedStruct);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  DerivedValue = {}", Value.GetValue());
                }
            }
        }
    }

    // 测试 TestClass
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FBaseClass ---");
    FType BaseClassType = FTypeManager::TypeOf<FBaseClass>();
    if (BaseClassType != nullptr)
    {
        HK_LOG_INFO(ELogcat::Reflection, "类型名称: {}", BaseClassType->Name.GetStdString());
        TArray<FProperty> Props = BaseClassType->GetAllProperties();
        HK_LOG_INFO(ELogcat::Reflection, "属性数量: {}", Props.Size());

        FBaseClass BaseClass;
        BaseClass.BaseValue = 50;

        for (FProperty Prop : Props)
        {
            if (Prop->Name.GetStdString() == "BaseValue")
            {
                TOptional<Int32> Value = Prop->GetValue<Int32>(&BaseClass);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  BaseValue = {}", Value.GetValue());
                }
            }
        }
    }

    // 测试 DerivedClass
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FDerivedClass ---");
    FType DerivedClassType = FTypeManager::TypeOf<FDerivedClass>();
    if (DerivedClassType != nullptr)
    {
        HK_LOG_INFO(ELogcat::Reflection, "类型名称: {}", DerivedClassType->Name.GetStdString());

        // 检查父类
        TArray<FType> Bases = DerivedClassType->Bases;
        HK_LOG_INFO(ELogcat::Reflection, "父类数量: {}", Bases.Size());
        for (FType Base : Bases)
        {
            if (Base != nullptr)
            {
                HK_LOG_INFO(ELogcat::Reflection, "  - 父类: {}", Base->Name.GetStdString());
            }
        }

        // 获取所有属性
        TArray<FProperty> AllProps = DerivedClassType->GetAllProperties();
        HK_LOG_INFO(ELogcat::Reflection, "所有属性数量（包括父类）: {}", AllProps.Size());

        FDerivedClass DerivedClass;
        DerivedClass.BaseValue = 20;
        DerivedClass.DerivedValue = 2.718f;

        for (FProperty Prop : AllProps)
        {
            if (Prop->Name.GetStdString() == "BaseValue")
            {
                TOptional<Int32> Value = Prop->GetValue<Int32>(&DerivedClass);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  BaseValue = {}", Value.GetValue());
                }
            }
            else if (Prop->Name.GetStdString() == "DerivedValue")
            {
                TOptional<Float> Value = Prop->GetValue<Float>(&DerivedClass);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  DerivedValue = {}", Value.GetValue());
                }
            }
        }
    }

    // 测试 SimpleClass
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FSimpleClass ---");
    FType SimpleClassType = FTypeManager::TypeOf<FSimpleClass>();
    if (SimpleClassType != nullptr)
    {
        HK_LOG_INFO(ELogcat::Reflection, "类型名称: {}", SimpleClassType->Name.GetStdString());
        TArray<FProperty> Props = SimpleClassType->GetAllProperties();
        HK_LOG_INFO(ELogcat::Reflection, "属性数量: {}", Props.Size());

        FSimpleClass SimpleClass;
        SimpleClass.Value = 999;

        for (FProperty Prop : Props)
        {
            if (Prop->Name.GetStdString() == "Value")
            {
                TOptional<Int32> Value = Prop->GetValue<Int32>(&SimpleClass);
                if (Value.IsSet())
                {
                    HK_LOG_INFO(ELogcat::Reflection, "  Value = {}", Value.GetValue());
                }
            }
        }
    }
}

void TestSerialization()
{
    HK_LOG_INFO(ELogcat::Reflection, "\n========== 序列化测试 ==========");

    // 测试 FBaseStruct 序列化
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FBaseStruct 序列化 ---");
    {
        FBaseStruct BaseStruct;
        BaseStruct.BaseValue = 123;

        {
            // 序列化到文件
            std::ofstream ofs("test_base_struct.json");
            if (ofs.is_open())
            {
                {
                    FJsonOutputArchive OutputArchive(ofs);
                    BaseStruct.Serialize(OutputArchive);
                }
                ofs.flush();
                ofs.close();
                HK_LOG_INFO(ELogcat::Reflection, "已序列化到 test_base_struct.json");
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_base_struct.json 进行写入");
            }
        }

        {
            // 从文件反序列化
            FBaseStruct LoadedStruct;
            std::ifstream ifs("test_base_struct.json");
            if (ifs.is_open() && ifs.good())
            {
                try
                {
                    {
                        FJsonInputArchive InputArchive(ifs);
                        LoadedStruct.Serialize(InputArchive);
                    }
                    ifs.close();

                    HK_LOG_INFO(ELogcat::Reflection, "反序列化后 BaseValue = {}", LoadedStruct.BaseValue);
                }
                catch (std::exception& e)
                {
                    HK_LOG_INFO(ELogcat::Reflection, "反序列化时发生异常: {}", e.what());
                }
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_base_struct.json 进行读取");
            }
        }
    }

    // 测试 FDerivedStruct 序列化
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FDerivedStruct 序列化 ---");
    {
        FDerivedStruct DerivedStruct;
        DerivedStruct.BaseValue = 456;
        DerivedStruct.DerivedValue = 1.234f;

        {
            // 序列化到文件
            std::ofstream ofs("test_derived_struct.json");
            if (ofs.is_open())
            {
                {
                    FJsonOutputArchive OutputArchive(ofs);
                    DerivedStruct.Serialize(OutputArchive);
                }
                ofs.flush();
                ofs.close();
                HK_LOG_INFO(ELogcat::Reflection, "已序列化到 test_derived_struct.json");
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_derived_struct.json 进行写入");
            }
        }
        {
            // 从文件反序列化
            FDerivedStruct LoadedStruct;
            std::ifstream ifs("test_derived_struct.json");
            if (ifs.is_open() && ifs.good())
            {
                {
                    FJsonInputArchive InputArchive(ifs);
                    LoadedStruct.Serialize(InputArchive);
                }
                ifs.close();

                HK_LOG_INFO(ELogcat::Reflection, "反序列化后 BaseValue = {}, DerivedValue = {}", LoadedStruct.BaseValue,
                            LoadedStruct.DerivedValue);
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_derived_struct.json 进行读取");
            }
        }
    }

    // 测试 FBaseClass 序列化
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FBaseClass 序列化 ---");
    {
        FBaseClass BaseClass;
        BaseClass.BaseValue = 789;

        {
            // 序列化到文件
            std::ofstream ofs("test_base_class.json");
            if (ofs.is_open())
            {
                {
                    FJsonOutputArchive OutputArchive(ofs);
                    BaseClass.Serialize(OutputArchive);
                }
                ofs.flush();
                ofs.close();
                HK_LOG_INFO(ELogcat::Reflection, "已序列化到 test_base_class.json");
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_base_class.json 进行写入");
            }
        }

        {
            // 从文件反序列化
            FBaseClass LoadedClass;
            std::ifstream ifs("test_base_class.json");
            if (ifs.is_open() && ifs.good())
            {
                {
                    FJsonInputArchive InputArchive(ifs);
                    LoadedClass.Serialize(InputArchive);
                }
                ifs.close();

                HK_LOG_INFO(ELogcat::Reflection, "反序列化后 BaseValue = {}", LoadedClass.BaseValue);
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_base_class.json 进行读取");
            }
        }
    }

    // 测试 FDerivedClass 序列化
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FDerivedClass 序列化 ---");
    {
        FDerivedClass DerivedClass;
        DerivedClass.BaseValue = 111;
        DerivedClass.DerivedValue = 5.678f;

        {
            // 序列化到文件
            std::ofstream ofs("test_derived_class.json");
            if (ofs.is_open())
            {
                {
                    FJsonOutputArchive OutputArchive(ofs);
                    DerivedClass.Serialize(OutputArchive);
                }
                ofs.flush();
                ofs.close();
                HK_LOG_INFO(ELogcat::Reflection, "已序列化到 test_derived_class.json");
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_derived_class.json 进行写入");
            }

            // 从文件反序列化
            FDerivedClass LoadedClass;
            std::ifstream ifs("test_derived_class.json");
            if (ifs.is_open() && ifs.good())
            {
                {
                    FJsonInputArchive InputArchive(ifs);
                    LoadedClass.Serialize(InputArchive);
                }
                ifs.close();

                HK_LOG_INFO(ELogcat::Reflection, "反序列化后 BaseValue = {}, DerivedValue = {}", LoadedClass.BaseValue,
                            LoadedClass.DerivedValue);
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_derived_class.json 进行读取");
            }
        }
    }

    // 测试 FSimpleClass 序列化
    HK_LOG_INFO(ELogcat::Reflection, "\n--- 测试 FSimpleClass 序列化 ---");
    {
        FSimpleClass SimpleClass;
        SimpleClass.Value = 888;
        FColor Color;

        {
            // 序列化到文件
            std::ofstream ofs("test_simple_class.xml");
            if (ofs.is_open())
            {
                {
                    FXMLOutputArchive OutputArchive(ofs);
                    Color.Serialize(OutputArchive);
                }
                ofs.flush();
                ofs.close();
                HK_LOG_INFO(ELogcat::Reflection, "已序列化到 test_simple_class.json");
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_simple_class.json 进行写入");
            }
        }
        {
            // 从文件反序列化
            FColor Color1;
            std::ifstream ifs("test_simple_class.xml");
            if (ifs.is_open() && ifs.good())
            {
                {
                    FXMLInputArchive InputArchive(ifs);
                    Color1.Serialize(InputArchive);
                }
                ifs.close();

                HK_LOG_INFO(ELogcat::Reflection, "反序列化后 Value = {}", Color1.R);
            }
            else
            {
                HK_LOG_ERROR(ELogcat::Reflection, "无法打开文件 test_simple_class.json 进行读取");
            }
        }
    }
}

int main()
{
#ifdef HK_WINDOWS
    // 设置控制台为UTF-8编码以支持中文显示
    SetConsoleOutputCP(65001); // UTF-8 code page
    SetConsoleCP(65001);       // UTF-8 code page
#endif

    // 测试反射功能
    TestReflection();

    // 测试序列化功能
    TestSerialization();

    HK_LOG_INFO(ELogcat::Reflection, "\n========== 所有测试完成 ==========");

    return 0;
}
