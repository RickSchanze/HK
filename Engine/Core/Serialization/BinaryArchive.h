#pragma once

#include "cereal/archives/binary.hpp"

class FBinaryOutputArchive : public cereal::BinaryOutputArchive
{
public:
    FBinaryOutputArchive(std::ostream& Stream) : cereal::BinaryOutputArchive(Stream) {}
};

class FBinaryInputArchive : public cereal::BinaryInputArchive
{
public:
    FBinaryInputArchive(std::istream& Stream) : cereal::BinaryInputArchive(Stream) {}
};
