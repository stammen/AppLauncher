//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.AppService;
using Windows.ApplicationModel.Background;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace AppLauncher
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            App.AppServiceConnected += AppServiceConnected;
        }

        private async void LaunchApp_Click(object sender, RoutedEventArgs e)
        {
            if (applicationsComboBox.SelectedIndex > 0)
            {
                var name = applicationsComboBox.Items[applicationsComboBox.SelectedIndex] as string;
                var uri = new Uri("com.stammen.applauncherextension:?action=launchApplication&name=\"" + name + "\"");
                var success = await Windows.System.Launcher.LaunchUriAsync(uri);
            }
        }

        private async void GetApplications_Click(object sender, RoutedEventArgs e)
        {
            var uri = new Uri(@"com.stammen.applauncherextension:?action=getApplications");
            var success = await Windows.System.Launcher.LaunchUriAsync(uri);
        }

        private async void UpdateApplications(string[] applications)
        {
            await this.Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                applicationsComboBox.Items.Clear();
                applicationsComboBox.Items.Add("Applications");
                applicationsComboBox.SelectedIndex = 0;

                foreach (var s in applications)
                {
                    applicationsComboBox.Items.Add(s);
                }
            });
        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {

        }

        private void AppServiceConnected(object sender, Windows.ApplicationModel.AppService.AppServiceTriggerDetails e)
        {
            e.AppServiceConnection.RequestReceived += OnAppServiceRequestReceived;
        }

        private async void OnAppServiceRequestReceived(AppServiceConnection sender, AppServiceRequestReceivedEventArgs args)
        {
            AppServiceDeferral messageDeferral = args.GetDeferral();

            var request = args.Request.Message;

            if (!request.ContainsKey("Message"))
            {
                messageDeferral.Complete();
                Debug.WriteLine("OnAppServiceRequestReceived: Invalid message");
                return;
            }

            string message = request["Message"] as string;

            switch (message)
            {
                case "getApplications":
                    var applications = request["Applications"] as string[];
                    UpdateApplications(applications);
                    break;
            }

            ValueSet returnMessage = new ValueSet();
            returnMessage.Add("Status", "OK");
            await args.Request.SendResponseAsync(returnMessage);
            messageDeferral.Complete();

            App.CloseAppService();
        }
    }
}
