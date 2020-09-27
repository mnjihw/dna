using System;
using MapleLib.WzLib.WzProperties;
using NAudio.Wave;
using System.IO;
using System.Windows;

namespace Wz
{
    public sealed class WzMp3Streamer
    {
        private WzMp3Streamer() { }
        private static readonly Lazy<WzMp3Streamer> _instance = new Lazy<WzMp3Streamer>(() => new WzMp3Streamer());
        public static WzMp3Streamer Instance { get => _instance.Value; }
        

        public float Volume
        {
            get => WavePlayer?.Volume ?? 0;
            set
            {
                if (WavePlayer != null&& value >= 0 && value <= 1.0)
                    WavePlayer.Volume = value;
                else
                    new Exception("Volume value wrong");
            }

        }

        public double Position
        {
            get
            {
                if (MpegStream != null)
                    return (double)MpegStream.Position / MpegStream.WaveFormat.AverageBytesPerSecond;
                return 0;
            }
            set
            {
                if (MpegStream != null)
                    MpegStream.Seek((long)(value * MpegStream.WaveFormat.AverageBytesPerSecond), SeekOrigin.Begin);
            }
        }
        public PlaybackState? PlaybackState => WavePlayer?.PlaybackState;
        public int Length => Sound.Length / 1000;
        private Stream ByteStream { get; set; }
        private WaveOut WavePlayer { get; set; }
        private WzSoundProperty Sound { get; set; }
        private Mp3FileReader MpegStream { get; set; }

        public void Pause() => WavePlayer.Pause();
        public void Play() => WavePlayer.Play();
        public void Stop() => WavePlayer.Stop();
        public void SetPlaybackStopped(EventHandler<StoppedEventArgs> handler) => WavePlayer.PlaybackStopped += handler;

        public void Open(WzSoundProperty sound)
        {
            if (MpegStream != null)
                Dispose();

            Sound = sound;
            ByteStream = new MemoryStream(sound.GetBytes(false));
            MpegStream = new Mp3FileReader(ByteStream);
            WavePlayer = new WaveOut(WaveCallbackInfo.FunctionCallback());
            WavePlayer.Init(MpegStream);

            //Volume = (float)(Application.Current.MainWindow as MainWindow).volumeSlider.Value;
            Volume = 1.0f;
        }
   
        public void Dispose()
        {
            if (MpegStream != null)
            {
                ByteStream.Dispose();
                WavePlayer.Dispose();
                MpegStream.Dispose();
                MpegStream = null;
            }
        }
        public void ResetPosition()
        {
            if(MpegStream != null)
                MpegStream.Seek(0, SeekOrigin.Begin);
        }
        
       
    }
}
