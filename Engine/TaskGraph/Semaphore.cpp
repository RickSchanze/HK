#include "TaskGraph/Semaphore.h"

#ifdef HK_WINDOWS
#include <windows.h>
#endif

FSemaphore::FSemaphore(Int32 InitialCount) : Count(InitialCount)
{
#ifdef HK_WINDOWS
    Semaphore = CreateSemaphoreW(nullptr, InitialCount, MAXLONG, nullptr);
    HK_ASSERT_RAW(Semaphore != nullptr);
#endif
}

FSemaphore::~FSemaphore()
{
#ifdef HK_WINDOWS
    if (Semaphore != nullptr)
    {
        CloseHandle(Semaphore);
    }
#endif
}

FSemaphore& FSemaphore::operator=(FSemaphore&& Other) noexcept
{
    if (this != &Other)
    {
#ifdef HK_WINDOWS
        if (Semaphore != nullptr)
        {
            CloseHandle(Semaphore);
        }
        Semaphore = Other.Semaphore;
        Other.Semaphore = nullptr;
#endif
        Count = Other.Count;
        Other.Count = 0;
    }
    return *this;
}

void FSemaphore::Wait()
{
#ifdef HK_WINDOWS
    DWORD Result = WaitForSingleObject(Semaphore, INFINITE);
    HK_ASSERT_RAW(Result == WAIT_OBJECT_0);
    --Count;
#endif
}

bool FSemaphore::TryWait()
{
#ifdef HK_WINDOWS
    DWORD Result = WaitForSingleObject(Semaphore, 0);
    if (Result == WAIT_OBJECT_0)
    {
        --Count;
        return true;
    }
    return false;
#endif
}

void FSemaphore::Signal()
{
#ifdef HK_WINDOWS
    LONG PreviousCount;
    BOOL Result = ReleaseSemaphore(Semaphore, 1, &PreviousCount);
    HK_ASSERT_RAW(Result != FALSE);
    ++Count;
#endif
}

void FSemaphore::Signal(Int32 InCount)
{
#ifdef HK_WINDOWS
    LONG PreviousCount;
    BOOL Result = ReleaseSemaphore(Semaphore, InCount, &PreviousCount);
    HK_ASSERT_RAW(Result != FALSE);
    Count += InCount;
#endif
}

