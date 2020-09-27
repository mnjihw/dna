using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace 리듬_DC_콘솔_봇
{
    public static class OrthographyChecker
    {

        private static readonly List<(string wrong, string right, int explanationIndex)> certainList = new List<(string, string, int)>();
        private static readonly List<(string wrong, string right, int explanationIndex, Predicate<(string input, string wrong)> condition)> uncertainList = new List<(string, string, int, Predicate<(string, string)>)>();
        private static readonly List<(string wrong, int explanationIndex, Predicate<(string input, string wrong)> condition)> uncertainList2 = new List<(string wrong, int explanationIndex, Predicate<(string input, string wrong)> condition)>();
        private static readonly Dictionary<int, string> explanations = new Dictionary<int, string>
        {
            [0] = "되어 = 돼임",
            [1] = "받침 있으면 이에요 없으면 예요 이로 끝나면 받침 없으므로 예요 요 떼서 말 되면 에요 아니에요는 예외 인명엔 예요(예: 길동이예요) 성까지 쓰면 이에요(예: 홍길동이에요)",
            [2] = "엔간하다 [형용사] 대중으로 보아 정도가 표준에 꽤 가깝다.",
            [3] = "왠지 빼고 웬임",
            [4] = "무조건 며칠이 맞음",
            [5] = "함부로 [부사] 조심하거나 깊이 생각하지 아니하고 마음 내키는 대로 마구.",
            [6] = "금세 [부사] 지금 바로.금시에가 줄어든 말로 구어체에서 많이 사용된다.",
            [7] = "요새 [명사] ‘요사이(이제까지의 매우 짧은 동안)’의 준말.",
            [8] = "뵈어=봬임",
            [9] = "틈틈이 간간이 누누이 곰곰이 일일이 짬짬이 번번이 깨끗이 깊숙이 등등...",
            [10] = "기본형이 붙히다가 아니고 붙이다임",
            [11] = "아니었 아니에요 아니어도 아니어서는 예외",
            [12] = "얻다: ‘어디에다’가 줄어든 말.",
            [13] = "오랜만 [명사] 오래간만(어떤 일이 있은 때로부터 긴 시간이 지난 뒤)의 준말.",
            [14] = "역할 (役割)[명사]  자기가 마땅히 하여야 할 맡은 바 직책이나 임무.",
            [15] = "굳이가 구개음화돼서 구지로 발음됨",
            [16] = "어떻게 해 = 어떡해임",
            [17] = "-ㄹ는지: 막연한 의문이나 의심을 나타낼 때 사용하는 어미.",
            [18] = "개수 (個數)[명사] 한 개씩 낱으로 셀 수 있는 물건의 수효.",
            [19] = "녹록지 넉넉지 익숙지 섭섭지 등등...",
            [20] = "계시다는 동사라 형용사 뒤에 쓰이는 어미 -ㄴ가(요)를 붙인 계신가요는 적절하지 않고, 주로 동사 뒤에 쓰이는 어미 -나(요)를 붙인 계시나요가 적절함",
            [21] = "지껄이다 [동사] 1. 약간 큰 소리로 떠들썩하게 이야기하다. 2. ‘말하다’를 낮잡아 이르는 말.",
            [22] = "짜깁기 [명사] 기존의 글이나 영화 따위를 편집하여 하나의 완성품으로 만드는 일.",
            [23] = "곁땀 [명사] 겨드랑이에서 나는 땀.",
            [24] = "들입다 [부사] 세차게 마구",
            [25] = "해님 [명사] 해를 인격화하여 높이거나 다정하게 이르는 말.",
            [26] = "고데기: 미용실에서 웨이브를 연출할 때 사용하는 둥근 형태로 된 기계.",
            [27] = "헤매다 [동사] 갈 바를 몰라 이리저리 돌아다니다.",
            [28] = "돌멩이 [명사] 돌덩이보다 작은 돌.",
            [29] = "알맹이 [명사] 1. 물건의 껍데기나 껍질을 벗기고 남은 속 부분.  2. 사물의 핵심이 되는 중요한 부분.",
            [30] = "눈곱 [명사] 눈에서 나오는 진득진득한 액.또는 그것이 말라붙은 것.",
            [31] = "설거지 [명사] 먹고 난 뒤의 그릇을 씻어 정리하는 일.",
            [32] = "희한하다 [형용사] 매우 드물거나 신기하다.",
            [33] = "도긴개긴 [명사] 조금 낫고 못한 정도의 차이는 있으나 본질적으로는 비슷비슷하여 견주어 볼 필요가 없음을 이르는 말.",
            [34] = "십상 (十常)[명사] 같은 말: 십상팔구(열에 여덟이나 아홉 정도로 거의 예외가 없음.)",
            [35] = "이중 피동",
            [36] = "tutorial: 1. (대학의)개별 지도 시간   2. 사용 지침서, (컴퓨터의)사용 지침 프로그램",
            [37] = "순댓국 [명사] 돼지를 삶은 국물에 순대를 넣고 끓인 국.",
            [38] = "만둣국 [명사] 만두를 넣고 끓인 국.",
            [39] = "대가 (代價) [명사] 1. 같은 말: 대금5(代金)(물건의 값으로 치르는 돈). 2. 일을 하고 그에 대한 값으로 받는 보수.",
            [40] = "이점 (利點) [명사] 이로운 점.",
            [41] = "나잇값 [명사] 나이에 어울리는 말과 행동을 낮잡아 이르는 말.",
            [42] = "죗값 [명사] 지은 죄에 대하여 치르는 대가.",
            [43] = "공깃밥 [명사] 공기에 담은 밥.",
            [44] = "받침 있으면 률 없거나 ㄴ받침이면 율",
            [45] = "상호가 배스킨라빈스 31임",
            [46] = "카디건 (cardigan)[명사] 털로 짠 스웨터의 하나.",
            [47] = "훼손 (毁損)[명사] 1. 체면이나 명예를 손상함. 2. 헐거나 깨뜨려 못 쓰게 만듦.",
            [48] = "폐쇄 (閉鎖)[명사] 1. 문 따위를 닫아걸거나 막아 버림. 2. 기관이나 시설을 없애거나 기능을 정지함.",
            [49] = "급처: 게임에서 아이템을 팔 때 쓰는 말. 급하게 처분의 줄임말.",
            [50] = "얼마큼: 얼마만큼이 줄어든 말.",
            [51] = "대갚음 [명사] 남에게 입은 은혜나 남에게 당한 원한을 잊지 않고 그대로 갚음.",
            [52] = "구레나룻 [명사] 귀밑에서 턱까지 잇따라 난 수염.",
            [53] = "저녁 [명사] 해가 질 무렵부터 밤이 되기까지의 사이.",
            [54] = "반대말 [명사] 같은 말: 반의어(그 뜻이 서로 정반대되는 관계에 있는 말).",
            [55] = "파투 [명사] 일이 잘못되어 흐지부지됨을 비유적으로 이르는 말.",
            [56] = "어쭙잖다 [형용사] 비웃음을 살 만큼 언행이 분수에 넘치는 데가 있다.",
            [57] = "뒤치다꺼리 [명사]  뒤에서 일을 보살펴서 도와주는 일.",
            [58] = "씨불이다 [동사] 주책없이 함부로 실없는 말을 하다.",
            [59] = "지르밟다 [동사] 위에서 내리눌러 밟다.",
            [60] = "아등바등 [부사] 무엇을 이루려고 애를 쓰거나 우겨 대는 모양.",
            [61] = "장롱 [명사] 옷 따위를 넣어 두는 장과 농을 아울러 이르는 말.",
            [62] = "철석같이 [부사] 마음이나 의지, 약속 따위가 매우 굳고 단단하게.",
            [63] = "어리바리 [부사] 정신이 또렷하지 못하거나 기운이 없어 몸을 제대로 놀리지 못하고 있는 모양.",
            [64] = "매가리 [명사] 맥(기운이나 힘)(脈)을 낮잡아 이르는 말.",
            [65] = " 조사라서 무조건 붙여 써야 함",
            [66] = "외래어는 뒤 초성이 거센소리나 된소리일 때 앞 받침을 안 적는 게 원칙. 쉽게 말하자면 받침 유무에 따라 발음 차이가 없으면 받침 안 씀",
            [67] = "적재, 등재, 연재의 그 재임",
            [68] = "외래어표기법 제 3장 제 6항 유음([l]) - 어중의 [l]이 모음 앞에 오면 ㄹㄹ로 적는다. 한국 롤렉스 홈피에도 롤렉스로 표기함.",
            [69] = "Dayjust가 아닌 Datejust임",
            [70] = "도대체 (都大體)[부사] 1. 다른 말은 그만두고 요점만 말하자면. 2. 유감스럽게도 전혀.",
            [71] = "오토매틱 [예문] 오토매틱의 automatic",
            [72] = "별의별 (別-別)[관형사] 보통과 다른 갖가지의.",
            [73] = "담구다가 아니고 담그다임",
            [74] = "Tangente[taŋ|gεntә]로 탕겐테가 맞음",
            [75] = "대리자라는 뜻의 명사로 쓸 때의 발음은 델리깃임",
            [76] = "재작년 (再昨年)[명사] 지난해의 바로 전 해.",
            [77] = "불리다 [동사] 부르다(말이나 행동 따위로 다른 사람의 주의를 끌거나 오라고 하다.)의 피동사.",
            [78] = "모자라다 [동사] 기준이 되는 양이나 정도에 미치지 못하다.",
            [79] = "건드리다 [동사] 조금 움직일 만큼 손으로 만지거나 무엇으로 대다.",
            [80] = "그렇다가 형용사라 그런가요가 맞음",
            [81] = "짚이다 [동사] 헤아려 본 결과 어떠할 것으로 짐작이 가다.",
            [82] = "높이다 [동사] 높다의 사동사.",
            [83] = "삼가다 [동사] 1. 몸가짐이나 언행을 조심하다. 2.꺼리는 마음으로 양(量)이나 횟수가 지나치지 아니하도록 하다.",
            [84] = "횡격막 (橫膈膜/橫隔膜)[명사] [같은 말] 가로막(배와 가슴 사이를 분리하는 근육).",
            [85] = "예컨대 단언컨대 생각건대 하건대 등등..",
            [86] = "맡기다 [동사] 맡다(어떤 일에 대한 책임을 지고 담당하다)의 사동사.",
            [87] = "유도신문 (誘導訊問): 증인을 신문하는 사람이 희망하는 답변을 암시하면서, 증인이 무의식중에 원하는 대답을 하도록 꾀어 묻는 일.",
            [88] = "뒤 초성이 된소리나 거센소리일 때는 사이시옷을 안 적는 게 원칙임",
            [89] = "홍채 [명사] 안구의 각막과 수정체 사이에 있는 둥근 모양의 얇은 막.",
            [90] = "이요는 연결 어미라 쉼표 앞이나 요 떼서 말 될 때만 올 수 있고 요는 해요체 이오는 하오체라 일상에서 쓸 때는 이오보단 요가 적절함",
            [91] = "받침 있으면 이었 없으면 였 이로 끝나면 받침 없으므로 였 아니었은 예외 인명엔 였(예: 길동이였다) 성까지 쓰면 이었다(예: 홍길동이었다)",
            [92] = "받침 있으면 이어도 없으면 여도 이로 끝나면 받침 없으므로 여도 아니어도는 예외 인명엔 여도(예: 길동이여도) 성까지 쓰면 이어도(예: 홍길동이어도)",
            [93] = "받침 있으면 이어서 없으면 여서 이로 끝나면 받침 없으므로 여서 아니어서는 예외 인명엔 여서(예: 길동이여서) 성까지 쓰면 이어서(예: 홍길동이어서)",
            [94] = "선택은 든 과거 회상은 던",
            [95] = "바라다 [동사] 1. 생각이나 바람대로 어떤 일이나 상태가 이루어지거나 그렇게 되었으면 하고 생각하다.",
            [96] = "지위, 신분, 자격은 로서(예: 친구로서 등) 재료, 수단, 도구, 시간을 셀 때 셈에 넣는 한계는 로써(예: 흙으로써, 함으로써, 오늘로써 등)",
            [97] = "애먼 [관형사] 1. 일의 결과가 다른 데로 돌아가 억울하게 느껴지는. 2. 일의 결과가 다른 데로 돌아가 엉뚱하게 느껴지는.",
            [98] = "응 아니의 높임 표현은 예 아니요임. 아니오는 그는 사람이 아니오. 와 같이 씀",
            [99] = "-다시피:  1.‘-는 바와 같이’의 뜻을 나타내는 연결 어미. 2. 어떤 동작에 가까움을 나타내는 연결 어미.",
            [100] = "걸맞다는 형용사라 동사 뒤에 쓰이는 어미 -는를 붙인 걸맞는은 적절하지 않고, 주로 형용사 뒤에 쓰이는 어미 -은을 붙인 걸맞은이 적절함",
            [101] = "태엽 (胎葉) [명사] 얇고 긴 강철 띠를 돌돌 말아 그 풀리는 힘으로 시계 따위를 움직이게 하는 장치.",
            [102] = "sh는 시로 표기함. 스킨십 멤버십 플래그십 플래시 등등...",
            [103] = "플래카드 (placard)[명사] 긴 천에 표어 따위를 적어 양쪽을 장대에 매어 높이 들거나 길 위에 달아 놓은 표지물.",
            [104] = "난센스 (nonsense)[명사] 이치에 맞지 아니하거나 평범하지 아니한 말 또는 일.",
            [105] = "발음기호가 [ʃӕ]이면 섀 [ʃe]이면 셰. 쉐는 아예 틀림",
            [106] = "swag[swӕg]이므로 스왜그가 맞고 굳이 줄이자면 스왝이 맞음",
            [107] = "swagger[|swӕgə(r)]이므로 스왜거가 맞음",
            [108] = "담뱃값 [명사] 1. 담배의 가격. 2. 담배를 살 돈. 3. 약간의 돈을 비유적으로 이르는 말",
            [109] = "노랑은 노란 빛깔이라는 뜻으로 그 뒤에 색을 붙이지 않음",
            [110] = "파랑은 파란 빛깔이라는 뜻으로 그 뒤에 색을 붙이지 않음",
            [111] = "빨강은 빨간 빛깔이라는 뜻으로 그 뒤에 색을 붙이지 않음",
            [112] = "욱여넣다 [동사] 주위에서 중심으로 함부로 밀어 넣다.",
            [113] = "뒤처지다 [동사] 어떤 수준이나 대열에 들지 못하고 뒤로 처지거나 남게 되다.",


        };
        private static readonly StringBuilder sb = new StringBuilder(512);

        static OrthographyChecker()
        {
            certainList.Add(("되요", "돼요", 0));
            certainList.Add(("되여", "되여", 0));
            certainList.Add(("되욤", "돼욤", 0));
            certainList.Add(("되염", "돼염", 0));
            certainList.Add(("되용", "돼용", 0));
            certainList.Add(("되영", "돼영", 0));
            certainList.Add(("되욧", "돼욧", 0));
            certainList.Add(("되엿", "돼엿", 0));
            certainList.Add(("되양", "돼양", 0));
            certainList.Add(("되얌", "돼얌", 0));
            certainList.Add(("되야", "돼야", 0));
            certainList.Add(("되서", "돼서", 0));
            certainList.Add(("되봐", "돼봐", 0));
            certainList.Add(("됬", "됐", 0));
            certainList.Add(("되간다", "돼간다", 0));
            certainList.Add(("되가네", "돼가네", 0));
            certainList.Add(("되갈", "돼갈", 0));
            certainList.Add(("되왔", "돼왔", 0));
            certainList.Add(("되잇", "돼있", 0));
            certainList.Add(("되있", "돼있", 0));
            certainList.Add(("되 있", "돼 있", 0));
            certainList.Add(("되먹", "돼먹", 0));
            certainList.Add(("되버림", "돼버림", 0));
            certainList.Add(("되버린", "돼버린", 0));
            certainList.Add(("되버려", "돼버려", 0));
            certainList.Add(("되버렷", "돼버렸", 0));
            certainList.Add(("되버렸", "돼버렸", 0));
            certainList.Add(("되버리는", "돼버리는", 0));
            certainList.Add(("되버리고", "돼버리고", 0));
            certainList.Add(("되버리지", "돼버리지", 0));
            certainList.Add(("되보임", "돼보임", 0));
            certainList.Add(("되보여", "돼보여", 0));
            certainList.Add(("되보이", "돼보이", 0));
            certainList.Add(("되보일", "돼보일", 0));
            certainList.Add(("되보인다", "돼보인다", 0));
            certainList.Add(("되보엿", "돼보였", 0));
            certainList.Add(("되보였", "돼보였", 0));
            certainList.Add(("되보여야", "돼보여야", 0));
            certainList.Add(("됌", "됨", 0));
            certainList.Add(("됄", "될", 0));
            certainList.Add(("됀", "된", 0));
            certainList.Add(("돼는데", "되는데", 0));
            certainList.Add(("뭐에요", "뭐예요", 1));
            certainList.Add(("뭐에여", "뭐예여", 1));
            certainList.Add(("뭐에염", "뭐예염", 1));
            certainList.Add(("뭐에야", "뭐예야", 1));
            certainList.Add(("뭐에얌", "뭐예얌", 1));
            certainList.Add(("뭐에욧", "뭐예욧", 1));
            certainList.Add(("뭐에엿", "뭐예엿", 1));
            certainList.Add(("머에요", "머예요", 1));
            certainList.Add(("머에여", "머예여", 1));
            certainList.Add(("머에염", "머예염", 1));
            certainList.Add(("머에야", "머예야", 1));
            certainList.Add(("머에얌", "머예얌", 1));
            certainList.Add(("머에욧", "머예욧", 1));
            certainList.Add(("머에엿", "머예엿", 1));
            certainList.Add(("거에요", "거예요", 1));
            certainList.Add(("거에여", "거예여", 1));
            certainList.Add(("거에염", "거예염", 1));
            certainList.Add(("거에야", "거예야", 1));
            certainList.Add(("거에얌", "거예얌", 1));
            certainList.Add(("거에욧", "거예욧", 1));
            certainList.Add(("거에엿", "거예엿", 1));
            certainList.Add(("거에욬", "거예욬", 1));
            certainList.Add(("거에옄", "거예옄", 1));
            certainList.Add(("꺼에요", "꺼예요", 1));
            certainList.Add(("꺼에여", "꺼예여", 1));
            certainList.Add(("꺼에염", "꺼예염", 1));
            certainList.Add(("꺼에야", "꺼예야", 1));
            certainList.Add(("꺼에얌", "꺼예얌", 1));
            certainList.Add(("꺼에욧", "꺼예욧", 1));
            certainList.Add(("꺼에엿", "꺼예엿", 1));
            certainList.Add(("꺼에욬", "꺼예욬", 1));
            certainList.Add(("꺼에옄", "꺼예옄", 1));
            certainList.Add(("앵간", "엔간", 2));
            certainList.Add(("왠만", "웬만", 3));
            certainList.Add(("왠일", "웬일", 3));
            certainList.Add(("함부러", "함부로", 5));
            certainList.Add(("뵈요", "봬요", 8));
            certainList.Add(("뵈여", "봬여", 8));
            certainList.Add(("뵈염", "봬염", 8));
            certainList.Add(("뵜", "뵀", 8));
            certainList.Add(("뵈서", "봬서", 8));
            certainList.Add(("이래뵈도", "이래 봬도", 8));
            certainList.Add(("이래 뵈도", "이래 봬도", 8));
            certainList.Add(("틈틈히", "틈틈이", 9));
            certainList.Add(("간간히", "간간이", 9));
            certainList.Add(("곰곰히", "곰곰이", 9));
            certainList.Add(("일일히", "일일이", 9));
            certainList.Add(("번번히", "번번이", 9));
            certainList.Add(("짬짬히", "짬짬이", 9));
            certainList.Add(("깨끗히", "깨끗이", 9));
            certainList.Add(("깊숙히", "깊숙이", 9));
            certainList.Add(("붙히", "붙이", 10));
            certainList.Add(("붙힘", "붙임", 10));
            certainList.Add(("붙혔", "붙였", 10));
            certainList.Add(("붙혀", "붙여", 10));
            certainList.Add(("붙힐", "붙일", 10));
            certainList.Add(("아니엿", "아니었", 11));
            certainList.Add(("아니였", "아니었", 11));
            certainList.Add(("아니예요", "아니에요", 11));
            certainList.Add(("아니여도", "아니어도", 11));
            certainList.Add(("오랫만", "오랜만", 13));
            certainList.Add(("역활", "역할", 14));
            certainList.Add(("궂이", "굳이", 15));
            certainList.Add(("궃이", "굳이", 15));
            certainList.Add(("어떻함", "어떡함", 16));
            certainList.Add(("어떻해", "어떡해", 16));
            certainList.Add(("할런지", "할는지", 17));
            certainList.Add(("될런지", "될는지", 17));
            certainList.Add(("볼런지", "볼는지", 17));
            certainList.Add(("을런지", "을는지", 17));
            certainList.Add(("보일런지", "보일는지", 17));
            certainList.Add(("아닐런지", "아닐는지", 17));
            certainList.Add(("갯수", "개수", 18));
            certainList.Add(("녹록치", "녹록지", 19));
            certainList.Add(("넉넉치", "넉넉지", 19));
            certainList.Add(("익숙치", "익숙지", 19));
            certainList.Add(("섭섭치", "섭섭지", 19));
            certainList.Add(("계신가요", "계시나요", 20));
            certainList.Add(("계신가여", "계시나여", 20));
            certainList.Add(("계신가욤", "계시나욤", 20));
            certainList.Add(("계신가염", "계시나염", 20));
            certainList.Add(("계신가용", "계시나용", 20));
            certainList.Add(("계신가영", "계시나영", 20));
            certainList.Add(("계시다면", "계신다면", 20));
            certainList.Add(("짓걸이네", "지껄이네", 21));
            certainList.Add(("짓거리네", "지껄이네", 21));
            certainList.Add(("짓껄이네", "지껄이네", 21));
            certainList.Add(("짓걸이지", "지껄이지", 21));
            certainList.Add(("짓거리지", "지껄이지", 21));
            certainList.Add(("짓껄이지", "지껄이지", 21));
            certainList.Add(("짓꺼리지", "지껄이지", 21));
            certainList.Add(("짓걸임", "지껄임", 21));
            certainList.Add(("짓거림", "지껄임", 21));
            certainList.Add(("짓껄임", "지껄임", 21));
            certainList.Add(("짓꺼림", "지껄임", 21));
            certainList.Add(("짜집기", "짜깁기", 22));
            certainList.Add(("겨땀", "곁땀", 23));
            certainList.Add(("드립다", "들입다", 24));
            certainList.Add(("햇님", "해님", 25));
            certainList.Add(("고대기", "고데기", 26));
            certainList.Add(("헤메다", "헤매다", 27));
            certainList.Add(("헤멤", "헤맴", 27));
            certainList.Add(("헤메도", "헤매도", 27));
            certainList.Add(("헤멨", "헤맸", 27));
            certainList.Add(("돌맹이", "돌멩이", 28));
            certainList.Add(("알멩이", "알맹이", 29));
            certainList.Add(("눈꼽", "눈곱", 30));
            certainList.Add(("설겆이", "설거지", 31));
            certainList.Add(("희안", "희한", 32));
            certainList.Add(("도찐개찐", "도긴개긴", 33));
            certainList.Add(("쉽상", "십상", 34));
            certainList.Add(("보여짐", "보임", 35));
            certainList.Add(("보여집", "보입", 35));
            certainList.Add(("보여질", "보일", 35));
            certainList.Add(("보여져", "보여", 35));
            certainList.Add(("보여졌", "보였", 35));
            certainList.Add(("보여진", "보인", 35));
            certainList.Add(("보여지지", "보이지", 35));
            certainList.Add(("보여지질", "보이질", 35));
            certainList.Add(("보여지는", "보이는", 35));
            certainList.Add(("보여지네", "보이네", 35));
            certainList.Add(("보여지고", "보이고", 35));
            certainList.Add(("잊혀집", "잊힙", 35));
            certainList.Add(("잊혀짐", "잊힘", 35));
            certainList.Add(("잊혀지지", "잊히지", 35));
            certainList.Add(("잊혀지질", "잊히질", 35));
            certainList.Add(("잊혀질", "잊힐", 35));
            certainList.Add(("잊혀져", "잊혀", 35));
            certainList.Add(("잊혀지는", "잊히는", 35));
            certainList.Add(("잊혀졌", "잊혔", 35));
            certainList.Add(("잊혀진", "잊힌", 35));
            certainList.Add(("믿겨집", "믿깁", 35));
            certainList.Add(("믿겨짐", "믿김", 35));
            certainList.Add(("믿겨질", "믿길", 35));
            certainList.Add(("믿겨져", "믿겨", 35));
            certainList.Add(("믿겨졌", "믿겼", 35));
            certainList.Add(("믿겨진", "믿긴", 35));
            certainList.Add(("믿겨지지", "믿기지", 35));
            certainList.Add(("믿겨지질", "믿기질", 35));
            certainList.Add(("믿겨지는", "믿기는", 35));
            certainList.Add(("믿겨지네", "믿기네", 35));
            certainList.Add(("짜여집", "짜입", 35));
            certainList.Add(("짜여짐", "짜임", 35));
            certainList.Add(("짜여지지", "짜이지", 35));
            certainList.Add(("짜여지질", "짜이질", 35));
            certainList.Add(("짜여질", "짜일", 35));
            certainList.Add(("짜여져", "짜여", 35));
            certainList.Add(("짜여지는", "짜이는", 35));
            certainList.Add(("짜여졌", "짜였", 35));
            certainList.Add(("짜여진", "짜인", 35));
            certainList.Add(("쓰여집", "쓰입", 35));
            certainList.Add(("쓰여짐", "쓰임", 35));
            certainList.Add(("쓰여질", "쓰일", 35));
            certainList.Add(("쓰여져", "쓰여", 35));
            certainList.Add(("쓰여졌", "쓰였", 35));
            certainList.Add(("쓰여진", "쓰인", 35));
            certainList.Add(("쓰여지지", "쓰이지", 35));
            certainList.Add(("쓰여지질", "쓰이질", 35));
            certainList.Add(("쓰여지는", "쓰이는", 35));
            certainList.Add(("쓰여지네", "쓰이네", 35));
            certainList.Add(("듀토리얼", "튜토리얼", 36));
            certainList.Add(("댓가", "대가", 39));
            certainList.Add(("잇점", "이점", 40));
            certainList.Add(("나이값", "나잇값", 41));
            certainList.Add(("죄값", "죗값", 42));
            certainList.Add(("공기밥", "공깃밥", 43));
            certainList.Add(("임신률", "임신율", 44));
            certainList.Add(("할인률", "할인율", 44));
            certainList.Add(("출산률", "출산율", 44));
            certainList.Add(("베스킨", "배스킨", 45));
            certainList.Add(("가디건", "카디건", 46));
            certainList.Add(("회손", "훼손", 47));
            certainList.Add(("급쳐", "급처", 49));
            certainList.Add(("얼만큼", "얼마큼", 50));
            certainList.Add(("되갚음", "대갚음", 51));
            certainList.Add(("구렛나루", "구레나룻", 52));
            certainList.Add(("저녘", "저녁", 53));
            certainList.Add(("반댓말", "반대말", 54));
            certainList.Add(("어줍잖", "어쭙잖", 56));
            certainList.Add(("뒤치닥거리", "뒤치다꺼리", 57));
            certainList.Add(("씨부림", "씨불임", 58));
            certainList.Add(("시부림", "씨불임", 58));
            certainList.Add(("씨부리지", "씨불이지", 58));
            certainList.Add(("시부리지", "씨불이지", 58));
            certainList.Add(("씨부리질", "씨불이질", 58));
            certainList.Add(("시부리질", "씨불이질", 58));
            certainList.Add(("씨부릴", "씨불일", 58));
            certainList.Add(("시부릴", "씨불일", 58));
            certainList.Add(("씨부려", "씨불여", 58));
            certainList.Add(("시부려", "씨불여", 58));
            certainList.Add(("씨부리는", "씨불이는", 58));
            certainList.Add(("시부리는", "씨불이는", 58));
            certainList.Add(("씨부렷", "씨불였", 58));
            certainList.Add(("시부렷", "씨불였", 58));
            certainList.Add(("씨부렸", "씨불였", 58));
            certainList.Add(("시부렸", "씨불였", 58));
            certainList.Add(("씨부린", "씨불인", 58));
            certainList.Add(("씨부리면", "씨불이면", 58));
            certainList.Add(("시부리면", "씨불이면", 58));
            certainList.Add(("즈려밟", "지르밟", 59));
            certainList.Add(("아둥바둥", "아등바등", 60));
            certainList.Add(("장농", "장롱", 61));
            certainList.Add(("철썩같이", "철석같이", 62));
            certainList.Add(("어리버리", "어리바리", 63));
            certainList.Add(("맥아리", "매가리", 64));
            certainList.Add(("셋트", "세트", 66));
            certainList.Add(("셋팅", "세팅", 66));
            certainList.Add(("티켓팅", "티케팅", 66));
            certainList.Add(("랩퍼", "래퍼", 66));
            certainList.Add(("랩핑", "래핑", 66));
            certainList.Add(("마켓팅", "마케팅", 66));
            certainList.Add(("컷팅", "커팅", 66));
            certainList.Add(("컷트", "커트", 66));
            certainList.Add(("컷터", "커터", 66));
            certainList.Add(("탑제", "탑재", 67));
            certainList.Add(("로렉스", "롤렉스", 68));
            certainList.Add(("데이저스트", "데이트저스트(데잇저스트)", 69));
            certainList.Add(("데이져스트", "데이트저스트(데잇저스트)", 69));
            certainList.Add(("도데체", "도대체", 70));
            certainList.Add(("오토메틱", "오토매틱", 71));
            certainList.Add(("별에별", "별의별", 72));
            certainList.Add(("담궈", "담가", 73));
            certainList.Add(("탕켄테", "탕겐테", 74));
            certainList.Add(("델리게이트", "델리깃", 75));
            certainList.Add(("제작년", "재작년", 76));
            certainList.Add(("불리움", "불림", 77));
            certainList.Add(("불리우는", "불리는", 77));
            certainList.Add(("불리운", "불린", 77));
            certainList.Add(("불리우게", "불리게", 77));
            certainList.Add(("불리어", "불려", 77));
            certainList.Add(("모자름", "모자람", 78));
            certainList.Add(("모자른", "모자란", 78));
            certainList.Add(("건들여", "건드려", 79));
            certainList.Add(("건들임", "건드림", 79));
            certainList.Add(("건들이는", "건드리는", 79));
            certainList.Add(("건들이지", "건드리지", 79));
            certainList.Add(("그렇나요", "그런가요", 80));
            certainList.Add(("짚히는", "짚이는", 81));
            certainList.Add(("짚힘", "짚임", 81));
            certainList.Add(("짚혀", "짚여", 81));
            certainList.Add(("짚혔", "짚였", 81));
            certainList.Add(("높혀", "높여", 82));
            certainList.Add(("높히", "높이", 82));
            certainList.Add(("높혔", "높였", 82));
            certainList.Add(("높힌", "높인", 82));
            certainList.Add(("삼가해", "삼가", 83));
            certainList.Add(("횡경막", "횡격막", 84));
            certainList.Add(("예컨데", "예컨대", 85));
            certainList.Add(("단언컨데", "단언컨대", 85));
            certainList.Add(("생각컨데", "생각건대", 85));
            certainList.Add(("생각컨대", "생각건대", 85));
            certainList.Add(("하건데", "하건대", 85));
            certainList.Add(("맞기다", "맡기다", 86));
            certainList.Add(("맞김", "맡김", 86));
            certainList.Add(("맞기지", "맡기지", 86));
            certainList.Add(("맞겨", "맡겨", 86));
            certainList.Add(("맞겼", "맡겼", 86));
            certainList.Add(("유도심문", "유도신문", 87));
            certainList.Add(("홍체", "홍채", 89));
            certainList.Add(("음으로서", "음으로써", 96));
            certainList.Add(("함으로서", "함으로써", 96));
            certainList.Add(("됨으로서", "됨으로써", 96));
            certainList.Add(("봄으로서", "봄으로써", 96));
            certainList.Add(("짐으로서", "짐으로써", 96));
            certainList.Add(("싶이", "시피", 96));
            certainList.Add(("걸맞는", "걸맞은", 100));
            certainList.Add(("테엽", "태엽", 101));
            certainList.Add(("테옆", "태엽", 101));
            certainList.Add(("태옆", "태엽", 101));
            certainList.Add(("스킨쉽", "스킨십", 102));
            certainList.Add(("리더쉽", "리더십", 102));
            certainList.Add(("멤버쉽", "멤버십", 102));
            certainList.Add(("플래그쉽", "플래그십", 102));
            certainList.Add(("콩글리쉬", "콩글리시", 102));
            certainList.Add(("재패니쉬", "재패니시", 102));
            certainList.Add(("재플리쉬", "재플리시", 102));
            certainList.Add(("페티쉬", "페티시", 102));
            certainList.Add(("쉴드", "실드", 102));
            certainList.Add(("쉴더", "실더", 102));
            certainList.Add(("부쉬", "부시", 102));
            certainList.Add(("리쉬", "리시", 102));
            certainList.Add(("대쉬", "대시", 102));
            certainList.Add(("피쉬", "피시", 102));
            certainList.Add(("캐쉬", "캐시", 102));
            certainList.Add(("케쉬", "캐시", 102));
            certainList.Add(("푸쉬", "푸시", 102));
            certainList.Add(("쉬프트", "시프트", 102));
            certainList.Add(("쉬핑", "시핑", 102));
            certainList.Add(("쉽핑", "시핑", 102));
            certainList.Add(("플랜카드", "플래카드", 103));
            certainList.Add(("넌센스", "난센스", 104));
            certainList.Add(("쉐도우", "섀도", 105));
            certainList.Add(("쉐도", "섀도", 105));
            certainList.Add(("쉐어", "셰어", 105));
            certainList.Add(("쉐이크", "셰이크", 105));
            certainList.Add(("쉐이딩", "셰이딩", 105));
            certainList.Add(("쉐이드", "셰이드", 105));
            certainList.Add(("스웩", "스왝", 106));
            certainList.Add(("스웨그", "스왜그", 106));
            certainList.Add(("스웨거", "스왜거", 107));
            certainList.Add(("담배값", "담뱃값", 108));
            certainList.Add(("노랑색", "노란색(노랑)", 109));
            certainList.Add(("파랑색", "파란색(파랑)", 110));
            certainList.Add(("빨강색", "빨간색(빨강)", 111));
            certainList.Add(("우겨넣", "욱여넣", 112));
            certainList.Add(("뒤쳐지", "뒤처지", 113));
            certainList.Add(("뒤쳐질", "뒤처질", 113));
            certainList.Add(("뒤쳐져", "뒤처져", 113));
            certainList.Add(("뒤쳐졌", "뒤처졌", 113));


            uncertainList.Add(("되", "돼", 0, t => Regex.IsMatch(t.input, $@"{t.wrong}$", RegexOptions.Multiline)));
            uncertainList.Add(("되도", "돼도", 0, t => !(t.input.Contains($"{t.wrong}록") || Regex.IsMatch(t.input, $@"{t.wrong}\s*(않는|안한)"))));
            uncertainList.Add(("됫", "됐", 0, t => !t.input.Contains($"{t.wrong}박")));
            uncertainList.Add(("되라", "돼라", 0, t =>
            {
                string[] postfixes = { "고", "네", "니", "는", "능", "더", "던", "지", "하" };

                foreach(var postfix in postfixes)
                {
                    if (t.input.Contains($"{t.wrong}{postfix}"))
                        return false;
                }
                
                return true;
            }
            ));
            uncertainList.Add(("왠", "웬", 3, t => !(t.input.Contains($"오{t.wrong}") || t.input.Contains($"{t.wrong}지") || t.input.Contains($"{t.wrong}종일"))));
            uncertainList.Add(("웬지", "왠지", 3, t => t.input.Contains($"{t.wrong} ")));
            uncertainList.Add(("몇일", "며칠", 4, t => !t.input.Contains($"{t.wrong}까")));
            uncertainList.Add(("금새", "금세", 6, t => !t.input.Contains($"{t.wrong}로")));
            uncertainList.Add(("요세", "요새", 7, t => t.input.Contains($"{t.wrong} ")));
            uncertainList.Add(("어따", "얻다", 12, t => t.input.Contains($" {t.wrong}")));
            uncertainList.Add(("순대국", "순댓국", 37, t => !t.input.Contains($"{t.wrong}밥")));
            uncertainList.Add(("만두국", "만둣국", 38, t => !t.input.Contains($"{t.wrong}밥")));
            uncertainList.Add(("폐쇠", "폐쇄", 48, t => !Regex.IsMatch(t.input, $@"{t.wrong}\s*공포증")));
            uncertainList.Add(("파토", "파투", 55, t => !t.input.Contains($"레{t.wrong}리")));

            foreach (var (wrong, right) in new (string wrong, string right)[] { ("뒷", "뒤"), ("윗", "위"), ("아랫", "아래") })
            {
                uncertainList.Add((wrong, right, 88, t =>
                {
                    int index = t.input.IndexOf(t.wrong);
                    int length = t.input.Length;

                    if (index + wrong.Length != length)
                    {
                        Hangul h = Hangul.Disassemble(t.input[index + wrong.Length]);
                        if (h.Initial == 'ㅊ' || h.Initial == 'ㅉ')
                            return true;
                    }
                    return false;
                }
                ));
            }
            uncertainList2.Add((" 처럼", 65, t => true));
            uncertainList2.Add((" 입니다", 65, t => true));
            uncertainList2.Add((" 인데요", 65, t => true));
            uncertainList2.Add((" 인데수", 65, t => true));
            uncertainList2.Add((" 인데숭", 65, t => true));
            uncertainList2.Add((" 를", 65, t => true));
            uncertainList2.Add((" 한테", 65, t => true));
            uncertainList2.Add((" 부터", 65, t => true));
            uncertainList2.Add((" 커녕", 65, t => true));
            uncertainList2.Add((" 뿐", 65, t =>
            {
                var match = Regex.Match(t.input, $@"([가-힣])\s*{t.wrong}");
                if (match.Success)
                {
                    char c = match.Groups[1].Value[0];
                    Hangul h = Hangul.Disassemble(c);
                    if (!(c == '다' || h.Final == 'ㄹ'))
                        return true;
                }
                return false;
               
            }
            ));

        }

        public static string Check(string input)
        {
            sb.Clear();

            //int index = certainList.FindIndex(tuple => input.Contains(tuple.wrong));
            if(input == null)
                Console.WriteLine();
            foreach(var tuple in certainList)
            {
                if (input.Contains(tuple.wrong))
                {
                    var line = $"{tuple.wrong}->{tuple.right} [{explanations[tuple.explanationIndex]}]";

                    if(sb.Length + line.Length >= 400)                    
                        return sb.ToString();
                    sb.AppendLine(line);
                }
            }

            foreach (var tuple in uncertainList)
            {
                if (input.Contains(tuple.wrong) && tuple.condition((input, tuple.wrong)))
                {
                    var line = $"{tuple.wrong}->{tuple.right} [{explanations[tuple.explanationIndex]}]";

                    if (sb.Length + line.Length >= 400)
                        return sb.ToString();
                    sb.AppendLine(line);
                }
            }

            foreach(var tuple in uncertainList2)
            {
                if(input.Contains(tuple.wrong) && tuple.condition((input, tuple.wrong)))
                {
                    Regex regex = new Regex($@"(\S*)\s*{tuple.wrong}");
                    var matches = regex.Matches(input);
                    string word = default;

                    foreach(Match match in matches)
                    {
                        if(!string.IsNullOrEmpty(match.Groups[1].Value))
                        {
                            word = match.Groups[1].Value;
                            break;
                        }
                    }

                    var line = $"{word}{tuple.wrong}->{word}{tuple.wrong.TrimStart()} [{tuple.wrong.TrimStart()}{(Hangul.EndsWithFinal(tuple.wrong) ? "은" : "는")}{explanations[tuple.explanationIndex]}]";

                    if (sb.Length + line.Length >= 400)
                        return sb.ToString();
                    sb.AppendLine(line);
                }
            }

           
            return sb.ToString();

        }

        


    
    }
}
