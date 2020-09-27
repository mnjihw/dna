using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace 리듬_DC_콘솔_봇
{
    public sealed class Hangul
    {
        private static char[] InitialTable => new [] { 'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };
        private static char[] MedialTable => new [] { 'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ', 'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ', 'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ', 'ㅡ', 'ㅢ', 'ㅣ' };
        private static char[] FinalTable => new [] { ' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ', 'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ', 'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ' };
        private static int UnicodeHangulSyllablesFirst => 0xAC00;
        private static int UnicodeHangulSyllablesLast => 0xD7AF;
        private static int UnicodeHangulCompatibilityJamoFirst => 0x3131;
        private static int UnicodeHangulCompatibilityJamoLast => 0x318F;

        public char? Initial { get; }
        public char? Medial { get; }
        public char Final { get; }

        public Hangul(char? initial) : this(initial, null) { }        
        public Hangul(char? initial, char? medial) : this(initial, medial, ' ') { }
        public Hangul(char? initial, char? medial, char final)
        {
            Initial = initial;
            Medial = medial;
            Final = final;
        }

        public static char Assemble(char initial, char medial)
        {
            return Assemble(initial, medial, ' ');
        }

        public static char Assemble(char initial, char medial, char final)
        {
            int initialIndex, medialIndex, finalIndex;
            int unicode;

            initialIndex = Array.IndexOf(InitialTable, initial);
            medialIndex = Array.IndexOf(MedialTable, medial);
            finalIndex = Array.IndexOf(FinalTable, final);

            if (initialIndex == -1 || medialIndex == -1 || finalIndex == -1)
            {
                if (initialIndex != -1)
                    return InitialTable[initialIndex];
                else if (medialIndex != -1)
                    return MedialTable[medialIndex];
                else if (finalIndex != -1)
                    return FinalTable[finalIndex];
                else
                    return '\0';
            }
            else
                unicode = UnicodeHangulSyllablesFirst + (initialIndex * 21 + medialIndex) * 28 + finalIndex;

            return Convert.ToChar(unicode);
        }

        public static Hangul Disassemble(char c)
        {
            int initialIndex, medialIndex, finalIndex;
            int unicode = Convert.ToUInt16(c);

            if ((unicode < UnicodeHangulSyllablesFirst) || (unicode > UnicodeHangulSyllablesLast))
            {
                if (unicode >= UnicodeHangulCompatibilityJamoFirst && unicode <= UnicodeHangulCompatibilityJamoLast)
                {
                    if (InitialTable.Contains(c))
                        return new Hangul(c);
                    else if (MedialTable.Contains(c))
                        return new Hangul(null, c);
                    else if (FinalTable.Contains(c))
                        return new Hangul(null, null, c);
                }
                return new Hangul(null);
            }

            unicode -= UnicodeHangulSyllablesFirst;

            initialIndex = unicode / (21 * 28);
            unicode %= 21 * 28;
            medialIndex = unicode / 28;
            unicode %= 28;
            finalIndex = unicode;

            return new Hangul(InitialTable[initialIndex], MedialTable[medialIndex], FinalTable[finalIndex]);
        }

        public static bool IsHangul(char c)
        {
            if (c >= UnicodeHangulSyllablesFirst && c <= UnicodeHangulSyllablesLast || c >= UnicodeHangulCompatibilityJamoFirst && c <= UnicodeHangulCompatibilityJamoLast)
                return true;
            else
                return false;
        }

        public static bool EndsWithFinal(string input)
        {
            var lastCharacter = input[input.Length - 1];
            return Hangul.Disassemble(lastCharacter).Final != ' ';
        }

        public char? GetFirstCharacter() => Initial ?? Medial ?? Final;
        public override string ToString() => $"{Initial} {Medial} {Final}";

    }
}
