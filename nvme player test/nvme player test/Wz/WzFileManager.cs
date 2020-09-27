using System;
using System.Collections.Generic;
using MapleLib.WzLib;
using MapleLib.WzLib.WzProperties;
using System.IO;


namespace Wz
{
    public class WzFileManager
    {
        public Dictionary<string, WzMainDirectory> wzDirs = new Dictionary<string, WzMainDirectory>();
        private WzMapleVersion version;

        public WzFileManager(string directory, WzMapleVersion version)
        {
            BaseDir = directory;
            this.version = version;
        }

        public WzFileManager(string directory)
        {
            BaseDir = directory;
            version = WzMapleVersion.GENERATE;
        }

        private string Capitalize(string x)
        {
            if (x.Length > 0 && char.IsLower(x[0]))
            {
                return new string(new char[] { char.ToUpper(x[0]) }) + x.Substring(1);
            }
            else
            {
                return x;
            }
        }

        public bool LoadWzFile(string name)
        {
            try
            {
                WzFile wzf = new WzFile(Path.Combine(BaseDir, Capitalize(name) + ".wz"), version);
                wzf.ParseWzFile();
                name = name.ToLower();
                wzDirs[name] = new WzMainDirectory(wzf);
                return true;
            }
            catch (Exception e)
            {
                //HaRepackerLib.Warning.Error
                Console.WriteLine("Error initializing " + name + ".wz (" + e.Message + ").\r\nCheck that the directory is valid and the file is not in use.");
                return false;
            }
        }


        public WzMainDirectory GetMainDirectoryByName(string name)
        {
            return wzDirs[name.ToLower()];
        }
        
        public WzDirectory this[string name]
        {
            get { return wzDirs[name.ToLower()].MainDir; }
        }

        public WzDirectory String
        {
            get { return GetMainDirectoryByName("string").MainDir; }
        }

        public string BaseDir { get; }


        public void ExtractSoundFile(WzInformationManager wzInformationManager)
        {
            foreach (WzImage soundImage in this["sound"].WzImages)
            {
                if (!soundImage.Name.ToLower().Contains("bgm"))
                    continue;
                if (!soundImage.Parsed)
                    soundImage.ParseImage();
                foreach (WzSoundProperty bgm in soundImage.WzProperties)
                {
                    if(bgm.Name != "battleHorizonTheme" && bgm.Name != "Silence")
                        wzInformationManager.BGMs[$@"{WzInfoTools.RemoveExtension(soundImage.Name)}/{bgm.Name}"] = bgm;
                }
            }
        }

  

    }
}
