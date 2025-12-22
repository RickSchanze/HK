//
// Created by Admin on 2025/12/21.
//

#include "SlangCompiler.h"

#include "Config/ConfigManager.h"
#include "Core/Container/FixedArray.h"
#include "Core/Logging/Logger.h"
#include "Core/String/String.h"
#include "Core/Utility/Profiler.h"
#include "Render/RenderConfig.h"
#include "slang-com-ptr.h"
#include "slang/slang.h"
#include <filesystem>
#include <fstream>

using namespace slang;

class FSlangCompiler::FImpl
{
public:
    Slang::ComPtr<slang::IGlobalSession> GlobalSession;
    Slang::ComPtr<slang::ISession> CompileSession;
    TFixedArray<Int32, (Int32)EShaderCompileTarget::Count> LanguageIndices;

    FImpl()
    {
        slang::createGlobalSession(GlobalSession.writeRef());
        std::ranges::fill(LanguageIndices, -1);
        const auto Cfg = FConfigManager::Get()->GetConfig<FRenderConfig>();
        TArray<const char*> SearchPaths;
        if (Cfg)
        {
            for (auto& Path : Cfg->GetShaderPaths())
            {
                SearchPaths.Add(Path.CStr());
            }
        }
        slang::SessionDesc Desc{};
        slang::TargetDesc TargetDesc[Int32(EShaderCompileTarget::Count)];

        // SPIRV target
        TargetDesc[0].format = SLANG_SPIRV;
        TargetDesc[0].profile = GlobalSession->findProfile("glsl_460");
        LanguageIndices[Int32(EShaderCompileTarget::Spirv)] = 0;

        // GLSL target
        TargetDesc[1].format = SLANG_GLSL;
        TargetDesc[1].profile = GlobalSession->findProfile("glsl_460");
        LanguageIndices[Int32(EShaderCompileTarget::GLSL)] = 1;

        // HLSL target
        TargetDesc[2].format = SLANG_HLSL;
        TargetDesc[2].profile = GlobalSession->findProfile("sm_6_0");
        LanguageIndices[Int32(EShaderCompileTarget::HLSL)] = 2;

        Desc.searchPaths = SearchPaths.Data();
        Desc.searchPathCount = static_cast<SlangInt>(SearchPaths.Size());
        Desc.targets = TargetDesc;
        Desc.targetCount = 3;
        GlobalSession->createSession(Desc, CompileSession.writeRef());
    }

    ~FImpl()
    {
        CompileSession = {};
        GlobalSession = {};
    }

    Int32 GetCompileTargetIndex(EShaderCompileTarget Target)
    {
        if (Target >= EShaderCompileTarget::Count)
        {
            return -1;
        }
        return LanguageIndices[Int32(Target)];
    }

    static bool ProcessReflectFixedParameters(slang::ProgramLayout* Layout, FShaderParameterSheet& OutParameterSheet)
    {
        const SlangInt GlobalParamCount = Layout->getParameterCount();

        for (SlangInt Index = 0; Index < GlobalParamCount; Index++)
        {
            const auto Param = Layout->getParameterByIndex(Index);
            if (!Param)
                continue;

            const auto Variable = Param->getVariable();
            if (!Variable)
                continue;

            const char* VarName = Variable->getName();
            if (!VarName)
                continue;

            FStringView VarNameView(VarName);

            // 检查固定参数名称
            if (VarNameView == "GCamera")
            {
                OutParameterSheet.bNeedCamera = true;
            }
            else if (VarNameView == "GModel")
            {
                OutParameterSheet.bNeedModel = true;
            }
            else if (VarNameView == "GTexturePool" || VarNameView == "GSamplerPool")
            {
                OutParameterSheet.bNeedResourcePool = true;
            }
        }

        return true;
    }

