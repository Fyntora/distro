using System.Reflection.PortableExecutable;
using archbuilder.helpers;
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
            }
        }
    }
}