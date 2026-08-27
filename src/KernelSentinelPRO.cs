using System;
using System.Diagnostics;
using System.IO;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Threading;

namespace KernelSentinelPRO
{
    class Program
    {
        private static Process serverProcess = null;

        [DllImport("Kernel32")]
        private static extern bool SetConsoleCtrlHandler(EventHandler handler, bool add);
        private delegate bool EventHandler(CtrlType sig);
        private static EventHandler exitHandler;

        private enum CtrlType
        {
            CTRL_C_EVENT = 0,
            CTRL_BREAK_EVENT = 1,
            CTRL_CLOSE_EVENT = 2,
            CTRL_LOGOFF_EVENT = 5,
            CTRL_SHUTDOWN_EVENT = 6
        }

        private static bool Handler(CtrlType sig)
        {
            KillServer();
            return true;
        }

        static void Main(string[] args)
        {
            Console.Title = "KernelSentinel PRO - Live Windows Kernel & System Optimizer Engine";
            exitHandler += new EventHandler(Handler);
            SetConsoleCtrlHandler(exitHandler, true);

            PrintHeader();

            string appDir = AppDomain.CurrentDomain.BaseDirectory;
            string serverScript = Path.Combine(appDir, "server.ps1");

            if (!File.Exists(serverScript))
            {
                string parentDir = Directory.GetParent(appDir).FullName;
                string parentScript = Path.Combine(parentDir, "server.ps1");
                if (File.Exists(parentScript))
                {
                    serverScript = parentScript;
                    appDir = parentDir;
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.WriteLine("[ERROR] Could not locate 'server.ps1' in: " + appDir);
                    Console.ResetColor();
                    Console.WriteLine("Press any key to exit...");
                    Console.ReadKey();
                    return;
                }
            }

            int port = 8080;
            if (IsPortOccupied(8080))
            {
                port = 8085;
            }

            string dashboardUrl = "http://127.0.0.1:" + port;

            Console.ForegroundColor = ConsoleColor.Yellow;
            Console.WriteLine("[*] Starting KernelSentinel Engine on " + dashboardUrl + "...");
            Console.ResetColor();

            try
            {
                ProcessStartInfo psi = new ProcessStartInfo();
                psi.FileName = "powershell.exe";
                psi.Arguments = "-ExecutionPolicy Bypass -NoProfile -File \"" + serverScript + "\" -Port " + port;
                psi.WorkingDirectory = appDir;
                psi.UseShellExecute = false;
                psi.CreateNoWindow = true;

                serverProcess = Process.Start(psi);

                Console.ForegroundColor = ConsoleColor.Green;
                Console.WriteLine("[+] Kernel Sentinel Server Active (PID: " + serverProcess.Id + ")");
                Console.ResetColor();

                Thread.Sleep(1000);

                Console.ForegroundColor = ConsoleColor.Cyan;
                Console.WriteLine("[*] Launching 60 FPS Dashboard in Default Browser...");
                Console.ResetColor();

                Process.Start(new ProcessStartInfo(dashboardUrl) { UseShellExecute = true });

                Console.WriteLine();
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine("================================================================");
                Console.WriteLine("  KERNELSENTINEL PRO IS ACTIVE & RUNNING IN BACKGROUND");
                Console.WriteLine("  Web URL: " + dashboardUrl);
                Console.WriteLine("  Engine: Direct Win32 Memory & 1s Live Kernel Telemetry");
                Console.WriteLine("================================================================");
                Console.ResetColor();
                Console.WriteLine();
                Console.WriteLine("Press [Q] or [ESC] to stop the Sentinel server...");

                while (true)
                {
                    if (Console.KeyAvailable)
                    {
                        var key = Console.ReadKey(true);
                        if (key.Key == ConsoleKey.Q || key.Key == ConsoleKey.Escape)
                        {
                            break;
                        }
                    }

                    if (serverProcess != null && serverProcess.HasExited)
                    {
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("[!] Server terminated. Exiting...");
                        Console.ResetColor();
                        break;
                    }

                    Thread.Sleep(500);
                }
            }
            catch (Exception ex)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("[ERROR] Failed to launch Sentinel: " + ex.Message);
                Console.ResetColor();
            }
            finally
            {
                KillServer();
            }

            Console.ForegroundColor = ConsoleColor.DarkGray;
            Console.WriteLine("[*] KernelSentinel PRO shutdown cleanly.");
            Console.ResetColor();
        }

        private static void KillServer()
        {
            try
            {
                if (serverProcess != null && !serverProcess.HasExited)
                {
                    serverProcess.Kill();
                    serverProcess.Dispose();
                }
            }
            catch { }
        }

        private static bool IsPortOccupied(int port)
        {
            try
            {
                using (TcpClient tcp = new TcpClient())
                {
                    tcp.Connect("127.0.0.1", port);
                    return true;
                }
            }
            catch
            {
                return false;
            }
        }

        private static void PrintHeader()
        {
            Console.ForegroundColor = ConsoleColor.Cyan;
            Console.WriteLine("================================================================");
            Console.WriteLine("   KERNELSENTINEL PRO - WINDOWS KERNEL & SYSTEM OPTIMIZER        ");
            Console.WriteLine("   Native Standalone 64-bit Engine                              ");
            Console.WriteLine("================================================================");
            Console.ResetColor();
            Console.WriteLine();
        }
    }
}
