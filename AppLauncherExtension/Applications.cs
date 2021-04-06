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
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Windows.ApplicationModel;
using Windows.ApplicationModel.AppService;
using Windows.Foundation.Collections;

namespace AppLauncherExtension
{
    public partial class Applications : Form
    {
        [DllImport("ApplicationsHelper.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        private static extern void GetInstalledApplicationNames([MarshalAs(UnmanagedType.SafeArray)] out string[] array);

        [DllImport("ApplicationsHelper.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        static extern bool UpdateInstalledApplications();

        public Applications()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            this.SuspendLayout();
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(0, 0);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.WindowState = System.Windows.Forms.FormWindowState.Minimized;
            this.ShowInTaskbar = false;
            this.Shown += new System.EventHandler(this.OnLoaded);
            //this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.SystemControl_FormClosing);
            this.ResumeLayout(false);
        }

        private async void OnLoaded(object sender, EventArgs e)
        {
            ValueSet result = new ValueSet();

            if (UpdateInstalledApplications())
            {
                string[] array;
                GetInstalledApplicationNames(out array);
                result.Add("Message", "getApplications");

                result.Add("Applications", array);
                result.Add("Status", "OK");

                AppServiceConnection connection = new AppServiceConnection();
                connection.PackageFamilyName = Package.Current.Id.FamilyName;
                connection.AppServiceName = "com.stammen.applauncher.appservice";
                AppServiceConnectionStatus status = await connection.OpenAsync();
                if (status != AppServiceConnectionStatus.Success)
                {
                    Debug.WriteLine(status);
                    Application.Exit();
                }
                AppServiceResponse response = await connection.SendMessageAsync(result);
            }

            Application.Exit();
        }

    }
}
