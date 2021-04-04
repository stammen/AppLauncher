//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
// 
// // The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the APPLICATIONSHELPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// APPLICATIONSHELPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef APPLICATIONSHELPER_EXPORTS
#define APPLICATIONSHELPER_API __declspec(dllexport)
#else
#define APPLICATIONSHELPER_API __declspec(dllimport)
#endif

// This class is exported from the dll
class APPLICATIONSHELPER_API CApplicationsHelper {
public:
	CApplicationsHelper(void);
	// TODO: add your methods here.
};

extern APPLICATIONSHELPER_API int nApplicationsHelper;

APPLICATIONSHELPER_API int fnApplicationsHelper(void);
