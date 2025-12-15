#pragma once

template <typename T>
class FSingleton
{
public:
    virtual ~FSingleton() = default;
    virtual void StartUp() {}
    virtual void ShutDown() {}

    static T* Get()
    {
        static T* Instance;
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