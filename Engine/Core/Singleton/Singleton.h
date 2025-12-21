#pragma once

template <typename T>
class FSingleton
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

    virtual ~FSingleton() = default;
    virtual void StartUp() {}
    virtual void ShutDown() {}

    static T* Get()
    {
        if (!Instance)
        {
            Instance = new T();
            Instance->StartUp();
        }
        return Instance;
    }

    static T& GetRef()
    {
        return *Get();
    }
};