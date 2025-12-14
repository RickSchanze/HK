#pragma once

#include "Core/Utility/Macros.h"
#include "cereal/archives/json.hpp"

class FJsonOutputArchive : public cereal::JSONOutputArchive
{
public:
    struct FOptions
    {
        // 浮点数精度
        Int32 Precision;
        // 缩进
        Int32 Indent;

        FOptions() : Precision(3), Indent(4) {}
    };

    explicit FJsonOutputArchive(std::ostream& Stream, FOptions const& Options = FOptions())
        : JSONOutputArchive(
              Stream, cereal::JSONOutputArchive::Options(
                          Options.Precision, cereal::JSONOutputArchive::Options::IndentChar::space, Options.Indent))
    {
    }
};

class FJsonInputArchive : public cereal::JSONInputArchive
{
public:
    explicit FJsonInputArchive(std::istream& Stream) : JSONInputArchive(Stream) {}
};
