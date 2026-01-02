using System.Reflection.PortableExecutable;
using System.IO.Compression;
using archbuilder.helpers;
using System.Security.Cryptography;
namespace archbuilder
{

    public class Program
    {
        public static async Task Main(string[] args)
        {
            Directory.CreateDirectory("out");
            using (var progressBar = new ProgressBar())
            {
                await DownloadHelper.DownloadFileAsync("https://github.com/torvalds/linux/archive/refs/heads/master.zip", "out/linux.zip", progressBar);
                await unzipFile("out/linux.zip", "out/", progressBar);
                Directory.Move("out/linux-master", "out/linux");
            }
            if (!cx.IsLinux())
            {
                Console.WriteLine("This program is intended to run on Linux systems.");
                return;
            } else
            {
                Console.WriteLine("Linux system detected. Proceeding with execution...");
                
            }

        }
        public static async Task unzipFile(string zipPath, string extractPath, ProgressBar progressBar)
        {
            ZipFile.ExtractToDirectory(zipPath, extractPath);
        }
    }
}