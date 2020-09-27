using MapleLib.WzLib;

namespace Wz
{
    public class WzMainDirectory
    {
        public WzMainDirectory(WzFile file)
        {
            File = file;
            MainDir = file.WzDirectory;
        }

        public WzMainDirectory(WzFile file, WzDirectory directory)
        {
            File = file;
            MainDir = directory;
        }

        public WzFile File { get; }
        public WzDirectory MainDir { get; }
    }
}
