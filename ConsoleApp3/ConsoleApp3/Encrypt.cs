using Sodium;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleApp3
{
    public static class Encrypt
    {

        public static string GenerateEncPassword(string password, string publicKey, string keyId, string version)
        {
            var time = ((DateTimeOffset)DateTime.UtcNow).ToUnixTimeSeconds();
            var keyBytes = publicKey.HexToBytes(); 
            var key = new byte[32];
            new Random().NextBytes(key);
            var iv = new byte[12];
            var tag = new byte[16];

            var plainText = Encoding.UTF8.GetBytes(password);
            var cipherText = new byte[plainText.Length];
            using var cipher = new AesGcm(key);

            cipher.Encrypt(iv, plainText, cipherText, tag, Encoding.UTF8.GetBytes(time.ToString()));

            var encryptedKey = SealedPublicKeyBox.Create(key, keyBytes);
            var bytesOfLen = BitConverter.GetBytes((short)encryptedKey.Length);
            var info = new byte[] { 1, byte.Parse(keyId) };
            var bytes = CombineArrays(info, bytesOfLen, encryptedKey, tag, cipherText);

            return $"#PWD_INSTAGRAM_BROWSER:{version}:{time}:{Convert.ToBase64String(bytes)}";
        }

        public static byte[] HexToBytes(this string hex) => Enumerable.Range(0, hex.Length / 2).Select(x => Convert.ToByte(hex[(x * 2) .. (x * 2 + 2)], 16)).ToArray();



        private static T[] CombineArrays<T>(params T[][] arrays)
        {
            T[] rv = new T[arrays.Sum(a => a.Length)];
            int offset = 0;

            foreach(T[] array in arrays)
            {
                Buffer.BlockCopy(array, 0, rv, offset, array.Length);
                offset += array.Length;
            }
            return rv;
        }


    }
}
