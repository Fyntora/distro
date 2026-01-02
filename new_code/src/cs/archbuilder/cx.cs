using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Cryptography.X509Certificates;
namespace cx
{

#pragma warning disable CS8981 // The type name only contains lower-cased ascii characters. Such names may become reserved for the language.
    public class commands
#pragma warning restore CS8981 // The type name only contains lower-cased ascii characters. Such names may become reserved for the language.
    {


        public static void ExecuteCommand(string shell, string command, bool root, bool debug)
        {
            try
            {
                string fileName = "";
                string arguments = "";
                bool isLinux = RuntimeInformation.IsOSPlatform(OSPlatform.Linux);
                bool isMacOS = RuntimeInformation.IsOSPlatform(OSPlatform.OSX);
                bool isWindows = RuntimeInformation.IsOSPlatform(OSPlatform.Windows);

                if (isWindows)
                {
                    if (root)
                    {
                        fileName = "powershell";
                        arguments = $"Start-Process cmd -Verb RunAs -ArgumentList '/c {command}'";
                    }
                    else
                    {
                        fileName = "cmd.exe";
                        arguments = $"/c {command}";
                    }
                }
                else
                {
                    if (string.IsNullOrWhiteSpace(shell))
                    {
                        Console.WriteLine("No shell specified, defaulting to bash.");
                        shell = "bash"; // Default to bash if no shell is specified
                    }

                    if (root)
                    {
                        fileName = "sudo";
                        arguments = $"/bin/{shell} -c \"{command}\"";
                    }
                    else
                    {
                        fileName = $"/bin/{shell}";
                        arguments = $"-c \"{command}\"";
                    }
                }

                ProcessStartInfo startInfo = new ProcessStartInfo()
                {
                    FileName = fileName,
                    Arguments = arguments,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true
                };

#pragma warning disable CS8600 // Converting null literal or possible null value to non-nullable type.
                using (Process process = Process.Start(startInfo))
                {
#pragma warning disable CS8602 // Dereference of a possibly null reference.
                    while (!process.StandardOutput.EndOfStream)
                    {
                        string line = process.StandardOutput.ReadLine();
                        if (line != null)
                            Console.WriteLine(line);
                    }
#pragma warning restore CS8602 // Dereference of a possibly null reference.

                    while (!process.StandardError.EndOfStream)
                    {
                        string line = process.StandardError.ReadLine();
                        if (line != null)
                            Console.WriteLine(line);
                    }

                    process.WaitForExit();
                }
#pragma warning restore CS8600 // Converting null literal or possible null value to non-nullable type.
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error executing command: {ex.Message}");
            }

        }
    }
    public class OsChecker
    {
        public static bool IsLinux()
        {
            return RuntimeInformation.IsOSPlatform(OSPlatform.Linux);
        }

        public static bool IsMacOS()
        {
            return RuntimeInformation.IsOSPlatform(OSPlatform.OSX);
        }

        public static bool IsWindows()
        {
            return OperatingSystem.IsWindows();
        }
        public static string currentOS
        {
            get
            {
                if (IsLinux()) return "linux";
                if (IsMacOS()) return "macos";
                if (IsWindows()) return "windows";
                return "unknown";
            }
        }
    }

    public class Checks
    {
        public static bool IfExists(string path)
        {
            return System.IO.File.Exists(path) || System.IO.Directory.Exists(path);
        }


        public static void IsRunning(string processName, int processID, Action ifRunningAction, Action ifNotRunningAction)
        {
            try
            {
                if (processID == 0)
                {
                    Process[] processes = Process.GetProcessesByName(processName);
                    if (processes.Length > 0)
                    {
                        ifRunningAction();
                    }
                    else
                    {
                        ifNotRunningAction();
                    }
                }
                else
                {
                    Process process = Process.GetProcessById(processID);
                    if (process != null && process.ProcessName.Equals(processName, StringComparison.OrdinalIgnoreCase))
                    {
                        ifRunningAction();
                    }
                    else
                    {
                        ifNotRunningAction();
                    }
                }
            }
            catch (ArgumentException)
            {
                ifNotRunningAction();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error checking process: {ex.Message}");
                ifNotRunningAction();
            }

        }
        public static bool IsRooted()
        {
            if (OsChecker.IsLinux() || OsChecker.IsMacOS())
            {
                return Environment.UserName == "root";
            }
            else if (OsChecker.IsWindows())
            {
                var identity = System.Security.Principal.WindowsIdentity.GetCurrent();
                var principal = new System.Security.Principal.WindowsPrincipal(identity);
                return principal.IsInRole(System.Security.Principal.WindowsBuiltInRole.Administrator);


            }
            return false;
        }
    }
}