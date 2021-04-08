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
#include <collection.h>
#include <atlbase.h>
#include <Shellapi.h>
#include <shlobj.h>
#include <propkey.h>
#include <string>
#include <sstream>     
#include <iostream>
#include <ppltasks.h>

using namespace Windows::ApplicationModel;
using namespace Windows::Storage;
using namespace Windows::Foundation::Collections;
using namespace Platform::Collections;
using namespace Windows::Management::Deployment;
using namespace concurrency;

std::mutex Applications::s_mutex;
std::vector<std::wstring> Applications::s_applications;

Applications::Applications()
{
}


Applications::~Applications()
{
}


HRESULT LaunchUWPApp(LPCWSTR aumid)
{
    Platform::String^ id = ref new  Platform::String(aumid);

    auto pm = ref new PackageManager();
    auto packages = pm->FindPackagesForUser(L"");
    for (Package^ p : packages)
    {
        std::wcout << "DisplayName:" << p->DisplayName->Data() << std::endl;

        auto appListTask = create_task(p->GetAppListEntriesAsync());
        appListTask.wait();
        auto appListEntries = appListTask.get();

        if (appListEntries == nullptr || appListEntries->Size == 0)
        {
            continue;
        }

        auto appListEntry = p->GetAppListEntries()->GetAt(0);

        if (appListEntry->AppUserModelId == id)
        {
            auto task = create_task(appListEntry->LaunchAsync());
            task.wait();
            bool result = task.get();
            std::cout << "LaunchAsync result:" << result << std::endl;
            if (result == true)
            {
                return S_OK;
            }
            else
            {
                break;
            }
        }
    }

    // last resort
    CComPtr<IApplicationActivationManager> AppActivationMgr = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&AppActivationMgr));
    if (SUCCEEDED(hr))
    {
        DWORD pid = 0;
        hr = AppActivationMgr->ActivateApplication(aumid, nullptr, AO_NONE, &pid);
    }
    return hr;
}

typedef struct {
    DWORD pid;
    HWND hwnd;
} WINDOWPROCESSINFO;

static BOOL CALLBACK OnGetWindowByProcess(HWND hwnd, LPARAM lParam)
{
    WINDOWPROCESSINFO* infoPtr = (WINDOWPROCESSINFO*)lParam;
    DWORD check = 0;
    BOOL br = TRUE;
    GetWindowThreadProcessId(hwnd, &check);
    if (check == infoPtr->pid)
    {
        infoPtr->hwnd = hwnd;
        br = FALSE;
    }
    return br;
}


HRESULT LaunchAppFromShortCut(IShellItem* psi)
{
    HRESULT hr;
    IShellLink* psl;

    // save the shortcut file to the app's temporary folder as we are only going to use it once
    StorageFolder^ localFolder = ApplicationData::Current->TemporaryFolder;
    Platform::String^ path = localFolder->Path + L"\\shortcut.lnk";

    // Get a pointer to the IShellLink interface. 
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl);
    if (SUCCEEDED(hr))
    {
        IPersistFile* ppf = nullptr;

        PIDLIST_ABSOLUTE pidl;

        hr = SHGetIDListFromObject(psi, &pidl);
        if (SUCCEEDED(hr))
        {
            // set the shortcut info for this app
            hr = psl->SetIDList(pidl);
            CoTaskMemFree(pidl);
        }

        // Query IShellLink for the IPersistFile interface, used for saving the 
        // shortcut in persistent storage. 
        if (SUCCEEDED(hr))
        {
            hr = psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
            if (SUCCEEDED(hr))
            {
                // Save the link by calling IPersistFile::Save. 
                hr = ppf->Save(path->Data(), TRUE);
                ppf->Release();
                if (SUCCEEDED(hr))
                {
                    // launch the app using its newly created shortcut
                    SHELLEXECUTEINFO sxi = { 0 };
                    sxi.cbSize = sizeof(sxi);
                    sxi.nShow = SW_SHOWNORMAL;
                    sxi.lpVerb = _T("open");
                    sxi.lpFile = path->Data();
                    sxi.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI | SEE_MASK_WAITFORINPUTIDLE;
                    if (!ShellExecuteEx(&sxi))
                    {
                        psl->Release();
                        return GetLastError();
                    }

                    if (sxi.hProcess != NULL)
                    {
                        WINDOWPROCESSINFO info;
                        info.pid = GetProcessId(sxi.hProcess); 
                        info.hwnd = 0;
                        //AllowSetForegroundWindow(info.pid);
                        EnumWindows(OnGetWindowByProcess, (LPARAM)&info);
                        if (info.hwnd != 0)
                        {
                            //SetForegroundWindow(info.hwnd);
                            //SetActiveWindow(info.hwnd);
                        }
                    }
                }
            }
        }
        psl->Release();
    }
    return hr;
}

