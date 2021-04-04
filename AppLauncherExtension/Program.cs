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
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Web;

namespace AppLauncherExtension
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            string[] args = Environment.GetCommandLineArgs();

            if (args.Length == 2)
            {
                Uri myUri = new Uri(args[1]);
                var query = HttpUtility.ParseQueryString(myUri.Query);
                if(query != null && query["action"] != null)
                {
                    string action = query.Get("action");
                    switch(action)
                    {
                        case "launchApplication":
                            var name = query.Get("name");
                            Application.Run(new Form1(name));
                            break;
                        case "getApplications":
                            Application.Run(new Applications());
                            break;
                    }
                }
            }
        }
    }
}
