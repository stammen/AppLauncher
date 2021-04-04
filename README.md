# UWP Application Launcher

This sample demonstrates how a UWP application can call Win32 methods unavailable to a UWP application to enumerate applications installed on the user's system and selectively launch an application

The example uses a Win32 Desktop Extension and an AppService.

Note: This example will only work in Desktop scenarios

## Requirements

* Visual Studio 2019 with Windows Universal App Development package installed. Please make sure C++ UWP development tools are installed
* Windows SDK version 19041

## Running the Sample

* Open AppLauncher.sln with Visual Studio 2019

* Select the Debug/x86 or Debug/x64 configuration. (Release/x86 and Release x/64 also work)

* Set the PackagingProject project as the StartUp project

* Press F5 to build and run the solution. 

* Click on the Get Applications Button. The combobox will be filled with all of the user's applications. Select an application from the combobox. Click the Launch App button to launch the application.

* Minimize the app you just launched to the taskbar. Click the Launch App button to bring the minimized application to the forground. Note: Some apps will instead launch a new instance of the app.

## Discussion

This example launches a Win32 helper app to do the following tasks:

1. Enumerate all apps installed on the user's computer

1. Launch an app

This helper app is launched by the UWP app using the following protocols:

### Enumerate apps

    com.stammen.applauncherextension:?"action=getApplications

### Launch an App

    com.stammen.applauncherextension:?"action=launchApplication&name=PowerPoint"

Once the AppLauncher app has been deployed on you computer, you can also test it out from the Command Prompt:

```console
start com.stammen.applauncherextension:?"action=getApplications
```

```console
start com.stammen.applauncherextension:?"action=launchApplication&name=PowerPoint"
```

