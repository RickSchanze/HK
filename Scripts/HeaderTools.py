#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
代码生成工具：根据 HSTRUCT 和 HPROPERTY 宏生成反射注册代码
类似虚幻引擎的代码生成系统
"""

import os
import re
import hashlib
import multiprocessing
from pathlib import Path
from typing import List, Dict, Tuple, Optional
from dataclasses import dataclass
from concurrent.futures import ProcessPoolExecutor, as_completed


@dataclass
class PropertyInfo:
    """属性信息"""
    name: str
    type: str
    line: int
    attributes: Dict[str, str]  # Key -> Value


@dataclass
class StructInfo:
    """结构体信息"""
    name: str
    properties: List[PropertyInfo]
    attributes: Dict[str, str]  # Key -> Value
    line: int
    file_path: str
    type: str  # "struct" 或 "class"
    base_class: Optional[str]  # 父类名称，如果没有则为 None
    macro_type: str  # "HSTRUCT" 或 "HCLASS"
    has_hk_api: bool = False  # 是否有 HK_API 导出宏


@dataclass
class EnumMemberInfo:
    """枚举成员信息"""
    name: str
    value: Optional[str]  # 如果有显式值
    line: int


@dataclass
class EnumInfo:
    """枚举信息"""
    name: str
    members: List[EnumMemberInfo]
    attributes: Dict[str, str]  # Key -> Value
    line: int
    file_path: str


def parse_henum_attributes(attr_str: str) -> Dict[str, str]:
    """
    解析 HENUM 的参数（与 HSTRUCT 相同）
    """
    return parse_hstruct_attributes(attr_str)


def parse_hstruct_attributes(attr_str: str) -> Dict[str, str]:
    """
    解析 HSTRUCT 的参数
    支持格式：
    - HSTRUCT() -> {}
    - HSTRUCT(Attribute) -> {"Attribute": ""}
    - HSTRUCT(A=B) -> {"A": "B"}
    - HSTRUCT(A="B") -> {"A": "B"}
    - HSTRUCT(A="B C") -> {"A": "B C"}
    """
    attributes = {}
    
    if not attr_str or not attr_str.strip():
        return attributes
    
    # 移除首尾空格
    attr_str = attr_str.strip()
    
    # 处理多个属性，用逗号分隔
    # 但要注意引号内的逗号不应该分割
    parts = []
    current = ""
    in_quotes = False
    quote_char = None
    
    for char in attr_str:
        if char in ('"', "'") and (not current or current[-1] != '\\'):
            if not in_quotes:
                in_quotes = True
                quote_char = char
            elif char == quote_char:
                in_quotes = False
                quote_char = None
            current += char
        elif char == ',' and not in_quotes:
            if current.strip():
                parts.append(current.strip())
            current = ""
        else:
            current += char
    
    if current.strip():
        parts.append(current.strip())
    
    # 解析每个部分
    for part in parts:
        part = part.strip()
        if not part:
            continue
        
        # 检查是否有 = 号
        if '=' in part:
            # 分割 Key 和 Value
            eq_pos = part.find('=')
            key = part[:eq_pos].strip()
            value = part[eq_pos + 1:].strip()
            
            # 移除引号
            if value.startswith('"') and value.endswith('"'):
                value = value[1:-1]
            elif value.startswith("'") and value.endswith("'"):
                value = value[1:-1]
            
            # 处理转义字符
            value = value.replace('\\"', '"').replace("\\'", "'")
            
            attributes[key] = value
        else:
            # 没有 = 号，只有属性名
            attributes[part] = ""
    
    return attributes


def extract_all_struct_info(file_path: str) -> List[StructInfo]:
    """
    从文件中提取所有结构体信息
    返回 StructInfo 列表
    """
    structs = []
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"错误：无法读取文件 {file_path}: {e}")
        return structs
    
    lines = content.split('\n')
    
    # 查找 HSTRUCT 和 HCLASS 宏
    hstruct_pattern = re.compile(r'HSTRUCT\s*\(([^)]*)\)')
    hclass_pattern = re.compile(r'HCLASS\s*\(([^)]*)\)')
    # 匹配 struct/class 名称，可能包含 HK_API 和继承：
    # struct Name : BaseClass 或 class Name : public BaseClass
    # struct HK_API Name 或 class HK_API Name : BaseClass
    # 只匹配第一个父类
    struct_pattern = re.compile(r'(struct|class)\s+(HK_API\s+)?(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?')
    property_pattern = re.compile(r'HPROPERTY\s*\(([^)]*)\)')
    
    i = 0
    while i < len(lines):
        line = lines[i]
        i += 1
        
        # 查找 HSTRUCT 或 HCLASS
        hstruct_match = hstruct_pattern.search(line)
        hclass_match = hclass_pattern.search(line)
        
        if hstruct_match or hclass_match:
            if hstruct_match:
                attr_str = hstruct_match.group(1)
                pending_macro_type = "HSTRUCT"
            else:
                attr_str = hclass_match.group(1)
                pending_macro_type = "HCLASS"
            
            attributes = parse_hstruct_attributes(attr_str)
            
            # 查找对应的 struct/class 定义
            current_struct_name = None
            current_struct_type = None
            current_base_class = None
            struct_start_line = i - 1
            
            # 在接下来的几行中查找 struct/class 定义
            current_has_hk_api = False
            for j in range(i, min(i + 10, len(lines))):
                struct_match = struct_pattern.search(lines[j])
                if struct_match:
                    current_struct_type = struct_match.group(1)
                    has_hk_api_match = struct_match.group(2)  # HK_API 部分
                    current_struct_name = struct_match.group(3)
                    if struct_match.group(4):
                        current_base_class = struct_match.group(4).strip()
                    else:
                        current_base_class = None
                    current_has_hk_api = has_hk_api_match is not None and "HK_API" in has_hk_api_match
                    break
            
            if current_struct_name:
                # 解析结构体内容
                in_struct = False
                brace_count = 0
                properties = []
                found_hproperty = False
                hproperty_line_idx = -1
                pending_hproperty_attributes = None
                
                # 从找到struct/class定义的行开始解析
                start_j = j if current_struct_name else i
                for j in range(start_j, len(lines)):
                    line_content = lines[j]
                    
                    if not in_struct and '{' in line_content:
                        in_struct = True
                        brace_count = 0
                    
                    if in_struct:
                        brace_count += line_content.count('{')
                        brace_count -= line_content.count('}')
                        
                        # 查找 HPROPERTY
                        prop_match = property_pattern.search(line_content)
                        if prop_match:
                            found_hproperty = True
                            hproperty_line_idx = j + 1
                            attr_str = prop_match.group(1)
                            pending_hproperty_attributes = parse_hstruct_attributes(attr_str)
                            
                            # 检查当前行是否同时包含属性定义
                            type_name_pattern = re.compile(r'(\w+(?:\s*\*\s*)?)\s+(\w+)\s*[;={]')
                            type_name_match = type_name_pattern.search(line_content)
                            if type_name_match:
                                prop_type = type_name_match.group(1).strip()
                                prop_name = type_name_match.group(2).strip()
                                prop_type = re.sub(r'\s+', ' ', prop_type)
                                properties.append(PropertyInfo(
                                    name=prop_name,
                                    type=prop_type,
                                    line=j + 1,
                                    attributes=pending_hproperty_attributes or {}
                                ))
                                found_hproperty = False
                                hproperty_line_idx = -1
                                pending_hproperty_attributes = None
                        
                        # 如果之前找到了 HPROPERTY，检查下一行是否有属性定义
                        if found_hproperty and hproperty_line_idx > 0 and j + 1 > hproperty_line_idx:
                            type_name_pattern = re.compile(r'(\w+(?:\s*\*\s*)?)\s+(\w+)\s*[;={]')
                            type_name_match = type_name_pattern.search(line_content)
                            if type_name_match:
                                prop_type = type_name_match.group(1).strip()
                                prop_name = type_name_match.group(2).strip()
                                prop_type = re.sub(r'\s+', ' ', prop_type)
                                properties.append(PropertyInfo(
                                    name=prop_name,
                                    type=prop_type,
                                    line=j + 1,
                                    attributes=pending_hproperty_attributes or {}
                                ))
                                found_hproperty = False
                                hproperty_line_idx = -1
                                pending_hproperty_attributes = None
                        
                        # 结构体结束
                        if brace_count == 0 and in_struct:
                            structs.append(StructInfo(
                                name=current_struct_name,
                                properties=properties,
                                attributes=attributes,
                                line=struct_start_line + 1,
                                file_path=file_path,
                                type=current_struct_type,
                                base_class=current_base_class,
                                macro_type=pending_macro_type,
                                has_hk_api=current_has_hk_api
                            ))
                            i = j + 1
                            break
    
    return structs


def extract_struct_info(file_path: str) -> Optional[StructInfo]:
    """
    从文件中提取结构体信息
    返回 StructInfo 或 None（如果没有找到 HSTRUCT）
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"错误：无法读取文件 {file_path}: {e}")
        return None
    
    lines = content.split('\n')
    
    # 查找 HSTRUCT 和 HCLASS 宏
    hstruct_pattern = re.compile(r'HSTRUCT\s*\(([^)]*)\)')
    hclass_pattern = re.compile(r'HCLASS\s*\(([^)]*)\)')
    # 匹配 struct/class 名称，可能包含 HK_API 和继承：
    # struct Name : BaseClass 或 class Name : public BaseClass
    # struct HK_API Name 或 class HK_API Name : BaseClass
    # 只匹配第一个父类
    struct_pattern = re.compile(r'(struct|class)\s+(HK_API\s+)?(\w+)(?:\s*:\s*(?:public|private|protected)?\s*(\w+))?')
    property_pattern = re.compile(r'HPROPERTY\s*\(([^)]*)\)')
    
    struct_info = None
    current_struct_name = None
    current_struct_type = None  # "struct" 或 "class"
    current_base_class = None
    current_has_hk_api = False
    in_struct = False
    brace_count = 0
    properties = []
    found_hproperty = False
    hproperty_line_idx = -1
    pending_hproperty_attributes = None
    pending_hstruct = None
    pending_attributes = None
    pending_macro_type = None  # "HSTRUCT" 或 "HCLASS"
    
    for i, line in enumerate(lines, 1):
        # 查找 HSTRUCT 或 HCLASS
        hstruct_match = hstruct_pattern.search(line)
        hclass_match = hclass_pattern.search(line)
        if (hstruct_match or hclass_match) and not in_struct and pending_hstruct is None:
            if hstruct_match:
                attr_str = hstruct_match.group(1)
                pending_macro_type = "HSTRUCT"
            else:
                attr_str = hclass_match.group(1)
                pending_macro_type = "HCLASS"
            attributes = parse_hstruct_attributes(attr_str)
            pending_hstruct = i
            pending_attributes = attributes
        
        # 如果有待处理的 HSTRUCT/HCLASS，查找 struct/class 定义
        if pending_hstruct is not None and not in_struct:
            struct_match = struct_pattern.search(line)
            if struct_match:
                current_struct_type = struct_match.group(1)  # "struct" 或 "class"
                has_hk_api_match = struct_match.group(2)  # HK_API 部分
                current_struct_name = struct_match.group(3)
                # 检查是否有父类（第4个分组）
                if struct_match.group(4):
                    current_base_class = struct_match.group(4).strip()
                else:
                    current_base_class = None
                current_has_hk_api = has_hk_api_match is not None and "HK_API" in has_hk_api_match
                # 检查当前行或下一行是否有开括号
                if '{' in line:
                    in_struct = True
                    brace_count = 0
                    properties = []
                    found_hproperty = False
                    hproperty_line_idx = -1
                    pending_hproperty_attributes = None
                    struct_info = StructInfo(
                        name=current_struct_name,
                        properties=[],
                        attributes=pending_attributes,
                        line=pending_hstruct,
                        file_path=file_path,
                        type=current_struct_type,
                        base_class=current_base_class,
                        macro_type=pending_macro_type or "HSTRUCT",
                        has_hk_api=current_has_hk_api
                    )
                    pending_hstruct = None
                    pending_attributes = None
                    pending_macro_type = None
                elif i < len(lines) and '{' in lines[i]:
                    # 下一行有开括号，但我们需要处理当前行
                    pass
        
        # 检查是否应该开始结构体解析（开括号行）
        if pending_hstruct is not None and current_struct_name and '{' in line and not in_struct:
            in_struct = True
            brace_count = 0
            properties = []
            found_hproperty = False
            hproperty_line_idx = -1
            pending_hproperty_attributes = None
            struct_info = StructInfo(
                name=current_struct_name,
                properties=[],
                attributes=pending_attributes,
                line=pending_hstruct,
                file_path=file_path,
                type=current_struct_type,
                base_class=current_base_class,
                macro_type=pending_macro_type or "HSTRUCT",
                has_hk_api=current_has_hk_api
            )
            pending_hstruct = None
            pending_attributes = None
            pending_macro_type = None
        
        if in_struct:
            # 计算大括号
            brace_count += line.count('{')
            brace_count -= line.count('}')
            
            # 查找 HPROPERTY
            prop_match = property_pattern.search(line)
            if prop_match:
                found_hproperty = True
                hproperty_line_idx = i
                # 解析 HPROPERTY 的属性
                attr_str = prop_match.group(1)
                pending_hproperty_attributes = parse_hstruct_attributes(attr_str)
                # 检查当前行是否同时包含属性定义
                # 匹配格式：Type Name; 或 Type Name{}; 或 Type Name = value;
                type_name_pattern = re.compile(r'(\w+(?:\s*\*\s*)?)\s+(\w+)\s*[;={]')
                type_name_match = type_name_pattern.search(line)
                if type_name_match:
                    # 当前行同时有 HPROPERTY 和属性定义
                    prop_type = type_name_match.group(1).strip()
                    prop_name = type_name_match.group(2).strip()
                    prop_type = re.sub(r'\s+', ' ', prop_type)
                    properties.append(PropertyInfo(
                        name=prop_name,
                        type=prop_type,
                        line=i,
                        attributes=pending_hproperty_attributes or {}
                    ))
                    found_hproperty = False
                    hproperty_line_idx = -1
                    pending_hproperty_attributes = None
            
            # 如果之前找到了 HPROPERTY，检查当前行是否有属性定义
            if found_hproperty and hproperty_line_idx > 0 and i > hproperty_line_idx:
                # 匹配格式：Type Name; 或 Type Name{}; 或 Type Name = value;
                type_name_pattern = re.compile(r'(\w+(?:\s*\*\s*)?)\s+(\w+)\s*[;={]')
                type_name_match = type_name_pattern.search(line)
                if type_name_match:
                    prop_type = type_name_match.group(1).strip()
                    prop_name = type_name_match.group(2).strip()
                    prop_type = re.sub(r'\s+', ' ', prop_type)
                    properties.append(PropertyInfo(
                        name=prop_name,
                        type=prop_type,
                        line=i,
                        attributes=pending_hproperty_attributes or {}
                    ))
                    found_hproperty = False
                    hproperty_line_idx = -1
                    pending_hproperty_attributes = None
            
            # 结构体结束
            if brace_count == 0 and in_struct:
                if struct_info:
                    struct_info.properties = properties
                in_struct = False
                break
    
    return struct_info


