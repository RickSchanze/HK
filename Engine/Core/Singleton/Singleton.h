#pragma once

#if HK_DEBUG
struct FSingletonCounter
{
    static inline Int32 Counter = 0;
};
#endif

template <typename T>
class TSingleton
{
private:
    static inline T* Instance = nullptr;

public:
    static void Destroy()
    {
        if (Instance)
        {
            Instance->ShutDown();
            delete Instance;
            Instance = nullptr;
        }
    }

    virtual ~TSingleton() = default;
    virtual void StartUp() {}
    virtual void ShutDown() {}

    static T* Get()
    {
        if (!Instance)
        {
            Instance = new T();
            Instance->StartUp();
#if HK_DEBUG
            ++FSingletonCounter::Counter;
#endif
        }
        return Instance;
    }

    static T& GetRef()
    {
        return *Get();
    }
};