    static bool ProcessReflectPushConstant(slang::ProgramLayout* Layout, FShaderParameterSheet& OutParameterSheet,
                                           FString& OutErrorMessage)
    {
        const SlangInt GlobalParamCount = Layout->getParameterCount();
        bool IsPushConstantProcessed = false;

        for (SlangInt Index = 0; Index < GlobalParamCount; Index++)
        {
            const auto Param = Layout->getParameterByIndex(Index);
            if (!Param)
                continue;

            const auto Variable = Param->getVariable();
            if (!Variable)
                continue;

            const auto Category = Param->getCategory();
            const auto TypeLayout = Param->getTypeLayout();

            if (Category == slang::PushConstantBuffer)
            {
                if (IsPushConstantProcessed)
                {
                    OutErrorMessage = "PushConstant变量一个Shader只能出现一次.";
                    return false;
                }

                IsPushConstantProcessed = true;
                const auto ElementTypeLayout = TypeLayout->getElementTypeLayout();
                const auto PushConstantKind = ElementTypeLayout->getKind();

                if (PushConstantKind != slang::TypeReflection::Kind::Struct)
                {
                    OutErrorMessage = "PushConstant需要是一个结构体";
                    return false;
                }

                // 反射 PushConstant 结构体成员
                const SlangInt MemberCount = ElementTypeLayout->getFieldCount();
                for (SlangInt MemberIndex = 0; MemberIndex < MemberCount; MemberIndex++)
                {
                    const auto VarLayout = ElementTypeLayout->getFieldByIndex(MemberIndex);
                    if (!VarLayout)
                        continue;

                    const auto Var = VarLayout->getVariable();
                    if (!Var)
                        continue;

                    FShaderPushConstantItem Item;
                    Item.Name = FName(Var->getName());
                    Item.Offset = static_cast<UInt32>(VarLayout->getOffset());
                    Item.Size = static_cast<UInt32>(VarLayout->getTypeLayout()->getSize());

                    OutParameterSheet.PushConstants.Add(Item);
                }
            }
        }

        return true;
    }

    bool LoadShaderModule(const FString& ShaderPath, Slang::ComPtr<slang::IBlob>& OutDiagnostics,
                          slang::IModule*& OutModule)
    {
        OutModule = CompileSession->loadModule(ShaderPath.CStr(), OutDiagnostics.writeRef());
        if (OutModule != nullptr)
        {
            if (OutDiagnostics != nullptr)
            {
                HK_LOG_WARN(ELogcat::Shader, "编译 {} 警告: \n{}", ShaderPath, (const char*)OutDiagnostics->getBufferPointer());
            }
            return true;
        }
        return false;
    }

    bool FindEntryPoints(slang::IModule* Module, Slang::ComPtr<slang::IEntryPoint>& OutVertexEntry,
                         Slang::ComPtr<slang::IEntryPoint>& OutFragmentEntry, FString& OutErrorMessage)
    {
        Module->findEntryPointByName("VertexMain", OutVertexEntry.writeRef());
        if (!OutVertexEntry)
        {
            OutErrorMessage = "找不到 VertexMain 入口点";
            return false;
        }

        Module->findEntryPointByName("FragmentMain", OutFragmentEntry.writeRef());
        if (!OutFragmentEntry)
        {
            OutErrorMessage = "找不到 FragmentMain 入口点";
            return false;
        }

        return true;
    }

    bool CreateProgram(slang::IModule* Module, slang::IEntryPoint* VertexEntry, slang::IEntryPoint* FragmentEntry,
                       Slang::ComPtr<slang::IComponentType>& OutProgram, FString& OutErrorMessage)
    {
        TArray<slang::IComponentType*> Components = {Module, VertexEntry, FragmentEntry};
        Slang::ComPtr<slang::IBlob> Diagnostics;
        CompileSession->createCompositeComponentType(Components.Data(), static_cast<SlangInt>(Components.Size()),
                                                     OutProgram.writeRef(), Diagnostics.writeRef());
        if (Diagnostics)
        {
            OutErrorMessage = Diagnostics->getBufferSize() ? static_cast<const char*>(Diagnostics->getBufferPointer())
                                                           : "Unknown Error";
            return false;
        }

        return true;
    }

    bool ReflectShaderParameters(slang::ProgramLayout* Layout, FShaderParameterSheet& OutParameterSheet,
                                 FString& OutErrorMessage)
    {
        if (!ProcessReflectFixedParameters(Layout, OutParameterSheet))
        {
            OutErrorMessage = "反射固定参数失败";
            return false;
        }

        if (!ProcessReflectPushConstant(Layout, OutParameterSheet, OutErrorMessage))
        {
            return false;
        }

        return true;
    }

    bool GetEntryPointIndices(slang::ProgramLayout* Layout, SlangInt& OutVertexIndex, SlangInt& OutFragmentIndex,
                              FString& OutErrorMessage)
    {
        OutVertexIndex = -1;
        OutFragmentIndex = -1;

        const SlangInt EntryPointCount = Layout->getEntryPointCount();
        for (SlangInt Index = 0; Index < EntryPointCount; Index++)
        {
            slang::EntryPointLayout* EntryPoint = Layout->getEntryPointByIndex(Index);
            switch (EntryPoint->getStage())
            {
                case SLANG_STAGE_VERTEX:
                    OutVertexIndex = Index;
                    break;
                case SLANG_STAGE_FRAGMENT:
                    OutFragmentIndex = Index;
                    break;
                default:
                    break;
            }
        }

        if (OutVertexIndex == -1 || OutFragmentIndex == -1)
        {
            OutErrorMessage = "无法找到顶点或片段着色器入口点";
            return false;
        }

        return true;
    }