def extract_enum_info(file_path: str) -> Optional[EnumInfo]:
    """
    从文件中提取枚举信息
    返回 EnumInfo 或 None（如果没有找到 HENUM）
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
    except Exception as e:
        print(f"错误：无法读取文件 {file_path}: {e}")
        return None
    
    lines = content.split('\n')
    
    # 查找 HENUM 宏
    henums_pattern = re.compile(r'HENUM\s*\(([^)]*)\)')
    enum_pattern = re.compile(r'enum\s+(?:class\s+)?(\w+)')
    
    enum_info = None
    current_enum_name = None
    in_enum = False
    brace_count = 0
    members = []
    pending_henum = None
    pending_attributes = None
    
    for i, line in enumerate(lines, 1):
        # 查找 HENUM
        henums_match = henums_pattern.search(line)
        if henums_match and not in_enum and pending_henum is None:
            attr_str = henums_match.group(1)
            attributes = parse_henum_attributes(attr_str)
            pending_henum = i
            pending_attributes = attributes
        
        # 如果有待处理的 HENUM，查找 enum 定义
        if pending_henum is not None and not in_enum:
            enum_match = enum_pattern.search(line)
            if enum_match:
                current_enum_name = enum_match.group(1)
                # 检查当前行或下一行是否有开括号
                if '{' in line:
                    in_enum = True
                    brace_count = 0
                    members = []
                    enum_info = EnumInfo(
                        name=current_enum_name,
                        members=[],
                        attributes=pending_attributes,
                        line=pending_henum,
                        file_path=file_path
                    )
                    pending_henum = None
                    pending_attributes = None
                elif i < len(lines) and '{' in lines[i]:
                    # 下一行有开括号
                    pass
        
        # 检查是否应该开始枚举解析（开括号行）
        if pending_henum is not None and current_enum_name and '{' in line and not in_enum:
            in_enum = True
            brace_count = 0
            members = []
            enum_info = EnumInfo(
                name=current_enum_name,
                members=[],
                attributes=pending_attributes,
                line=pending_henum,
                file_path=file_path
            )
            pending_henum = None
            pending_attributes = None
        
        if in_enum:
            # 计算大括号
            brace_count += line.count('{')
            brace_count -= line.count('}')
            
            # 查找枚举成员
            # 先移除注释
            line_no_comment = line.split('//')[0] if '//' in line else line
            
            # 匹配格式：MemberName 或 MemberName = Value
            # 需要更精确的匹配，避免匹配到其他内容
            member_pattern = re.compile(r'\b(\w+)(?:\s*=\s*([^,}\s]+))?\s*(?:[,}]|$)')
            member_matches = member_pattern.finditer(line_no_comment)
            
            for match in member_matches:
                member_name = match.group(1).strip()
                member_value = match.group(2).strip() if match.group(2) else None
                
                # 跳过空名称
                if not member_name:
                    continue
                
                # 移除可能的尾随空格
                if member_value:
                    member_value = member_value.strip()
                
                members.append(EnumMemberInfo(
                    name=member_name,
                    value=member_value,
                    line=i
                ))
            
            # 枚举结束
            if brace_count == 0 and in_enum:
                if enum_info:
                    enum_info.members = members
                in_enum = False
                break
    
    return enum_info


def generate_enum_header_code(enum_info: EnumInfo) -> str:
    """
    生成枚举的 .generated.h 代码（全局函数，不在宏中）
    """
    enum_name = enum_info.name
    register_func_name = f"Z_Register_{enum_name}"
    registerer_struct_name = f"F_Z_Register_{enum_name}"
    registerer_var_name = f"Z_REGISTERER_{enum_name.upper()}"
    
    code = f"""HK_API void {register_func_name}();
