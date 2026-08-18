#pragma once

#include <cpu/ppc_context.h>
#include <cassert>
#include <unordered_map>
#include <vector>

#ifndef _WIN32
#define MEM_COMMIT  0x00001000  
#define MEM_RESERVE 0x00002000  
#endif

struct Memory
{
    uint8_t* base{};

    struct XexRegion
    {
        uint32_t image_base;
        uint32_t image_size;
        uint32_t code_base;
        uint32_t code_end;
    };

    std::unordered_map<uint32_t, PPCFunc*> functions;
    std::vector<XexRegion> regions;

    Memory();

    bool IsInMemoryRange(const void* host) const noexcept
    {
        return host >= base && host < (base + PPC_MEMORY_SIZE);
    }

    void* Translate(size_t offset) const noexcept
    {
        if (offset)
            assert(offset < PPC_MEMORY_SIZE);

        return base + offset;
    }

    uint32_t MapVirtual(const void* host) const noexcept
    {
        if (host)
            assert(IsInMemoryRange(host));

        return static_cast<uint32_t>(static_cast<const uint8_t*>(host) - base);
    }

    PPCFunc* FindFunction(uint32_t guest) const noexcept
    {
        auto it = functions.find(guest);
        if (it != functions.end())
            return it->second;

        return nullptr;
    }

    void InsertFunction(uint32_t guest, PPCFunc* host)
    {
        functions[guest] = host;

        for (const auto& region : regions)
        {
            if (guest >= region.code_base && guest < region.code_end)
            {
                *(PPCFunc**)(base + region.image_base + region.image_size + (uint64_t(guest - region.code_base) * 2)) = host;
                break;
            }
        }
    }

    void RegisterXexRegion(uint32_t image_base, uint32_t image_size, uint32_t code_base, uint32_t code_end)
    {
        regions.push_back({image_base, image_size, code_base, code_end});
    }
};

extern "C" void* MmGetHostAddress(uint32_t ptr);
extern Memory g_memory;