    bool CompileShaderStage(slang::IComponentType* Program, SlangInt StageIndex, Int32 TargetIndex,
                            Slang::ComPtr<slang::IBlob>& OutCode, FString& OutErrorMessage)
    {
        Slang::ComPtr<slang::IBlob> Diagnostics;
        SlangResult Result =
            Program->getEntryPointCode(StageIndex, TargetIndex, OutCode.writeRef(), Diagnostics.writeRef());
        if (SLANG_FAILED(Result))
        {
            OutErrorMessage = Diagnostics && Diagnostics->getBufferSize()
                                  ? static_cast<const char*>(Diagnostics->getBufferPointer())
                                  : "编译着色器阶段失败";
            return false;
        }

        return true;
    }

    void WriteShaderCode(const Slang::ComPtr<slang::IBlob>& VertexCode, const Slang::ComPtr<slang::IBlob>& FragmentCode,
                         EShaderCompileTarget Target, TArray<UInt32>& OutCode)
    {
        // 如果目标是 SPIRV，合并两个着色器的代码
        if (Target == EShaderCompileTarget::Spirv)
        {
            const UInt32 VertexCodeSize = static_cast<UInt32>(VertexCode->getBufferSize());
            const UInt32 FragmentCodeSize = static_cast<UInt32>(FragmentCode->getBufferSize());

            const UInt32 VertexWordCount = VertexCodeSize / sizeof(UInt32);
            const UInt32 FragmentWordCount = FragmentCodeSize / sizeof(UInt32);

            OutCode.Reserve(VertexWordCount + FragmentWordCount);
            const UInt32* VertexData = reinterpret_cast<const UInt32*>(VertexCode->getBufferPointer());
            const UInt32* FragmentData = reinterpret_cast<const UInt32*>(FragmentCode->getBufferPointer());

            OutCode.Append(VertexData, VertexData + VertexWordCount);
            OutCode.Append(FragmentData, FragmentData + FragmentWordCount);
        }
        else
        {
            // 对于 GLSL/HLSL，只保存顶点着色器代码
            const UInt32 CodeSize = static_cast<UInt32>(VertexCode->getBufferSize());
            const UInt32 WordCount = CodeSize / sizeof(UInt32);
            const UInt32* CodeData = reinterpret_cast<const UInt32*>(VertexCode->getBufferPointer());
            OutCode.Append(CodeData, CodeData + WordCount);
        }
    }

    void WriteDebugOutput(const FShaderCompileRequest& Request, slang::IComponentType* Program,
                          SlangInt VertexStageIndex, SlangInt FragmentStageIndex)
    {
        if (Request.DebugOutputTarget == EShaderCompileTarget::Count || Request.DebugOutputPath.IsEmpty())
        {
            return;
        }

        const Int32 DebugTargetIndex = GetCompileTargetIndex(Request.DebugOutputTarget);
        if (DebugTargetIndex == -1)
        {
            return;
        }

        // 获取调试代码
        Slang::ComPtr<slang::IBlob> Diagnostics;
        Slang::ComPtr<slang::IBlob> DebugVertexCode;
        Slang::ComPtr<slang::IBlob> DebugFragmentCode;

        Program->getEntryPointCode(VertexStageIndex, DebugTargetIndex, DebugVertexCode.writeRef(),
                                   Diagnostics.writeRef());
        Program->getEntryPointCode(FragmentStageIndex, DebugTargetIndex, DebugFragmentCode.writeRef(),
                                   Diagnostics.writeRef());

        // 确定文件扩展名
        FString Extension;
        if (Request.DebugOutputTarget == EShaderCompileTarget::GLSL)
        {
            Extension = ".glsl";
        }
        else if (Request.DebugOutputTarget == EShaderCompileTarget::HLSL)
        {
            Extension = ".hlsl";
        }
        else
        {
            Extension = ".txt";
        }

        // 确保目录存在
        const std::filesystem::path DebugPath(Request.DebugOutputPath.CStr());
        const std::filesystem::path DebugDir = DebugPath.parent_path();
        if (!DebugDir.empty() && !std::filesystem::exists(DebugDir))
        {
            std::filesystem::create_directories(DebugDir);
        }

        // 写入调试文件
        FString DebugFilePath = Request.DebugOutputPath;
        if (!DebugFilePath.EndsWith(Extension.CStr()))
        {
            DebugFilePath += Extension;
        }

        std::ofstream DebugFile(DebugFilePath.CStr());
        if (DebugFile.is_open())
        {
            DebugFile << "// ============================================\n";
            DebugFile << "// Vertex Shader\n";
            DebugFile << "// ============================================\n\n";
            if (DebugVertexCode && DebugVertexCode->getBufferSize() > 0)
            {
                DebugFile << static_cast<const char*>(DebugVertexCode->getBufferPointer());
            }
            DebugFile << "\n\n// ============================================\n";
            DebugFile << "// Fragment Shader\n";
            DebugFile << "// ============================================\n\n";
            if (DebugFragmentCode && DebugFragmentCode->getBufferSize() > 0)
            {
                DebugFile << static_cast<const char*>(DebugFragmentCode->getBufferPointer());
            }
            DebugFile.close();
            HK_LOG_INFO(ELogcat::Shader, "调试着色器代码已写入: {}", DebugFilePath);
        }
        else
        {
            HK_LOG_WARN(ELogcat::Shader, "无法写入调试着色器文件: {}", DebugFilePath);
        }
    }

