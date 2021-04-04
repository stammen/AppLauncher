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
using System.Drawing;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace AppLauncherExtension
{
    public partial class Form1 : Form
    {
        [DllImport("ApplicationsHelper.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.StdCall)]
        private static extern bool LaunchInstalledApplication([MarshalAs(UnmanagedType.BStr)] string version);

        private String _appName;

        public Form1(String appName)
        {
            InitializeComponent();
            _appName = appName;
            this.ShowInTaskbar = false;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Size = new Size(250, 60);
            this.Shown += new System.EventHandler(this.OnShown);
            this.StartPosition = FormStartPosition.CenterScreen;
        }

        private void OnShown(object sender, EventArgs e)
        {
            this.BringToFront();
            DrawString();
            LaunchInstalledApplication(_appName);
            //Thread.Sleep(1000);
            this.Close();
        }

        public void DrawString()
        {
            System.Drawing.Graphics formGraphics = this.CreateGraphics();
            string drawString = "Launching " + _appName + "...";
            System.Drawing.Font drawFont = new System.Drawing.Font("Arial", 12);
            System.Drawing.SolidBrush drawBrush = new System.Drawing.SolidBrush(System.Drawing.Color.Black);
            float x = 25.0F;
            float y = 20.0F;
            System.Drawing.StringFormat drawFormat = new System.Drawing.StringFormat();
            formGraphics.DrawString(drawString, drawFont, drawBrush, x, y, drawFormat);
            drawFont.Dispose();
            drawBrush.Dispose();
            formGraphics.Dispose();
        }
    }
}
