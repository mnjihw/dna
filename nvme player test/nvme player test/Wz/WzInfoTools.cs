namespace Wz
{
    public static class WzInfoTools
    {
        public static string RemoveExtension(string source)
        {
            if (source.Substring(source.Length - 4) == ".img")
                return source.Substring(0, source.Length - 4);
            return source;
        }
    }
}
