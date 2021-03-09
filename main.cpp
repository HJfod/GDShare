#include "src/GDSM.hpp"
#include <Windows.h>

DWORD WINAPI unload_thread(void* hModule) {
    GDShare::live::unload();

    FreeLibraryAndExitThread((HMODULE)hModule, 0);

    exit(0);

    return 0;
}

DWORD WINAPI load_thread(void* hModule) {
    if (!GDShare::live::init()) {
        MessageBoxA(nullptr, "GDShare: Unable to initialize!", "Error", MB_ICONERROR);

        GDShare::live::unload();

        return 0;
    }

    #ifdef GDCONSOLE
    GDShare::live::awaitUnload();

    unload_thread(hModule);
    #endif

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            CreateThread(0, 0x1000, load_thread, hModule, 0, 0);
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH: break;
        case DLL_PROCESS_DETACH:
            #ifndef GDCONSOLE
            CreateThread(0, 0x1000, unload_thread, hModule, 0, 0);
            #endif
            break;
    }
    
    return true;
}