std::wstring GetDisplayName(IShellItem* psi, SIGDN sigdn)
{
    LPWSTR pszName = nullptr;
    std::wstring result;
    HRESULT hr = psi->GetDisplayName(sigdn, &pszName);
    if (SUCCEEDED(hr)) {
        result = pszName;
        CoTaskMemFree(pszName);
    }

    return result;
}

Platform::String^ GetParsingPath(IShellItem2* psi2)
{
    LPWSTR pszValue;
    Platform::String^ result = ref new Platform::String();
    HRESULT hr = psi2->GetString(PKEY_ParsingPath, &pszValue);
    if (SUCCEEDED(hr)) {
        result = ref new Platform::String(pszValue);
        CoTaskMemFree(pszValue);
    }
    return result;
}

HRESULT Applications::LaunchApplication(const std::wstring& name)
{
    auto result = LaunchApplicationImp(name);

    // simulate press of ALT key to clear ForeGroundWindow lock
    INPUT inputs[2];
    ZeroMemory(inputs, sizeof(inputs));
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_MENU;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_MENU;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

    return result;
}

HRESULT Applications::LaunchApplicationImp(const std::wstring& name)
{
    ValueSet^ result = ref new ValueSet;
    IShellItem* psiFolder;
    bool found = false;

    HRESULT hr = SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT, NULL, IID_PPV_ARGS(&psiFolder));
    if (SUCCEEDED(hr))
    {
        IEnumShellItems* pesi;
        hr = psiFolder->BindToHandler(NULL, BHID_EnumItems, IID_PPV_ARGS(&pesi));
        if (hr == S_OK)
        {
            IShellItem* psi;
            while (pesi->Next(1, &psi, NULL) == S_OK && !found)
            {
                IShellItem2* psi2;
                if (SUCCEEDED(psi->QueryInterface(IID_PPV_ARGS(&psi2))))
                {
                    auto appName = GetDisplayName(psi2, SIGDN_NORMALDISPLAY);
                    if (!appName.empty())
                    {
                        if (appName == name)
                        {
                            found = true;

                            auto aumid = GetParsingPath(psi2);
                            hr = LaunchUWPApp(aumid->Data());

                            if (hr != S_OK) // Win32 desktop app
                            {
                                hr = LaunchAppFromShortCut(psi);
                                ;
                            }
                        }
                    }
                    psi2->Release();
                }
                psi->Release();
            }
            psiFolder->Release();
        }

    }
    return hr;
}

DWORD Applications::GetNumApplications()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    return static_cast<DWORD>(s_applications.size());
}

const LPWSTR Applications::GetApplication(DWORD index)
{
    std::lock_guard<std::mutex> lock(s_mutex);
    if (index < s_applications.size())
    {
        return (const LPWSTR)s_applications[index].c_str();
    }

    return nullptr;
}


BOOL Applications::UpdateApplications()
{
    std::lock_guard<std::mutex> lock(s_mutex);
    s_applications.clear();

    IShellItem* psiFolder;
    HRESULT hr = SHGetKnownFolderItem(FOLDERID_AppsFolder, KF_FLAG_DEFAULT, NULL, IID_PPV_ARGS(&psiFolder));
    if (SUCCEEDED(hr))
    {
        IEnumShellItems* pesi;
        hr = psiFolder->BindToHandler(NULL, BHID_EnumItems, IID_PPV_ARGS(&pesi));
        if (SUCCEEDED(hr))
        {
            IShellItem* psi;
            while (pesi->Next(1, &psi, NULL) == S_OK)
            {
                IShellItem2* psi2;
                if (SUCCEEDED(psi->QueryInterface(IID_PPV_ARGS(&psi2))))
                {
                    auto name = GetDisplayName(psi2, SIGDN_NORMALDISPLAY);
                    if (!name.empty())
                    {
                        s_applications.push_back(name);
                    }
                    psi2->Release();
                }
                psi->Release();
            }
            psiFolder->Release();
        }
    }

    return SUCCEEDED(hr);

}



