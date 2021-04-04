//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#include "pch.h"
#include "Applications.h"

#include <string>
#include <OAIdl.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

BOOL __stdcall UpdateInstalledApplications()
{
    return Applications::UpdateApplications();
}

int __stdcall LaunchInstalledApplication(BSTR name)
{
    if (name != nullptr)
    {
        std::wstring ws(name, SysStringLen(name));
        return Applications::LaunchApplication(ws);
    }

    return -1;
}

void __stdcall GetInstalledApplicationNames(SAFEARRAY*& pSafeArray)
{
    auto count = Applications::GetNumApplications();

    if (count > 0)
    {
        SAFEARRAYBOUND  Bound;
        Bound.lLbound = 0;
        Bound.cElements = count;

        pSafeArray = SafeArrayCreate(VT_BSTR, 1, &Bound);

        BSTR* pData;
        HRESULT hr = SafeArrayAccessData(pSafeArray, (void**)&pData);
        if (SUCCEEDED(hr))
        {
            for (DWORD i = 0; i < count; i++)
            {
                *pData++ = SysAllocString(Applications::GetApplication(i));
            }
            SafeArrayUnaccessData(pSafeArray);
        }
    }
    else
    {
        pSafeArray = nullptr;
    }
}

