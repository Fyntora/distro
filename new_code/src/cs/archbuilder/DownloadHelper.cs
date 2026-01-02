using System;
using System.Net.Http;
using System.IO;
using System.Threading.Tasks;

namespace archbuilder.helpers
{
    public class DownloadHelper
    {
        private static readonly HttpClient httpClient = new HttpClient();

        public static async Task DownloadFileAsync(string url, string destinationPath)
        {
            await DownloadFileAsync(url, destinationPath, null);
        }

        public static async Task DownloadFileAsync(string url, string destinationPath, IProgress<double>? progress)
        {
            using (var response = await httpClient.GetAsync(url, HttpCompletionOption.ResponseHeadersRead))
            {
                response.EnsureSuccessStatusCode();

                var totalBytes = response.Content.Headers.ContentLength;

                using (var contentStream = await response.Content.ReadAsStreamAsync())
                using (var fileStream = new FileStream(destinationPath, FileMode.Create, FileAccess.Write, FileShare.None))
                {
                    var totalBytesRead = 0L;
                    var buffer = new byte[8192];
                    var isMoreToRead = true;

                    do
                    {
                        var bytesRead = await contentStream.ReadAsync(buffer, 0, buffer.Length);
                        if (bytesRead == 0)
                        {
                            isMoreToRead = false;
                        }
                        else
                        {
                            await fileStream.WriteAsync(buffer, 0, bytesRead);

                            totalBytesRead += bytesRead;

                            if (progress != null && totalBytes.HasValue)
                            {
                                if (progress is ProgressBar progressBar)
                                {
                                    progressBar.Report((double)totalBytesRead / totalBytes.Value, totalBytes.Value);
                                }
                                else
                                {
                                    progress.Report((double)totalBytesRead / totalBytes.Value);
                                }
                            }
                        }
                    }
                    while (isMoreToRead);
                }
            }
        }
    }
}