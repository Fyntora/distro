using System;
using System.Text;
using System.Threading;

namespace archbuilder.helpers
{
    public class ProgressBar : IDisposable, IProgress<double>
    {
        private const int blockCount = 10;
        private readonly TimeSpan animationInterval = TimeSpan.FromSeconds(1.0 / 8);
        private const string animation = @"|/-\";

        private readonly Timer? timer;
        private readonly object lockObject = new object();

        private double currentProgress = 0;
        private string currentText = string.Empty;
        private bool disposed = false;
        private int animationIndex = 0;
        private long totalBytes = 0;
        private readonly bool isOutputRedirected;

        public ProgressBar()
        {
            isOutputRedirected = Console.IsOutputRedirected;

            if (!isOutputRedirected)
            {
                timer = new Timer(TimerHandler);
                ResetTimer();
            }
        }

        public void Report(double value)
        {
            value = Math.Max(0, Math.Min(1, value));
            Interlocked.Exchange(ref currentProgress, value);
        }

        public void Report(double value, long totalBytes)
        {
            Interlocked.Exchange(ref this.totalBytes, totalBytes);
            Report(value);
        }

        private void TimerHandler(object? state)
        {
            if (disposed) return;

            lock (lockObject)
            {
                if (disposed) return;

                int progressBlockCount = (int)(currentProgress * blockCount);
                int percent = (int)(currentProgress * 100);

                string bytesInfo = string.Empty;
                if (totalBytes > 0)
                {
                    long downloadedBytes = (long)(currentProgress * totalBytes);
                    bytesInfo = $" {FormatBytes(downloadedBytes)} / {FormatBytes(totalBytes)}";
                }

                string text = string.Format("[{0}{1}] {2,3}%{3} {4}",
                    new string('#', progressBlockCount),
                    new string('-', blockCount - progressBlockCount),
                    percent,
                    bytesInfo,
                    animation[animationIndex++ % animation.Length]);
                UpdateText(text);

                ResetTimer();
            }
        }

        private static string FormatBytes(long bytes)
        {
            if (bytes == 0) return "0 B";

            string[] suffixes = { "B", "KB", "MB", "GB", "TB" };
            int suffixIndex = 0;
            double value = bytes;
            
            while (Math.Round(value / 1024) >= 1 && suffixIndex < suffixes.Length - 1)
            {
                value /= 1024;
                suffixIndex++;
            }
            
            return $"{value:n1} {suffixes[suffixIndex]}";
        }

        private void UpdateText(string text)
        {
            int commonPrefixLength = 0;
            int commonLength = Math.Min(currentText.Length, text.Length);
            
            while (commonPrefixLength < commonLength && 
                   text[commonPrefixLength] == currentText[commonPrefixLength])
            {
                commonPrefixLength++;
            }

            StringBuilder outputBuilder = new StringBuilder();
            outputBuilder.Append('\b', currentText.Length - commonPrefixLength);

            outputBuilder.Append(text.Substring(commonPrefixLength));

            int overlapCount = currentText.Length - text.Length;
            if (overlapCount > 0)
            {
                outputBuilder.Append(' ', overlapCount);
                outputBuilder.Append('\b', overlapCount);
            }

            Console.Write(outputBuilder);
            currentText = text;
        }

        private void ResetTimer()
        {
            timer?.Change(animationInterval, TimeSpan.FromMilliseconds(-1));
        }

        public void Dispose()
        {
            if (disposed) return;

            lock (lockObject)
            {
                if (disposed) return;
                
                disposed = true;
                
                if (!isOutputRedirected)
                {
                    UpdateText(string.Empty);
                    Console.WriteLine();
                }
                
                timer?.Dispose();
            }
            
            GC.SuppressFinalize(this);
        }
    }
}