//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <mutex>
#include <vector>
#include <string>

class Applications
{
public:
    Applications();
    ~Applications();

    static BOOL UpdateApplications();
    static HRESULT LaunchApplication(const std::wstring& name);
    static DWORD GetNumApplications();
    static const LPWSTR GetApplication(DWORD index);

private:

    static HRESULT LaunchApplicationImp(const std::wstring& name);
    static std::mutex s_mutex;
    static std::vector<std::wstring> s_applications;
};
