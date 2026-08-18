#include <stdafx.h>
#include "memory.h"

Memory::Memory()
{
#ifdef _WIN32
    base = (uint8_t*)VirtualAlloc((void*)0x100000000ull, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        base = (uint8_t*)VirtualAlloc(nullptr, PPC_MEMORY_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (base == nullptr)
        return;

    DWORD oldProtect;
    VirtualProtect(base, 4096, PAGE_NOACCESS, &oldProtect);
#else
    base = (uint8_t*)mmap((void*)0x100000000ull, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == (uint8_t*)MAP_FAILED)
        base = (uint8_t*)mmap(NULL, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

    if (base == nullptr)
        return;

    mprotect(base, 4096, PROT_NONE);
#endif

    extern PPCFuncMapping PPCFuncMappings_default_ppc[];
    extern PPCFuncMapping PPCFuncMappings_eawebkit_ppc[];

    for (size_t i = 0; PPCFuncMappings_default_ppc[i].guest != 0; i++)
    {
        if (PPCFuncMappings_default_ppc[i].host != nullptr)
            InsertFunction(PPCFuncMappings_default_ppc[i].guest, PPCFuncMappings_default_ppc[i].host);
    }

    for (size_t i = 0; PPCFuncMappings_eawebkit_ppc[i].guest != 0; i++)
    {
        if (PPCFuncMappings_eawebkit_ppc[i].host != nullptr)
            InsertFunction(PPCFuncMappings_eawebkit_ppc[i].guest, PPCFuncMappings_eawebkit_ppc[i].host);
    }
}

void* MmGetHostAddress(uint32_t ptr)
{
    return g_memory.Translate(ptr);
}
