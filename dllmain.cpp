#include "src/mod.hpp"

// #define GDCONSOLE

#ifdef GDCONSOLE
#include "src/console.hpp"
#endif

DWORD WINAPI load_thread(LPVOID hModule) {
    if (gd::init()) {
        
        #ifdef GDCONSOLE
        if (!gd::console::load()) {
            MessageBoxA(nullptr, "Unable to hook up debugging console!", "GDShare", MB_ICONERROR);
            FreeLibraryAndExitThread((HMODULE)hModule, 0);
            return 1;
        }
        #endif

        if (mod::load()) {
            #ifdef GDCONSOLE
            gd::console::awaitUnload();
            #endif
        } else
            FreeLibraryAndExitThread((HMODULE)hModule, 0);
    } else
        FreeLibraryAndExitThread((HMODULE)hModule, 0);

    #ifdef GDCONSOLE

    mod::unload();
    gd::console::unload();
    FreeLibraryAndExitThread((HMODULE)hModule, 0);

    #endif

    return 0;
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // what the hell is this pie
        HANDLE _ = CreateThread(0, 0, load_thread, hModule, 0, nullptr);
        if (_) CloseHandle(_);
    }
    return TRUE;
}