struct {registerer_struct_name}
{{
    {registerer_struct_name}()
    {{
        {register_func_name}();
    }}
}};
static inline {registerer_struct_name} {registerer_var_name};
"""
    return code


def generate_enum_cpp_code(enum_info: EnumInfo, header_path: str, engine_dir: str) -> str:
    """
    生成枚举的 .generated.cpp 代码
    """
    enum_name = enum_info.name
    type_name = remove_prefix(enum_name)
    register_func_name = f"Z_Register_{enum_name}"
    register_impl_func_name = f"Z_Register_{enum_name}_Impl"
    
    # 生成实现函数（执行实际的注册操作）
    impl_content = f"""static void {register_impl_func_name}()
{{
    // 注册枚举类型
    FTypeMutable Type = FTypeManager::Register<{enum_name}>("{type_name}");

"""
    
    # 注册枚举成员
    for member in enum_info.members:
        enum_value_expr = f"{enum_name}::{member.name}"
        impl_content += f"""    // 注册枚举成员: {member.name}
    Type->RegisterEnumMember({enum_value_expr}, "{member.name}");

"""
    
    # 注册枚举属性
    for key, value in enum_info.attributes.items():
        if value:
            impl_content += f"""    // 注册枚举属性: {key} = {value}
    Type->RegisterAttribute(FName("{key}"), FName("{value}"));

"""
        else:
            impl_content += f"""    // 注册枚举属性: {key}
    Type->RegisterAttribute(FName("{key}"), FName());

"""
    
    impl_content += "}\n\n"
    
    # 生成注册函数（只注册函数，不执行注册操作）
    code = f"""{impl_content}void {register_func_name}()
{{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<{enum_name}>({register_impl_func_name});
}}
"""
    return code


def generate_header_file(struct_info: StructInfo, output_dir: str, struct_cache: Dict[str, str] = None) -> str:
    """
    生成 .generated.h 文件内容
    只包含一个宏定义，不包含任何头文件
    struct_cache: {类名: 宏类型} 的字典，用于检查父类是否被标注
    """
    if struct_cache is None:
        struct_cache = {}
    
    struct_name = struct_info.name
    macro_name = f"GENERATED_HEADER_{struct_name}"
    
    # 计算去掉前缀后的类型名
    type_name = remove_prefix(struct_name)
    
    # 生成注册器名称（使用 Z_ 前缀，基于去掉前缀后的类型名）
    registerer_struct_name = f"Z_{type_name}_Register"
    registerer_var_name = f"Z_REGISTERER_{type_name.upper()}"
    register_func_name = f"Register_{struct_name}"
    
    # 检查是否有 CustomReadWrite 属性
    has_custom_read_write = "CustomReadWrite" in struct_info.attributes
    
    # 生成序列化代码
    serialize_code = ""
    if not has_custom_read_write:
        # 过滤掉有 Transient 属性的属性
        serializable_properties = [
            prop for prop in struct_info.properties
            if "Transient" not in prop.attributes
        ]
        
        if serializable_properties:
            serialize_pairs = []
            for i, prop in enumerate(serializable_properties):
                # 最后一个不需要逗号
                if i < len(serializable_properties) - 1:
                    serialize_pairs.append(f'            MakeNamedPair("{prop.name}", {prop.name}),                                                                                     \\')
                else:
                    serialize_pairs.append(f'            MakeNamedPair("{prop.name}", {prop.name})                                                                                      \\')
            
            # 检查是否有被标注的父类
            has_annotated_parent = struct_info.base_class is not None and struct_info.base_class in struct_cache
            
            if struct_info.macro_type == "HCLASS":
                # class: 使用宏声明六个Serialize函数
                serialize_code = f"    HK_DECL_CLASS_SERIALIZATION({struct_name})                                                                                        \\\n"
            else:
                # struct: 生成模板函数
                super_call = ""
                if has_annotated_parent:
                    super_call = "        Super::Serialize(Ar);                                                                                        \\\n"
                
                serialize_code = f"""    template <typename Archive>                                                                                        \\
    void Serialize(Archive& Ar)                                                                                        \\
    {{                                                                                                                  \\
{super_call}        Ar(                                                                                                            \\
{chr(10).join(serialize_pairs)}
        );                                                                                                             \\
    }}                                                                                                                  \\
"""
    
    # 生成 typedef 代码
    typedef_code = ""
    # 根据宏类型决定使用 ThisStruct 还是 ThisClass
    if struct_info.macro_type == "HCLASS":
        # HCLASS 使用 ThisClass
        if struct_info.base_class is None:
            # 没有父类
            typedef_code = f"    typedef {struct_name} ThisClass;                                                                                        \\\n"
        else:
            # 有父类，检查父类是否被标注
            if struct_info.base_class in struct_cache:
                # 父类被标注，生成 Super 和 ThisClass
                # 需要检查父类的宏类型来决定使用 ThisStruct 还是 ThisClass
                parent_macro_type = struct_cache.get(struct_info.base_class, "")
                if parent_macro_type == "HCLASS":
                    typedef_code = f"    typedef {struct_info.base_class}::ThisClass Super;                                                                                        \\\n"
                else:
                    typedef_code = f"    typedef {struct_info.base_class}::ThisStruct Super;                                                                                        \\\n"
                typedef_code += f"    typedef {struct_name} ThisClass;                                                                                        \\\n"
    else:
        # HSTRUCT 使用 ThisStruct
        if struct_info.base_class is None:
            # 没有父类
            typedef_code = f"    typedef {struct_name} ThisStruct;                                                                                        \\\n"
        else:
            # 有父类，检查父类是否被标注
            if struct_info.base_class in struct_cache:
                # 父类被标注，生成 Super 和 ThisStruct
                # 需要检查父类的宏类型来决定使用 ThisStruct 还是 ThisClass
                parent_macro_type = struct_cache.get(struct_info.base_class, "")
                if parent_macro_type == "HCLASS":
                    typedef_code = f"    typedef {struct_info.base_class}::ThisClass Super;                                                                                        \\\n"
                else:
                    typedef_code = f"    typedef {struct_info.base_class}::ThisStruct Super;                                                                                        \\\n"
                typedef_code += f"    typedef {struct_name} ThisStruct;                                                                                        \\\n"
    
    # 如果结构体有 HK_API，则在注册函数中添加 HK_API
    register_func_decl = f"static void {register_func_name}();"
    if struct_info.has_hk_api:
        register_func_decl = f"static HK_API void {register_func_name}();"
    
    header_content = f"""#pragma once

#define {macro_name}                                                                                        \\
    struct {registerer_struct_name}                                                                                            \\
    {{                                                                                                                  \\
        {registerer_struct_name}()                                                                                             \\
        {{                                                                                                              \\
            {register_func_name}();                                                                                         \\
        }}                                                                                                              \\
        {register_func_decl}                                                                                 \\
    }};                                                                                                                 \\
{typedef_code}{serialize_code}    static inline {registerer_struct_name} {registerer_var_name};
"""
    
    return header_content


def remove_prefix(name: str) -> str:
    """
    移除类型名前缀（例如 FColor -> Color）
    如果以 F 开头且第二个字母是大写，则去掉 F 前缀
    """
    if len(name) > 1 and name[0] == 'F' and name[1].isupper():
        return name[1:]
    return name


def generate_cpp_file(struct_info: StructInfo, header_path: str, output_dir: str, engine_dir: str, struct_cache: Dict[str, str] = None) -> str:
    """
    生成 .generated.cpp 文件内容
    struct_cache: {类名: 宏类型} 的字典，用于检查父类是否被标注
    """
    if struct_cache is None:
        struct_cache = {}
    
    struct_name = struct_info.name
    type_name = remove_prefix(struct_name)
    registerer_struct_name = f"Z_{type_name}_Register"
    register_func_name = f"Register_{struct_name}"
    register_impl_func_name = f"Register_{struct_name}_Impl"
    
    # 计算注册类型名（去掉前缀）
    type_name = remove_prefix(struct_name)
    
    # 计算 .generated.h 的相对路径（在同一目录）
    file_stem = Path(header_path).stem
    generated_h_name = f"{file_stem}.generated.h"
    
    # 计算源文件的相对路径（从 Engine 目录开始）
    rel_path = os.path.relpath(header_path, engine_dir).replace('\\', '/')
    # 移除 .h 扩展名
    if rel_path.endswith('.h'):
        rel_path = rel_path[:-2]
    elif rel_path.endswith('.hpp'):
        rel_path = rel_path[:-4]
    
    # 生成实现函数（执行实际的注册操作）
    impl_content = f"""static void {register_impl_func_name}()
{{
    // 注册类型
    FTypeMutable Type = FTypeManager::Register<{struct_name}>("{type_name}");

"""
    
    # 注册父类（如果有被标注的父类）
    if struct_info.base_class is not None and struct_info.base_class in struct_cache:
        impl_content += f"""    // 注册父类: {struct_info.base_class}
    Type->RegisterParent(FTypeManager::TypeOf<{struct_info.base_class}>());

"""
    
    # 注册属性
    for prop in struct_info.properties:
        impl_content += f"""    // 注册属性: {prop.name}
    Type->RegisterProperty(&{struct_name}::{prop.name}, "{prop.name}");

"""
    
    # 注册结构体属性
    for key, value in struct_info.attributes.items():
        if value:
            impl_content += f"""    // 注册类型属性: {key} = {value}
    Type->RegisterAttribute(FName("{key}"), FName("{value}"));

"""
        else:
            impl_content += f"""    // 注册类型属性: {key}
    Type->RegisterAttribute(FName("{key}"), FName());

"""
    
    impl_content += "}\n\n"
    
    # 生成注册函数（只注册函数，不执行注册操作）
    # 如果结构体有 HK_API，则在函数定义中添加 HK_API
    register_func_prefix = ""
    if struct_info.has_hk_api:
        register_func_prefix = "HK_API "
    
    register_func_code = f"""{register_func_prefix}void {struct_name}::{registerer_struct_name}::{register_func_name}()
{{
    // 只注册类型注册器函数，不执行注册操作
    FTypeManager::RegisterTypeRegisterer<{struct_name}>({register_impl_func_name});
}}

"""
    
    # 生成序列化代码（如果是class且有属性需要序列化）
    serialize_impl_code = ""
    if struct_info.macro_type == "HCLASS":
        # 检查是否有 CustomReadWrite 属性
        has_custom_read_write = "CustomReadWrite" in struct_info.attributes
        
        if not has_custom_read_write:
            # 过滤掉有 Transient 属性的属性
            serializable_properties = [
                prop for prop in struct_info.properties
                if "Transient" not in prop.attributes
            ]
            
            if serializable_properties:
                serialize_pairs = []
                for i, prop in enumerate(serializable_properties):
                    # 最后一个不需要逗号
                    if i < len(serializable_properties) - 1:
                        serialize_pairs.append(f'        MakeNamedPair("{prop.name}", {prop.name}),')
                    else:
                        serialize_pairs.append(f'        MakeNamedPair("{prop.name}", {prop.name})')
                
                # 检查是否有被标注的父类
                has_annotated_parent = struct_info.base_class is not None and struct_info.base_class in struct_cache
                
                # 生成序列化代码宏定义（每行末尾都需要反斜杠，因为会被替换到另一个宏中）
                serialization_code_lines = []
                if has_annotated_parent:
                    serialization_code_lines.append("        Super::Serialize(Ar);")
                serialization_code_lines.append("        Ar(")
                # 添加序列化对（不在这里加反斜杠，后面统一加）
                serialization_code_lines.extend(serialize_pairs)
                serialization_code_lines.append("        );")
                
                # 将每行用反斜杠连接（所有行都需要反斜杠，因为会被替换到另一个宏中）
                serialization_code_content = ""
                for line in serialization_code_lines:
                    serialization_code_content += f"{line} \\\n"
                
                # 定义 {ClassName}_SERIALIZATION_CODE 宏
                serialize_impl_code = f"""#define {struct_name}_SERIALIZATION_CODE \\
{serialization_code_content}

"""
                # 使用HK_DEFINE_CLASS_SERIALIZATION宏
                serialize_impl_code += f"HK_DEFINE_CLASS_SERIALIZATION({struct_name})\n\n"
                # 取消定义宏
                serialize_impl_code += f"#undef {struct_name}_SERIALIZATION_CODE\n\n"
    
    # 生成完整的cpp内容
    cpp_content = f"""#include "{generated_h_name}"
#include "Core/Reflection/TypeManager.h"
#include "{rel_path}.h"


{impl_content}{register_func_code}{serialize_impl_code}"""
    
    return cpp_content


def get_file_hash(file_path: str) -> str:
    """计算文件内容的哈希值"""
    try:
        with open(file_path, 'rb') as f:
            return hashlib.md5(f.read()).hexdigest()
    except:
        return ""


def load_cache(cache_file: str) -> Dict[str, str]:
    """
    加载缓存文件
    返回 {文件路径: 哈希值} 的字典
    """
    cache = {}
    if os.path.exists(cache_file):
        try:
            with open(cache_file, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    if ':' in line:
                        file_path, file_hash = line.split(':', 1)
                        cache[file_path] = file_hash
        except:
            pass
    return cache


def load_struct_cache(cache_file: str) -> Dict[str, str]:
    """
    加载结构体/类缓存文件
    返回 {类名: 宏类型} 的字典，宏类型为 "HSTRUCT" 或 "HCLASS"
    """
    cache = {}
    if os.path.exists(cache_file):
        try:
            with open(cache_file, 'r', encoding='utf-8') as f:
                for line in f:
                    line = line.strip()
                    if ':' in line:
                        class_name, macro_type = line.split(':', 1)
                        cache[class_name] = macro_type
        except:
            pass
    return cache


def save_struct_cache(cache_file: str, cache: Dict[str, str]):
    """保存结构体/类缓存文件"""
    try:
        os.makedirs(os.path.dirname(cache_file), exist_ok=True)
        with open(cache_file, 'w', encoding='utf-8') as f:
            for class_name, macro_type in sorted(cache.items()):
                f.write(f"{class_name}:{macro_type}\n")
    except:
        pass


def save_cache(cache_file: str, cache: Dict[str, str]):
    """保存缓存文件"""
    try:
        os.makedirs(os.path.dirname(cache_file), exist_ok=True)
        with open(cache_file, 'w', encoding='utf-8') as f:
            for file_path, file_hash in sorted(cache.items()):
                f.write(f"{file_path}:{file_hash}\n")
    except:
        pass


def should_regenerate(source_file: str, generated_h: str, generated_cpp: str, cache: Dict[str, str]) -> bool:
    """
    检查是否需要重新生成
    返回 True 如果需要重新生成
    """
    # 如果生成的文件不存在，需要生成
    if not os.path.exists(generated_h) or not os.path.exists(generated_cpp):
        return True
    
    # 检查缓存
    source_hash = get_file_hash(source_file)
    cached_hash = cache.get(source_file, "")
    
    if cached_hash == source_hash:
        # 检查生成的文件是否比源文件新
        try:
            source_mtime = os.path.getmtime(source_file)
            gen_h_mtime = os.path.getmtime(generated_h)
            gen_cpp_mtime = os.path.getmtime(generated_cpp)
            
            if gen_h_mtime >= source_mtime and gen_cpp_mtime >= source_mtime:
                return False
        except:
            pass
    
    return True


def process_file(file_path: str, engine_dir: str, generated_dir: str, cache: Dict[str, str], struct_cache: Dict[str, str]) -> Tuple[bool, str, Optional[str]]:
    """
    处理单个文件
    返回 (成功, 消息, 文件哈希值)
    """
    try:
        file_path = os.path.abspath(file_path)
        rel_path = os.path.relpath(file_path, engine_dir)
        
        # 提取结构体和枚举信息
        struct_infos = extract_all_struct_info(file_path)
        enum_info = extract_enum_info(file_path)
        
        if not struct_infos and not enum_info:
            return (True, f"跳过 {rel_path}：未找到 HSTRUCT/HCLASS 或 HENUM 宏", None, None)
        
        # 确定输出路径
        file_stem = Path(file_path).stem
        generated_h = os.path.join(generated_dir, f"{file_stem}.generated.h")
        generated_cpp = os.path.join(generated_dir, f"{file_stem}.generated.cpp")
        
        # 检查是否需要重新生成
        if not should_regenerate(file_path, generated_h, generated_cpp, cache):
            return (True, f"跳过 {rel_path}：文件未更改", None, None)
        
        # 生成头文件内容
        header_parts = []
        header_parts.append("#pragma once\n")
        header_parts.append('#include "Core/Utility/Macros.h"\n')
        
        # 生成所有结构体宏定义
        for struct_info in struct_infos:
            struct_macro = generate_header_file(struct_info, generated_dir, struct_cache)
            # 移除 #pragma once（已经在上面添加了）
            struct_macro = struct_macro.replace("#pragma once\n\n", "")
            header_parts.append(struct_macro)
        
        # 生成枚举全局代码（不在宏中）
        if enum_info:
            enum_code = generate_enum_header_code(enum_info)
            header_parts.append(enum_code)
        
        header_content = "\n".join(header_parts)
        if not header_content.endswith("\n"):
            header_content += "\n"
        
        # 生成 cpp 文件内容
        cpp_parts = []
        
        # 计算 .generated.h 的相对路径（在同一目录）
        generated_h_name = f"{file_stem}.generated.h"
        
        # 计算源文件的相对路径（从 Engine 目录开始）
        rel_path_cpp = os.path.relpath(file_path, engine_dir).replace('\\', '/')
        # 移除 .h 扩展名
        if rel_path_cpp.endswith('.h'):
            rel_path_cpp = rel_path_cpp[:-2]
        elif rel_path_cpp.endswith('.hpp'):
            rel_path_cpp = rel_path_cpp[:-4]
        
        cpp_parts.append(f'#include "{generated_h_name}"')
        cpp_parts.append('#include "Core/Reflection/TypeManager.h"')
        cpp_parts.append(f'#include "{rel_path_cpp}.h"')
        cpp_parts.append("")
        cpp_parts.append("")
        
        # 生成所有结构体代码
        for struct_info in struct_infos:
            struct_cpp = generate_cpp_file(struct_info, file_path, generated_dir, engine_dir, struct_cache)
            # 移除 include 部分（已经在上面添加了）
            struct_cpp_lines = struct_cpp.split('\n')
            # 跳过前几行的 include
            skip_lines = 0
            for line in struct_cpp_lines:
                if line.strip().startswith('#include') or line.strip() == "":
                    skip_lines += 1
                else:
                    break
            struct_cpp = '\n'.join(struct_cpp_lines[skip_lines:])
            cpp_parts.append(struct_cpp)
        
        # 生成枚举代码
        if enum_info:
            enum_cpp = generate_enum_cpp_code(enum_info, file_path, engine_dir)
            cpp_parts.append(enum_cpp)
        
        cpp_content = "\n".join(cpp_parts)
        if not cpp_content.endswith("\n"):
            cpp_content += "\n"
        
        # 确保目录存在
        os.makedirs(generated_dir, exist_ok=True)
        
        # 写入文件
        with open(generated_h, 'w', encoding='utf-8') as f:
            f.write(header_content)
        
        with open(generated_cpp, 'w', encoding='utf-8') as f:
            f.write(cpp_content)
        
        # 计算并返回文件哈希
        source_hash = get_file_hash(file_path)
        
        parts = []
        if struct_infos:
            parts.append(f"{len(struct_infos)}struct")
        if enum_info:
            parts.append("enum")
        type_str = "+".join(parts) if parts else "unknown"
        
        # 返回所有结构体信息用于更新缓存
        struct_cache_infos = []
        for struct_info in struct_infos:
            struct_cache_infos.append((struct_info.name, struct_info.macro_type))
        
        return (True, f"生成 {rel_path} -> {file_stem}.generated.h/cpp ({type_str})", source_hash, struct_cache_infos)
    
    except Exception as e:
        return (False, f"处理 {file_path} 时出错: {e}", None, None)


def find_header_files(engine_dir: str) -> List[str]:
    """查找所有包含 HSTRUCT、HCLASS 或 HENUM 的头文件"""
    header_files = []
    
    for root, dirs, files in os.walk(engine_dir):
        # 跳过 Generated 目录
        if 'Generated' in root:
            continue
        
        for file in files:
            if file.endswith(('.h', '.hpp')):
                file_path = os.path.join(root, file)
                try:
                    with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                        content = f.read()
                        if 'HSTRUCT' in content or 'HCLASS' in content or 'HENUM' in content:
                            header_files.append(file_path)
                except:
                    pass
    
    return header_files


def find_source_file_for_generated(generated_file: str, engine_dir: str) -> Optional[str]:
    """
    根据生成文件名查找对应的源文件
    返回源文件路径，如果不存在则返回None
    
    注意：只要源文件存在就返回，不检查是否包含宏（因为文件可能被修改移除了宏）
    """
    # 从生成文件名中提取基础名称（去掉.generated.h或.generated.cpp）
    file_name = os.path.basename(generated_file)
    if file_name.endswith('.generated.h'):
        base_name = file_name.replace('.generated.h', '')
    elif file_name.endswith('.generated.cpp'):
        base_name = file_name.replace('.generated.cpp', '')
    else:
        return None
    
    # 在Engine目录中搜索对应的源文件（.h或.hpp）
    for root, dirs, files in os.walk(engine_dir):
        # 跳过Generated目录
        if 'Generated' in root:
            continue
        
        for ext in ['.h', '.hpp']:
            source_file = os.path.join(root, f"{base_name}{ext}")
            if os.path.exists(source_file):
                # 只要源文件存在就返回（不检查宏，因为文件可能被修改）
                return source_file
    
    return None


def cleanup_deleted_files(cache: Dict[str, str], struct_cache: Dict[str, str], 
                          engine_dir: str, generated_dir: str) -> int:
    """
    清理已删除源文件对应的生成文件
    返回删除的文件数量
    
    注意：struct_cache参数保留用于未来可能的扩展，当前不清理struct_cache
    因为结构体可能在其他文件中被引用
    """
    deleted_count = 0
    files_to_remove = []
    processed_generated_files = set()  # 记录已处理的生成文件，避免重复处理
    
    # 第一步：检查缓存中的每个文件是否仍然存在
    for file_path in list(cache.keys()):
        if not os.path.exists(file_path):
            # 源文件已删除，需要删除对应的生成文件
            file_stem = Path(file_path).stem
            generated_h = os.path.join(generated_dir, f"{file_stem}.generated.h")
            generated_cpp = os.path.join(generated_dir, f"{file_stem}.generated.cpp")
            
            # 删除生成文件
            deleted_any = False
            if os.path.exists(generated_h):
                try:
                    os.remove(generated_h)
                    deleted_any = True
                    processed_generated_files.add(generated_h)
                except Exception as e:
                    print(f"[WARN] 无法删除 {generated_h}: {e}")
            
            if os.path.exists(generated_cpp):
                try:
                    os.remove(generated_cpp)
                    deleted_any = True
                    processed_generated_files.add(generated_cpp)
                except Exception as e:
                    print(f"[WARN] 无法删除 {generated_cpp}: {e}")
            
            if deleted_any:
                rel_path = os.path.relpath(file_path, engine_dir)
                print(f"[删除] {rel_path} -> {file_stem}.generated.h/cpp")
                deleted_count += 1
            
            # 标记需要从缓存中移除
            files_to_remove.append(file_path)
    
    # 从缓存中移除已删除的文件
    for file_path in files_to_remove:
        cache.pop(file_path, None)
    
    # 第二步：扫描Generated目录中的所有生成文件，检查对应的源文件是否存在
    if os.path.exists(generated_dir):
        for file_name in os.listdir(generated_dir):
            if file_name.endswith(('.generated.h', '.generated.cpp')):
                generated_file = os.path.join(generated_dir, file_name)
                
                # 跳过已处理的文件
                if generated_file in processed_generated_files:
                    continue
                
                # 查找对应的源文件
                source_file = find_source_file_for_generated(generated_file, engine_dir)
                
                if source_file is None:
                    # 源文件不存在，删除生成文件
                    try:
                        os.remove(generated_file)
                        file_stem = Path(file_name).stem.replace('.generated', '')
                        print(f"[删除] 孤立生成文件 -> {file_name} (源文件不存在)")
                        deleted_count += 1
                    except Exception as e:
                        print(f"[WARN] 无法删除 {generated_file}: {e}")
    
    return deleted_count


def main():
    """主函数"""
    import sys
    
    # 确定路径
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    engine_dir = os.path.join(project_root, "Engine")
    generated_dir = os.path.join(engine_dir, "Generated")
    cache_file = os.path.join(generated_dir, ".cache", "file_hashes.txt")
    struct_cache_file = os.path.join(generated_dir, ".cache", "struct_cache.txt")
    
    print(f"搜索目录: {engine_dir}")
    print(f"输出目录: {generated_dir}")
    
    # 加载缓存
    cache = load_cache(cache_file)
    struct_cache = load_struct_cache(struct_cache_file)
    
    # 清理已删除源文件对应的生成文件
    deleted_count = cleanup_deleted_files(cache, struct_cache, engine_dir, generated_dir)
    if deleted_count > 0:
        print(f"清理了 {deleted_count} 个已删除源文件的生成文件")
    
    # 查找所有头文件
    header_files = find_header_files(engine_dir)
    
    if not header_files:
        print("未找到包含 HSTRUCT/HCLASS 的头文件")
        return 0
    
    print(f"找到 {len(header_files)} 个文件需要处理")
    
    # 使用多进程处理
    num_workers = min(multiprocessing.cpu_count(), len(header_files))
    
    success_count = 0
    fail_count = 0
    
    # 由于需要更新共享的缓存字典，我们需要在主进程中处理结果
    with ProcessPoolExecutor(max_workers=num_workers) as executor:
        futures = {
            executor.submit(process_file, f, engine_dir, generated_dir, cache, struct_cache): f
            for f in header_files
        }
        
        for future in as_completed(futures):
            result = future.result()
            if len(result) == 4:
                success, message, file_hash, struct_cache_info = result
            else:
                # 兼容旧版本
                success, message, file_hash = result[:3]
                struct_cache_info = result[3] if len(result) > 3 else None
            
            if success:
                success_count += 1
                print(f"[OK] {message}")
                # 更新缓存
                if file_hash is not None:
                    file_path = futures[future]
                    cache[file_path] = file_hash
                # 更新结构体缓存
                if struct_cache_info is not None:
                    # struct_cache_info 可能是单个元组或列表
                    if isinstance(struct_cache_info, list):
                        for class_name, macro_type in struct_cache_info:
                            struct_cache[class_name] = macro_type
                    else:
                        class_name, macro_type = struct_cache_info
                        struct_cache[class_name] = macro_type
            else:
                fail_count += 1
                print(f"[FAIL] {message}")
    
    # 保存更新后的缓存
    save_cache(cache_file, cache)
    save_struct_cache(struct_cache_file, struct_cache)
    
    print(f"\n完成: {success_count} 成功, {fail_count} 失败")
    return 0 if fail_count == 0 else 1


if __name__ == "__main__":
    exit(main())

