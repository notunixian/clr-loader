// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <metahost.h>
#include <CorError.h>
#pragma comment(lib, "mscoree.lib")
#include <sstream>

#include "host.h"
#include "include/MinHook.h"
#pragma comment(lib, "libMinHook-x64-v141-mdd.lib")

void empty_func() {}


int main()
{
    MH_Initialize();

    ICLRMetaHost* meta_host = NULL;
    ICLRRuntimeInfo* runtime_info = NULL;
    ICLRRuntimeHost* runtime_host = NULL;

    if (!AllocConsole())
    {
        MessageBoxA(NULL, "failed to init console", "failure", MB_OK);
        return 1;
    }

    CLRCreateInstance(CLSID_CLRMetaHost, IID_ICLRMetaHost, ((LPVOID*)&meta_host));
    meta_host->GetRuntime(L"v4.0.30319", IID_ICLRRuntimeInfo, (LPVOID*)&runtime_info);
    runtime_info->GetInterface(CLSID_CLRRuntimeHost, IID_ICLRRuntimeHost, (LPVOID*)&runtime_host);
    ICLRControl* clr_control = nullptr;
    runtime_host->GetCLRControl(&clr_control);

    host_ctrl* host_control = host_control = new host_ctrl();
    runtime_host->SetHostControl(host_control);
    clr_control->SetAppDomainManagerType(L"VRCLoader", L"VRCLoader.Domain.DomainManager");

    runtime_host->Start();

    INetDomain* net_domain = host_control->get_domainmanager();
    net_domain->Initialize();
    net_domain->MinHook_CreateInstance(
        empty_func,
        MH_CreateHook,
        MH_RemoveHook,
        MH_EnableHook,
        MH_DisableHook);
    net_domain->OnApplicationStart();

    runtime_info->Release();
    meta_host->Release();
    runtime_host->Release();
    net_domain->Release();

    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

