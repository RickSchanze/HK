#include "MemoryStream.h"
#include <xxhash.h>

FHashStreamBuf::FHashStreamBuf()
{
    MyState = XXH64_createState();
    if (MyState)
    {
        XXH64_reset(static_cast<XXH64_state_t*>(MyState), 0);
    }
}

FHashStreamBuf::~FHashStreamBuf()
{
    if (MyState)
    {
        XXH64_freeState(static_cast<XXH64_state_t*>(MyState));
    }
}

UInt64 FHashStreamBuf::GetHash() const
{
    if (MyState)
    {
        return XXH64_digest(static_cast<XXH64_state_t*>(MyState));
    }
    return 0;
}

std::streamsize FHashStreamBuf::xsputn(const char* S, std::streamsize N)
{
    if (MyState && N > 0)
    {
        XXH64_update(static_cast<XXH64_state_t*>(MyState), S, static_cast<size_t>(N));
    }
    return N;
}

int FHashStreamBuf::overflow(int C)
{
    if (C != EOF && MyState)
    {
        XXH64_update(static_cast<XXH64_state_t*>(MyState), &C, 1);
    }
    return C;
}