    bool RequestCompileGraphicsShader(const FShaderCompileRequest& Request, FShaderCompileResult& OutResult)
    {
        // 加载着色器模块
        Slang::ComPtr<slang::IBlob> Diagnostics;
        slang::IModule* Module = nullptr;
        if (!LoadShaderModule(Request.ShaderPath, Diagnostics, Module))
        {
            OutResult.ErrorMessage = Diagnostics && Diagnostics->getBufferSize()
                                         ? static_cast<const char*>(Diagnostics->getBufferPointer())
                                         : "Unknown Error";
            return false;
        }

        // 查找入口点
        Slang::ComPtr<slang::IEntryPoint> VertexEntryPoint;
        Slang::ComPtr<slang::IEntryPoint> FragmentEntryPoint;
        if (!FindEntryPoints(Module, VertexEntryPoint, FragmentEntryPoint, OutResult.ErrorMessage))
        {
            return false;
        }

        // 3. 创建程序
        Slang::ComPtr<slang::IComponentType> Program;
        if (!CreateProgram(Module, VertexEntryPoint, FragmentEntryPoint, Program, OutResult.ErrorMessage))
        {
            return false;
        }

        // 获取 ProgramLayout 用于反射
        slang::ProgramLayout* ProgLayout = Program->getLayout();
        if (!ProgLayout)
        {
            OutResult.ErrorMessage = "无法获取 ProgramLayout";
            return false;
        }

        // 反射着色器参数
        FString ReflectErrorMessage;
        if (!ReflectShaderParameters(ProgLayout, OutResult.ParameterSheet, ReflectErrorMessage))
        {
            OutResult.ErrorMessage = ReflectErrorMessage;
            return false;
        }

        // 6. 获取入口点索引
        SlangInt VertexStageIndex = -1;
        SlangInt FragmentStageIndex = -1;
        if (!GetEntryPointIndices(ProgLayout, VertexStageIndex, FragmentStageIndex, OutResult.ErrorMessage))
        {
            return false;
        }

        // 7. 获取编译目标索引
        const Int32 TargetIndex = GetCompileTargetIndex(Request.Target);
        if (TargetIndex == -1)
        {
            OutResult.ErrorMessage = "无效的编译目标";
            return false;
        }

        // 8. 编译顶点着色器
        Slang::ComPtr<slang::IBlob> VertexCode;
        FString VertexErrorMessage;
        if (!CompileShaderStage(Program, VertexStageIndex, TargetIndex, VertexCode, VertexErrorMessage))
        {
            OutResult.ErrorMessage = FString("编译顶点着色器失败: ") + VertexErrorMessage;
            return false;
        }

        // 9. 编译片段着色器
        Slang::ComPtr<slang::IBlob> FragmentCode;
        FString FragmentErrorMessage;
        if (!CompileShaderStage(Program, FragmentStageIndex, TargetIndex, FragmentCode, FragmentErrorMessage))
        {
            OutResult.ErrorMessage = FString("编译片段着色器失败: ") + FragmentErrorMessage;
            return false;
        }

        // 10. 写入着色器代码
        WriteShaderCode(VertexCode, FragmentCode, Request.Target, OutResult.Code);

        // 11. 写入调试输出（如果指定）
        WriteDebugOutput(Request, Program, VertexStageIndex, FragmentStageIndex);

        return true;
    }
};

void FSlangCompiler::StartUp()
{
    Impl = new FImpl();
}

void FSlangCompiler::ShutDown()
{
    delete Impl;
    Impl = nullptr;
}

bool FSlangCompiler::RequestCompileGraphicsShader(const FShaderCompileRequest& Request, FShaderCompileResult& OutResult)
{
    if (!Impl)
    {
        OutResult.ErrorMessage = "FSlangCompiler 未初始化";
        return false;
    }

    return Impl->RequestCompileGraphicsShader(Request, OutResult);
}