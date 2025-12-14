#pragma once
#define CEREAL_XML_STRING_VALUE "HK"
#include "Core/Utility/Macros.h"
#include "cereal/archives/xml.hpp"

class FXMLOutputArchive : public cereal::XMLOutputArchive
{
public:
    struct FOptions
    {
        Int32 Precision;
        Int32 Indent;
        bool Size;
        bool Type;

        FOptions() : Precision(3), Indent(4), Size(false), Type(false) {}
    };

    FXMLOutputArchive(std::ostream& Stream, FOptions Options = FOptions())
        : cereal::XMLOutputArchive(
              Stream, XMLOutputArchive::Options(Options.Precision, Options.Indent, Options.Type, Options.Size))
    {
    }
};

class FXMLInputArchive : public cereal::XMLInputArchive
{
public:
    FXMLInputArchive(std::istream& Stream) : cereal::XMLInputArchive(Stream) {}
};
