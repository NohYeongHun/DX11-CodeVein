#pragma once
class CIdGen
{
public:
    static uint32_t Next()
    {
        static std::atomic<uint32_t> s_Counter{ 0 };
        return ++s_Counter;      // 1,2,3…
    }
};

