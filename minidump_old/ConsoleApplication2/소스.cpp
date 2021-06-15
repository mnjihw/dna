#define _CRT_SECURE_NO_WARNINGS
#define WRITE_LOG
#include <io.h>
#include <stdio.h>
#include <Windows.h>
#include <winhttp.h>
#include "minidump.h"
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "winmm.lib")
 

void dbg(const char *fmt, ...);
void dump(const void *data, unsigned int size);
#import "mywinhttp.tlb" no_namespace named_guids
IManagedInterface *csi;
char ci_t[64];
char nick[1000][25];
char comment[1000][201];
unsigned int comment_idx, upparsing = 2, list_idx, flip, flip2, overlapped;
const char gallid_list[][32] = {"programming", "pokemon", "nintendo", "iu_new", "twice", "produce101",
"baseball_new2", "lovelive", "drama_new", "etc_entertainment2", "d_fighter_new", "hanwhaeagles",
"game_classic", "giants_new1", "m_entertainer", "idolmaster", "stock_new1"};
FILE *log_fp;

typedef struct
{
	char wrongstring[32];
	char correctstring[32]; 
	char explanation[165];
}point_out_t;

#define PS_MAX 512

point_out_t ps[PS_MAX];
unsigned int ps_idx;


const wchar_t CHOSUNG_LIST[] =
{
	'ㄱ', 'ㄲ', 'ㄴ', 'ㄷ', 'ㄸ', 'ㄹ', 'ㅁ', 'ㅂ', 'ㅃ',
	'ㅅ', 'ㅆ', 'ㅇ' , 'ㅈ', 'ㅉ', 'ㅊ', 'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ'
};

const wchar_t JUNGSUNG_LIST[] =
{
	'ㅏ', 'ㅐ', 'ㅑ', 'ㅒ', 'ㅓ', 'ㅔ',
	'ㅕ', 'ㅖ', 'ㅗ', 'ㅘ', 'ㅙ', 'ㅚ',
	'ㅛ', 'ㅜ', 'ㅝ', 'ㅞ', 'ㅟ', 'ㅠ',
	'ㅡ', 'ㅢ', 'ㅣ'
};

const wchar_t JONGSUNG_LIST[] =
{
	' ', 'ㄱ', 'ㄲ', 'ㄳ', 'ㄴ', 'ㄵ', 'ㄶ', 'ㄷ',
	'ㄹ', 'ㄺ', 'ㄻ', 'ㄼ', 'ㄽ', 'ㄾ', 'ㄿ', 'ㅀ',
	'ㅁ', 'ㅂ', 'ㅄ', 'ㅅ', 'ㅆ', 'ㅇ', 'ㅈ', 'ㅊ',
	'ㅋ', 'ㅌ', 'ㅍ', 'ㅎ'
};

typedef struct
{
	int f;
	int m;
	int l;
}hangul_t;


void hangul_disassemble(const wchar_t wc, hangul_t *param)
{
	if (wc >= 0xAC00 && wc <= 0xD7A3)
	{
		int wc2 = (int)wc - 0xAC00;
		param->l = wc2 % 28;
		wc2 /= 28;
		param->m = wc2 % 21;
		param->f = wc2 / 21;
	}
	else
	{
		param->f = -1;
		param->m = -1;
		param->l = -1;
	}
}

wchar_t hangul_assemble(hangul_t param)
{
	return 0xAC00 + ((param.f * 21) + param.m) * 28 + param.l;
}

void ps_setup(const char *s1, const char *s2, const char *s3)
{
	if (ps_idx >= PS_MAX)
	{
		printf("PS_MAX 초과함!\n");
		dbg("PS_MAX 초과함!");
		return;
	}

	strcpy(ps[ps_idx].wrongstring, s1);
	strcpy(ps[ps_idx].correctstring, s2);
	strcpy(ps[ps_idx].explanation, s3);
	++ps_idx;
}




#define E1 "(되어 = 돼임)"
#define E2 "(받침 있으면 이에요 없으면 예요 이로 끝나면 받침 없으므로 예요 아니에요는 예외 인명엔 예요(예 : 길동이예요) 성까지 쓰면 이에요(예 : 홍길동이에요))"
#define E3 "(국어사전 - 엔간하다 [형용사] : 대중으로 보아 정도가 표준에 꽤 가깝다.)"
#define E4 "(왠지 빼고 웬임)"
#define E5 "(무조건 며칠이 맞음)"
#define E6 "(국어사전 - 함부로 [부사] : 조심하거나 깊이 생각하지 아니하고 마음 내키는 대로 마구.)"
#define E7 "(국어사전 - 금세 [부사] : 지금 바로.금시에가 줄어든 말로 구어체에서 많이 사용된다.)"
#define E8 "(국어사전 - 요새 [명사] :‘요사이(이제까지의 매우 짧은 동안)’의 준말.)"
#define E9 "(뵈어 = 봬임)"
#define E10 "(틈틈이 간간이 누누이 곰곰이 일일이 번번이 깨끗이 깊숙이 등등..)"
#define E11 "(기본형이 붙히다가 아니고 붙이다임)"
#define E12 "(아니었 아니에요 아니어도는 예외)"
#define E13 "(국어사전 - 얻다 : 어디에다’가 줄어든 말.)"
#define E14 "(국어사전 - 오랜만 [명사] :오래간만(어떤 일이 있은 때로부터 긴 시간이 지난 뒤)의 준말.)"
#define E15 "(역할 (役割)[명사] :  자기가 마땅히 하여야 할 맡은 바 직책이나 임무.)"
#define E16 "(굳이가 구개음화돼서 구지로 발음됨)"
#define E17 "(어떻게 해 = 어떡해임)"
#define E18 "(-ㄹ는지 : 막연한 의문이나 의심을 나타낼 때 사용하는 어미)"
#define E19 "(개수 (個數)[명사] : 한 개씩 낱으로 셀 수 있는 물건의 수효.)"
#define E20 "(녹록지 넉넉지 익숙지 섭섭지 등등..)"
#define E21 "(계시다는 동사라 형용사 뒤에 쓰이는 어미 -ㄴ가(요)를 붙인 계신가요는 적절하지 않고, 주로 동사 뒤에 쓰이는 어미 -나(요)를 붙인 계시나요가 적절함)"
#define E22 "(국어사전 - 지껄이다 [동사] : 1. 약간 큰 소리로 떠들썩하게 이야기하다. 2. 말하다’를 낮잡아 이르는 말.)"
#define E23 "(국어사전 - 짜깁기 [명사] : 기존의 글이나 영화 따위를 편집하여 하나의 완성품으로 만드는 일.)"
#define E24 "(국어사전 - 곁땀 [명사] : 겨드랑이에서 나는 땀.)"
#define E25 "(국어사전 - 들입다 [부사] : 세차게 마구.)"
#define E26 "(국어사전 - 해님 [명사] : 해를 인격화하여 높이거나 다정하게 이르는 말.)"
#define E27 "(국어사전 - 고데기 : 미용실에서 웨이브를 연출할 때 사용하는 둥근 형태로 된 기계.)"
#define E28 "(국어사전 - 헤매다 [동사] : 갈 바를 몰라 이리저리 돌아다니다.)"
#define E29 "(국어사전 - 돌멩이 [명사] : 돌덩이보다 작은 돌.)"
#define E30 "(국어사전 - 알맹이 [명사] : 1. 물건의 껍데기나 껍질을 벗기고 남은 속 부분.  2. 사물의 핵심이 되는 중요한 부분.)"
#define E31 "(국어사전 - 눈곱 [명사] : 눈에서 나오는 진득진득한 액.또는 그것이 말라붙은 것.)"
#define E32 "(국어사전 - 설거지 [명사] : 먹고 난 뒤의 그릇을 씻어 정리하는 일.)"
#define E33 "(국어사전 - 희한하다 [형용사] : 매우 드물거나 신기하다.)"
#define E34 "(국어사전 - 도긴개긴 [명사] : 조금 낫고 못한 정도의 차이는 있으나 본질적으로는 비슷비슷하여 견주어 볼 필요가 없음을 이르는 말.)"
#define E35 "(국어사전 - 십상 (十常)[명사] : 같은 말 : 십상팔구(열에 여덟이나 아홉 정도로 거의 예외가 없음.))"
#define E36 "(이중 피동)"
#define E37 "(영어사전 - tutorial : 1. (대학의)개별 지도 시간   2. 사용 지침서, (컴퓨터의)사용 지침 프로그램)"
#define E38 "(국어사전 - 순댓국 [명사] : 돼지를 삶은 국물에 순대를 넣고 끓인 국.)"
#define E39 "(국어사전 - 만둣국 [명사] : 만두를 넣고 끓인 국.)"
#define E40 "(국어사전 - 대가 (代價) [명사] : 1. 같은 말 : 대금5(代金)(물건의 값으로 치르는 돈). 2. 일을 하고 그에 대한 값으로 받는 보수.)"
#define E41 "(국어사전 - 이점 (利點) [명사] : 이로운 점.)"
#define E42 "(국어사전 - 나잇값 [명사] : 나이에 어울리는 말과 행동을 낮잡아 이르는 말.)"
#define E43 "(국어사전 - 죗값 [명사] : 지은 죄에 대하여 치르는 대가.)"
#define E44 "(국어사전 - 공깃밥 [명사] : 공기에 담은 밥.)"
#define E45 "(받침 있으면 률 없거나 ㄴ받침이면 율)"
#define E46 "(상호가 배스킨라빈스 31임)"
#define E47 "(국어사전 - 카디건 (cardigan)[명사] : 털로 짠 스웨터의 하나.)"
#define E48 "(국어사전 - 훼손 (毁損)[명사] : 1. 체면이나 명예를 손상함. 2. 헐거나 깨뜨려 못 쓰게 만듦.)"
#define E49 "(국어사전 - 폐쇄 (閉鎖)[명사] : 1. 문 따위를 닫아걸거나 막아 버림. 2. 기관이나 시설을 없애거나 기능을 정지함.)"
#define E50 "(국어사전 - 급처 : 게임에서 아이템을 팔 때 쓰는 말. 급하게 처분의 줄임말.)"
#define E51 "(국어사전 - 얼마큼 :얼마만큼이 줄어든 말.)"
#define E52 "(국어사전 - 대갚음 [명사] : 남에게 입은 은혜나 남에게 당한 원한을 잊지 않고 그대로 갚음.)"
#define E53 "(국어사전 - 구레나룻 [명사] : 귀밑에서 턱까지 잇따라 난 수염.)"
#define E54 "(국어사전 - 저녁 [명사] : 해가 질 무렵부터 밤이 되기까지의 사이.)"
#define E55 "(국어사전 - 반대말 [명사] : 같은 말 : 반의어(그 뜻이 서로 정반대되는 관계에 있는 말).)"
#define E56 "(국어사전 - 파투 [명사] : 일이 잘못되어 흐지부지됨을 비유적으로 이르는 말.)"
#define E57 "(국어사전 - 어쭙잖다 [형용사] : 비웃음을 살 만큼 언행이 분수에 넘치는 데가 있다.)"
#define E58 "(국어사전 - 뒤치다꺼리 [명사] :  뒤에서 일을 보살펴서 도와주는 일.)"
#define E59 "(국어사전 - 씨불이다 [동사] : 주책없이 함부로 실없는 말을 하다.)"
#define E60 "(국어사전 - 지르밟다 [동사] : 위에서 내리눌러 밟다.)"
#define E61 "(국어사전 - 아등바등 [부사] : 무엇을 이루려고 애를 쓰거나 우겨 대는 모양.)"
#define E62 "(국어사전 - 장롱 [명사] : 옷 따위를 넣어 두는 장과 농을 아울러 이르는 말.)"
#define E63 "(국어사전 - 철석같이 [부사] : 마음이나 의지, 약속 따위가 매우 굳고 단단하게.)"
#define E64 "(국어사전 - 어리바리 [부사] : 정신이 또렷하지 못하거나 기운이 없어 몸을 제대로 놀리지 못하고 있는 모양.)"
#define E65 "(국어사전 - 매가리 [명사] : 맥(기운이나 힘)(脈)을 낮잡아 이르는 말.)"
#define E66 "(빨강은 빨간 빛깔이라는 뜻으로 그 뒤에 색을 붙일 필요가 없음)"
#define E67 "(외래어는 뒤 초성이 거센소리나 된소리일 때 앞 받침을 안 적는 게 원칙. 쉽게 말하자면 받침 유무에 따라 발음 차이가 없으면 받침 안 씀)"
#define E68 "(navi 이므로 내비)"
#define E69 "(외래어표기법 제 3장 제 6항 유음([l]) - 어중의 [l]이 모음 앞에 오면 ㄹㄹ로 적는다. 한국 롤렉스 홈피에도 롤렉스로 표기함.)"
#define E70 "(DAYJUST가 아닌 DATEJUST임)"
#define E71 "(국어사전 - 도대체 (都大體)[부사] : 1. 다른 말은 그만두고 요점만 말하자면. 2. 유감스럽게도 전혀.)"
#define E72 "(영어사전 - 오토매틱 [예문] : 오토매틱의 automatic)"
#define E73 "(국어사전 - 별의별 (別-別)[관형사] : 보통과 다른 갖가지의.)"
#define E74 "(담구다가 아니고 담그다임)"
#define E75 "(Tangente [taŋ|gεnte]로 탕겐테임)"
#define E76 "(platinum이므로 플래티넘이 맞음. 늄은 nium으로 쓸 경우임)"
#define E77 "(국어사전 - 재작년 (再昨年)[명사] : 지난해의 바로 전 해.)"
#define E78 "(국어사전 - 불리다 [동사] : 부르다(말이나 행동 따위로 다른 사람의 주의를 끌거나 오라고 하다.)의 피동사.)"
#define E79 "(국어사전 - 모자라다 [동사] : 기준이 되는 양이나 정도에 미치지 못하다.)"
#define E80 "(국어사전 - 건드리다 [동사] : 조금 움직일 만큼 손으로 만지거나 무엇으로 대다.)"
#define E81 "(그렇다가 형용사라 그런가요가 맞음)"
#define E82 "(국어사전 - 짚이다 [동사] : 헤아려 본 결과 어떠할 것으로 짐작이 가다.)"
#define E83 "(국어사전 - 높이다 [동사] : 높다의 사동사.)"
#define E84 "(국어사전 - 삼가다 [동사] : 1. 몸가짐이나 언행을 조심하다. 2. 꺼리는 마음으로 양(量)이나 횟수가 지나치지 아니하도록 하다.)"
#define E85 "(국어사전 - 횡격막 (橫膈膜/橫隔膜)[명사] : [같은 말] 가로막(배와 가슴 사이를 분리하는 근육).)"
#define E86 "(예컨대 단언컨대 생각건대 하건대 등등..)"
#define E87 "(국어사전 - 맡기다 [동사] : 맡다(어떤 일에 대한 책임을 지고 담당하다)의 사동사.)"
#define E88 "(국어사전 - 유도신문 (誘導訊問)	 : 증인을 신문하는 사람이 희망하는 답변을 암시하면서, 증인이 무의식중에 원하는 대답을 하도록 꾀어 묻는 일.)"
#define E89 "(뒤 초성이 된소리나 거센소리일 때는 사이시옷을 안 적는 게 원칙임)"
#define E90 "(국어사전 - 홍채 [명사] : 안구의 각막과 수정체 사이에 있는 둥근 모양의 얇은 막.)"


void ps_init()
{
	ps_setup("되요", "돼요", E1);
	ps_setup("되여", "돼여", E1);
	ps_setup("되욤", "돼욤", E1);
	ps_setup("되염", "돼염", E1);
	ps_setup("되용", "돼용", E1);
	ps_setup("되영", "돼영", E1);
	ps_setup("되욧", "돼욧", E1);
	ps_setup("되엿", "돼엿", E1);
	ps_setup("되양", "돼양", E1);
	ps_setup("되얌", "돼얌", E1);
	ps_setup("되야", "돼야", E1);
	ps_setup("되서", "돼서", E1);
	ps_setup("되봐", "돼봐", E1);
	ps_setup("됫", "됐", E1);
	ps_setup("됬", "됐", E1);
	ps_setup("되가", "돼가", E1);
	ps_setup("되간다", "돼간다", E1);
	ps_setup("되갈", "돼갈", E1);
	ps_setup("되감", "돼감", E1);
	ps_setup("되와", "돼와", E1);
	ps_setup("되왔", "돼왔", E1);
	ps_setup("되온다", "돼온다", E1);
	ps_setup("되잇", "돼있", E1);
	ps_setup("되있", "돼있", E1);
	ps_setup("되버림", "돼버림", E1);
	ps_setup("되버린", "돼버린", E1);
	ps_setup("되버렷", "돼버렸", E1);
	ps_setup("되버렸", "돼버렸", E1);
	ps_setup("되버리는", "돼버리는", E1);
	ps_setup("되버리고", "돼버리고", E1);
	ps_setup("되버리지", "돼버리지", E1);
	ps_setup("되버려", "돼버려", E1);
	ps_setup("되보임", "돼보임", E1);
	ps_setup("되보여", "돼보여", E1);
	ps_setup("되보인다", "돼보인다", E1);
	ps_setup("되보여서", "돼보여서", E1);
	ps_setup("되보엿", "돼보였", E1);
	ps_setup("되보였", "돼보였", E1);
	ps_setup("되보이", "돼보이", E1);
	ps_setup("되보일", "돼보일", E1);
	ps_setup("되보여야", "돼보여야", E1);
	ps_setup("됌", "됨", E1);
	ps_setup("되도", "돼도", E1);
	ps_setup("되라", "돼라", E1);
	ps_setup("돼는데", "되는데", E1);
	ps_setup("뭐에요", "뭐예요", E2);
	ps_setup("뭐에여", "뭐예여", E2);
	ps_setup("뭐에염", "뭐예염", E2);
	ps_setup("뭐에야", "뭐예야", E2);
	ps_setup("뭐에얌", "뭐예얌", E2);
	ps_setup("뭐에욧", "뭐예욧", E2);
	ps_setup("뭐에엿", "뭐예엿", E2);
	ps_setup("머에요", "머예요", E2);
	ps_setup("머에여", "머예여", E2);
	ps_setup("머에염", "머예염", E2);
	ps_setup("머에야", "머예야", E2);
	ps_setup("머에얌", "머예얌", E2);
	ps_setup("머에욧", "머예욧", E2);
	ps_setup("머에엿", "머예엿", E2);
	ps_setup("거에요", "거예요", E2);
	ps_setup("거에여", "거예여", E2);
	ps_setup("거에염", "거예염", E2);
	ps_setup("거에야", "거예야", E2);
	ps_setup("거에얌", "거예얌", E2);
	ps_setup("거에욧", "거예욧", E2);
	ps_setup("거에엿", "거예엿", E2);
	ps_setup("거에욬", "거예욬", E2);
	ps_setup("거에옄", "거예옄", E2);
	ps_setup("꺼에요", "꺼예요", E2);
	ps_setup("꺼에여", "꺼예여", E2);
	ps_setup("꺼에염", "꺼예염", E2);
	ps_setup("꺼에야", "꺼예야", E2);
	ps_setup("꺼에얌", "꺼예얌", E2);
	ps_setup("꺼에욧", "꺼예욧", E2);
	ps_setup("꺼에엿", "꺼예엿", E2);
	ps_setup("꺼에욬", "꺼예욬", E2);
	ps_setup("꺼에옄", "꺼예옄", E2);
	ps_setup("앵간", "엔간", E3);
	ps_setup("왠만", "웬만", E4);
	ps_setup("왠일", "웬일", E4);
	ps_setup("왠", "웬", E4);
	ps_setup("웬지", "왠지", E4);
	ps_setup("몇일", "며칠", E5);
	ps_setup("함부러", "함부로", E6);
	ps_setup("금새", "금세", E7);
	ps_setup("요세", "요새", E8);
	ps_setup("뵈요", "봬요", E9);
	ps_setup("뵈여", "봬여", E9);
	ps_setup("뵈염", "봬염", E9);
	ps_setup("뵜", "뵀", E9);
	ps_setup("뵈서", "봬서", E9);
	ps_setup("틈틈히", "틈틈이", E10);
	ps_setup("간간히", "간간이", E10);
	ps_setup("누누히", "누누이", E10);
	ps_setup("곰곰히", "곰곰이", E10);
	ps_setup("일일히", "일일이", E10);
	ps_setup("번번히", "번번이", E10);
	ps_setup("깨끗히", "깨끗이", E10);
	ps_setup("깊숙히", "깊숙이", E10);
	ps_setup("붙혔", "붙였", E11);
	ps_setup("붙혀", "붙여", E11);
	ps_setup("아니엿", "아니었", E12);
	ps_setup("아니였", "아니었", E12);
	ps_setup("아니예요", "아니에요", E12);
	ps_setup("아니여도", "아니어도", E12);
	ps_setup("어따", "얻다", E13);
	ps_setup("오랫만", "오랜만", E14);
	ps_setup("역활", "역할", E15);
	ps_setup("궂이", "굳이", E16);
	ps_setup("궃이", "굳이", E16);
	ps_setup("어떻함", "어떡함", E17);
	ps_setup("어떻해", "어떡해", E17);
	ps_setup("할런지", "할는지", E18);
	ps_setup("될런지", "될는지", E18);
	ps_setup("보일런지", "보일는지", E18);
	ps_setup("갯수", "개수", E19);
	ps_setup("이래뵈도", "이래봬도", E9);
	ps_setup("녹록치", "녹록지", E20);
	ps_setup("넉넉치", "넉넉지", E20);
	ps_setup("익숙치", "익숙지", E20);
	ps_setup("섭섭치", "섭섭지", E20);
	ps_setup("계신가요", "계시나요", E21);
	ps_setup("계신가여", "계시나여", E21);
	ps_setup("계신가욤", "계시나욤", E21);
	ps_setup("계신가염", "계시나염", E21);
	ps_setup("계신가용", "계시나용", E21);
	ps_setup("계신가영", "계시나영", E21);
	ps_setup("짓걸이네", "지껄이네", E22);
	ps_setup("짓거리네", "지껄이네", E22);
	ps_setup("짓껄이네", "지껄이네", E22);
	ps_setup("짓꺼리네", "지껄이네", E22);
	ps_setup("짓걸이지", "지껄이지", E22);
	ps_setup("짓거리지", "지껄이지", E22);
	ps_setup("짓껄이지", "지껄이지", E22);
	ps_setup("짓꺼리지", "지껄이지", E22);
	ps_setup("짓걸임", "지껄임", E22);
	ps_setup("짓거림", "지껄임", E22);
	ps_setup("짓껄임", "지껄임", E22);
	ps_setup("짓꺼림", "지껄임", E22);
	ps_setup("짜집기", "짜깁기", E23);
	ps_setup("겨땀", "곁땀", E24);
	ps_setup("드립다", "들입다", E25);
	ps_setup("햇님", "해님", E26);
	ps_setup("고대기", "고데기", E27);
	ps_setup("헤메다", "헤매다", E28);
	ps_setup("헤멤", "헤맴", E28);
	ps_setup("헤메도", "헤매도", E28);
	ps_setup("헤멨", "헤맸", E28);
	ps_setup("돌맹이", "돌멩이", E29);
	ps_setup("알멩이", "알맹이", E30);
	ps_setup("눈꼽", "눈곱", E31);
	ps_setup("설겆이", "설거지", E32);
	ps_setup("희안", "희한", E33);
	ps_setup("도찐개찐", "도긴개긴", E34);
	ps_setup("쉽상", "십상", E35);
	ps_setup("보여짐", "보임", E36);
	ps_setup("보여질", "보일", E36);
	ps_setup("보여져", "보여", E36);
	ps_setup("보여졌", "보였", E36);
	ps_setup("보여진", "보인", E36);
	ps_setup("보여지지", "보이지", E36);
	ps_setup("보여지질", "보이질", E36);
	ps_setup("보여지는", "보이는", E36);
	ps_setup("보여지네", "보이네", E36);
	ps_setup("잊혀짐", "잊힘", E36);
	ps_setup("잊혀지지", "잊히지", E36);
	ps_setup("잊혀지질", "잊히질", E36);
	ps_setup("잊혀질", "잊힐", E36);
	ps_setup("잊혀져", "잊혀", E36);
	ps_setup("잊혀지는", "잊히는", E36);
	ps_setup("잊혀졌", "잊혔", E36);
	ps_setup("잊혀진", "잊힌", E36);
	ps_setup("믿겨짐", "믿김", E36);
	ps_setup("믿겨질", "믿길", E36);
	ps_setup("믿겨져", "믿겨", E36);
	ps_setup("믿겨졌", "믿겼", E36);
	ps_setup("믿겨진", "믿긴", E36);
	ps_setup("믿겨지지", "믿기지", E36);
	ps_setup("믿겨지질", "믿기질", E36);
	ps_setup("믿겨지는", "믿기는", E36);
	ps_setup("믿겨지네", "믿기네", E36);
	ps_setup("짜여짐", "짜임", E36);
	ps_setup("짜여지지", "짜이지", E36);
	ps_setup("짜여지질", "짜이질", E36);
	ps_setup("짜여질", "짜일", E36);
	ps_setup("짜여져", "짜여", E36);
	ps_setup("짜여지는", "짜이는", E36);
	ps_setup("짜여졌", "짜였", E36);
	ps_setup("짜여진", "짜인", E36);
	ps_setup("쓰여짐", "쓰임", E36);
	ps_setup("쓰여질", "쓰일", E36);
	ps_setup("쓰여져", "쓰여", E36);
	ps_setup("쓰여졌", "쓰였", E36);
	ps_setup("쓰여진", "쓰인", E36);
	ps_setup("쓰여지지", "쓰이지", E36);
	ps_setup("쓰여지질", "쓰이질", E36);
	ps_setup("쓰여지는", "쓰이는", E36);
	ps_setup("쓰여지네", "쓰이네", E36);
	ps_setup("듀토리얼", "튜토리얼", E37);
	ps_setup("순대국", "순댓국", E38);
	ps_setup("만두국", "만둣국", E39);
	ps_setup("댓가", "대가", E40);
	ps_setup("잇점", "이점", E41);
	ps_setup("나이값", "나잇값", E42);
	ps_setup("죄값", "죗값", E43);
	ps_setup("공기밥", "공깃밥", E44);
	ps_setup("임신률", "임신율", E45);
	ps_setup("할인률", "할인율", E45);
	ps_setup("베스킨", "배스킨", E46);
	ps_setup("가디건", "카디건", E47);
	ps_setup("회손", "훼손", E48);
	ps_setup("폐쇠", "폐쇄", E49);
	ps_setup("급쳐", "급처", E50);
	ps_setup("얼만큼", "얼마큼", E51);
	ps_setup("되갚음", "대갚음", E52);
	ps_setup("구렛나루", "구레나룻", E53);
	ps_setup("저녘", "저녁", E54);
	ps_setup("반댓말", "반대말", E55);
	ps_setup("파토", "파투", E56);
	ps_setup("는대", "는데", "");
	ps_setup("던대", "던데", "");
	ps_setup("어줍잖", "어쭙잖", E57);
	ps_setup("뒤치닥거리", "뒤치다꺼리", E58);
	ps_setup("씨부림", "씨불임", E59);
	ps_setup("시부림", "씨불임", E59);
	ps_setup("씨부리지", "씨불이지", E59);
	ps_setup("시부리지", "씨불이지", E59);
	ps_setup("씨부리질", "씨불이질", E59);
	ps_setup("시부리질", "씨불이질", E59);
	ps_setup("씨부릴", "씨불일", E59);
	ps_setup("시부릴", "씨불일", E59);
	ps_setup("씨부려", "씨불여", E59);
	ps_setup("시부려", "씨불여", E59);
	ps_setup("씨부리는", "씨불이는", E59);
	ps_setup("시부리는", "씨불이는", E59);
	ps_setup("씨부렷", "씨불였", E59);
	ps_setup("시부렷", "씨불였", E59);
	ps_setup("씨부렸", "씨불였", E59);
	ps_setup("시부렸", "씨불였", E59);
	ps_setup("씨부린", "씨불인", E59);
	ps_setup("씨부리면", "씨불이면", E59);
	ps_setup("시부리면", "씨불이면", E59);
	ps_setup("즈려밟", "지르밟", E60);
	ps_setup("아둥바둥", "아등바등", E61);
	ps_setup("장농", "장롱", E62);
	ps_setup("철썩같이", "철석같이", E63);
	ps_setup("어리버리", "어리바리", E64);
	ps_setup("맥아리", "매가리", E65);
	ps_setup("빨강색", "빨간색(빨강)", E66);
	ps_setup("셋트", "세트", E67);
	ps_setup("셋팅", "세팅", E67);
	ps_setup("티켓팅", "티케팅", E67);
	ps_setup("랩퍼", "래퍼", E67);
	ps_setup("랩핑", "래핑", E67);
	ps_setup("마켓팅", "마케팅", E67);
	ps_setup("네비", "내비", E68);
	ps_setup("로렉스", "롤렉스", E69);
	ps_setup("로렉", "롤렉", E69);
	ps_setup("데이저스트", "데이트저스트(데잇저스트)", E70);
	ps_setup("데이져스트", "데이트저스트(데잇저스트)", E70);
	ps_setup("도데체", "도대체", E71);
	ps_setup("오토메틱", "오토매틱", E72);
	ps_setup("별에별", "별의별", E73);
	ps_setup("담궈", "담가", E74);
	ps_setup("탕켄테", "탕겐테", E75);
	ps_setup("플래티늄", "플래티넘", E76);
	ps_setup("플레티늄", "플래티넘", E76);
	ps_setup("플레티넘", "플래티넘", E76);
	ps_setup("제작년", "재작년", E77);
	ps_setup("불리움", "불림", E78);
	ps_setup("불리운", "불린", E78);
	ps_setup("불리우게", "불리게", E78);
	ps_setup("불리어", "불려", E78);
	ps_setup("모자름", "모자람", E79);
	ps_setup("모자른", "모자란", E79);
	ps_setup("건들여", "건드려", E80);
	ps_setup("건들임", "건드림", E80);
	ps_setup("건들이는", "건드리는", E80);
	ps_setup("건들이지", "건드리지", E80);
	ps_setup("그렇나요", "그런가요", E81);
	ps_setup("짚히는", "짚이는", E82);
	ps_setup("짚힘", "짚임", E82);
	ps_setup("짚혀", "짚여", E82);
	ps_setup("짚혔", "짚였", E82);
	ps_setup("높혀", "높여", E83);
	ps_setup("높히", "높이", E83);
	ps_setup("높혔", "높였", E83);
	ps_setup("높힌", "높인", E83);
	ps_setup("삼가해", "삼가", E84);
	ps_setup("삼가하세요", "삼가세요", E84);
	ps_setup("삼가하셈", "삼가셈", E84);
	ps_setup("횡경막", "횡격막", E85);
	ps_setup("예컨데", "예컨대", E86);
	ps_setup("단언컨데", "단언컨대", E86);
	ps_setup("생각컨데", "생각건대", E86);
	ps_setup("생각컨대", "생각건대", E86);
	ps_setup("하건데", "하건대", E86);
	ps_setup("맞기다", "맡기다", E87);
	ps_setup("맞김", "맡김", E87);
	ps_setup("맞기지", "맡기지", E87);
	ps_setup("맞길", "맡길", E87);
	ps_setup("맞겨", "맡겨", E87);
	ps_setup("맞겼", "맡겼", E87);
	ps_setup("유도심문", "유도신문", E88);
	ps_setup("뒷", "뒤", E89);
	ps_setup("윗", "위", E89);
	ps_setup("아랫", "아래", E89);
	ps_setup("홍체", "홍채", E90);
}

bool IsImproperString(char *wrongstring, char *s)
{
	bool ret = false;
	hangul_t hangul_comb;
	wchar_t wbuf[16];

	if (strcmp(wrongstring, "왠") == 0 && (strncmp(s, "왠지", 4) == 0 || strncmp(s, "왠만", 4) == 0))
		ret = true;
	else if (strcmp(wrongstring, "로렉") == 0 && strncmp(s, "로렉스", 6) == 0)
		ret = true;
	else if (strcmp(wrongstring, "되도") == 0 && (s[4] != '<' && s[4] != ' ' || s[4] == ' ' && strncmp(&s[5], "않는", 4) == 0))
		ret = true;
	else if (strcmp(wrongstring, "되라") == 0 && (strncmp(&s[4], "는", 2) == 0 || strncmp(&s[4], "능", 2) == 0 || strncmp(&s[4], "고", 2) == 0 || strncmp(&s[4], "더", 2) == 0 || strncmp(&s[4], "던", 2) == 0 || strncmp(&s[4], "지", 2) == 0 || strncmp(&s[4], "네", 2) == 0 || strncmp(&s[4], "니", 2) == 0 || strncmp(&s[4], "하", 2) == 0))
		ret = true;
	else if (strcmp(wrongstring, "는대") == 0 && (strncmp(&s[4], "로", 2) == 0 || strncmp(&s[4], "신", 2) == 0 || strncmp(&s[4], "학", 2) == 0 || strncmp(&s[4], "?", 1) == 0))
		ret = true;
	else if (strcmp(wrongstring, "파토") == 0 && strncmp(&s[-2], "레파토리", 4) == 0)
		ret = true;
	else if (strcmp(wrongstring, "몇일") == 0 && (strncmp(&s[4], "까", 2) == 0 || strncmp(&s[4], "텐데", 4) == 0))
		ret = true;
	else if (strcmp(wrongstring, "뒷") == 0 || strcmp(wrongstring, "윗") == 0 || strcmp(wrongstring, "아랫") == 0)
	{
		//여기 if (str.Length > pos + wrongstring.Length) 이거 넣어야함
		
		if (strcmp(wrongstring, "아랫") == 0)
		{
			MultiByteToWideChar(CP_ACP, 0, s, 6, wbuf, 3);
			wbuf[3] = L'\0';

			hangul_disassemble(wbuf[2], &hangul_comb);
		}
		else
		{
			MultiByteToWideChar(CP_ACP, 0, s, 4, wbuf, 2);
			wbuf[2] = L'\0';

			hangul_disassemble(wbuf[1], &hangul_comb);
		}
		if (hangul_comb.f == -1 || hangul_comb.m == -1 || hangul_comb.l == -1)
			ret = true;

		if (CHOSUNG_LIST[hangul_comb.f] != 'ㅍ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅌ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅊ' &&
			CHOSUNG_LIST[hangul_comb.f] != 'ㄲ' && CHOSUNG_LIST[hangul_comb.f] != 'ㄸ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅃ' &&
			CHOSUNG_LIST[hangul_comb.f] != 'ㅆ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅉ' && CHOSUNG_LIST[hangul_comb.f] != 'ㅋ')
			ret = true; //뒤 초성이 된소리나 거센소리가 아니면 넘어감

	}
	else if (strcmp(wrongstring, "폐쇠") == 0 && strncmp(&s[4], "공포증", 6) == 0)
		ret = true;

	return ret;


}
typedef struct
{
	char *title;
	char *nick;
	char *content;
	unsigned int recentnum;
}article_t;

typedef struct
{
	unsigned int idx;
	char nick[25];
}pointed_out_t;







void dump(const void *data, unsigned int size)
{
	unsigned int i, j, k, z;
	char buffer[128];

	for (z = 0, i = 0; i < size; i += 16u) {
		k = 0;

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size) {
				for (; j < 16u; ++j) {
					*(unsigned int *)&buffer[k] = '    ';
					k += 3u;
				}
				break;
			}
			buffer[k + 0u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] >> 4];
			buffer[k + 1u] = "0123456789ABCDEF"[((unsigned char *)data)[i + j] & 15];
			buffer[k + 2u] = ' ';
			k += 3u;
		}

		for (j = 0; j < 16u; ++j) {
			if (i + j >= size)
				break;
			if (z == 0) {
				if (((unsigned char *)data)[i + j] > 31u && ((unsigned char *)data)[i + j] < 127u)
					buffer[k++] = ((unsigned char *)data)[i + j];
				/* FIXME: i + j + 1u가 size보다 큰 경우, 운이 아주 나쁘면 memory access violation 발생 */
				else if (((((unsigned char *)data)[i + j] >= 0xB0u && ((unsigned char *)data)[i + j] <= 0xC8u) && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu)) ||
					((((unsigned char *)data)[i + j] >= 0x81u && ((unsigned char *)data)[i + j] <= 0xA0u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) ||
					((((unsigned char *)data)[i + j] >= 0xA1u && ((unsigned char *)data)[i + j] <= 0xC5u) && ((((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x5Au) || (((unsigned char *)data)[i + j + 1u] >= 0x61u && ((unsigned char *)data)[i + j + 1u] <= 0x7Au) || (((unsigned char *)data)[i + j + 1u] >= 0x81u && ((unsigned char *)data)[i + j + 1u] <= 0xA0u))) ||
					(((unsigned char *)data)[i + j] == 0xC6u && (((unsigned char *)data)[i + j + 1u] >= 0x41u && ((unsigned char *)data)[i + j + 1u] <= 0x52u)) ||
					(((unsigned char *)data)[i + j] == 0xA4u && (((unsigned char *)data)[i + j + 1u] >= 0xA1u && ((unsigned char *)data)[i + j + 1u] <= 0xFEu))) {
					if (j != 15u) {
						*(unsigned short *)&buffer[k] = *(unsigned short *) &((unsigned char *)data)[i + j];
						k += 2u;
						j += 1u;
					}
					else {
						buffer[k++] = '.';
						z = 1;
					}
				}
				else
					buffer[k++] = '.';
			}
			else {
				buffer[k++] = '.';
				z = 0;
			}
		}


		OutputDebugString((*(unsigned int *)&buffer[k] = 0, buffer));

	}
}


void dbg(const char *fmt, ...)
{
	char msg[1024];

	OutputDebugString((wvsprintf(msg, fmt, (va_list)((unsigned int)&fmt + sizeof(void *))), msg));
}



void WinHttp_Open(const char *Method, const char *Url, int Async = 0)
{
	csi->Open(Method, Url, Async);
}

void WinHttp_Send(const char *Body = 0)
{
	csi->Send(Body);
}

void WinHttp_SetRequestHeader(const char *Header, const char *Value)
{
	csi->SetRequestHeader(Header, Value);
}

void WinHttp_ResponseBody(char *Body)
{
	wchar_t buf[1024 * 512];
	DWORD dwLen;

	__try
	{
		csi->ResponseBody(buf);

		dwLen = WideCharToMultiByte(CP_ACP, 0, buf, -1, 0, 0, 0, 0);
		WideCharToMultiByte(CP_ACP, 0, buf, -1, Body, dwLen, 0, 0);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

}



BOOL DCLogin(const char *id, const char *pw)
{
	char buf[1024 * 512];


	WinHttp_Open("POST", "https://dcid.dcinside.com/join/member_check.php");
	WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 5.1; rv:9.0.1) Gecko/20100101 Firefox/9.0.1");
	WinHttp_SetRequestHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
	WinHttp_SetRequestHeader("Referer", "http://www.dcinside.com/");
	WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded");
	wsprintf(buf, "s_url=http://dcinside.com/&ssl=Y&user_id=%s&password=%s", id, pw);
	WinHttp_Send(buf);

	WinHttp_ResponseBody(buf);

	if (strstr(buf, "등록된 아이디가 아닙니다.") || strstr(buf, "비밀번호가 틀렸습니다."))
		return FALSE;
	else if (strstr(buf, "로그인을 5번 실패 하셨습니다."))
	{
		printf("로그인을 5번 실패 하셨습니다. 내일 다시 이용해 주세요\n");
		return FALSE;
	}
	else
		return TRUE;
}


void Getci_t(const char *gallName)
{
	char buf[1024 * 512];
	char *s;

	wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=1", gallName);

$start:;
	WinHttp_Open("GET", buf);
	WinHttp_Send();
	WinHttp_ResponseBody(buf);

	s = strstr(buf, "ci_t\" value=\"");
	if (!s)
	{
		printf("%s\n", buf);
		Sleep(30000);
		goto $start;
	}
	s += strlen("ci_t\" value=\"");
	strncpy(ci_t, s, 32);

	ci_t[32] = '\0';

}


void WriteComment(const char *gallName, const char *articlenum, const char *nick, const char *pw, const char *comment)
{
	char buf[1024 * 512];


	WinHttp_Open("POST", "http://gall.dcinside.com/forms/comment_submit");
	wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=%s&page=1", gallName, articlenum);
	WinHttp_SetRequestHeader("Referer", buf);
	

	WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
	WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
	WinHttp_SetRequestHeader("X-Requested-With", "XMLHttpRequest");


	wsprintf(buf, "ci_t=%s&name=%s&password=%s&memo=%s&id=%s&no=%s&best_orgin=", ci_t, nick, pw, comment, gallName, articlenum);

	__try
	{
		WinHttp_Send(buf);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}

}



void _point_out(const char *gallName, const char *articlenum)
{
	unsigned int i, j, k, idx = 0;
	char *s;
	char buf[512], buf2[512];
	pointed_out_t pointed_out[80];
	


	for (i = 0; i < comment_idx; ++i)
	{
		if (strstr(comment[i], "[리듬 맞춤법 봇♬]"))
			continue;
		for (j = 0; j < ps_idx; ++j)
		{
			if (strstr(comment[i], ps[j].wrongstring))
			{
				for (k = 0; k < comment_idx; ++k)
				{
					if (strstr(comment[k], "[리듬 맞춤법 봇♬]"))
					{
						if (strncmp(ps[j].wrongstring, &comment[k][strlen(nick[i]) + 2], strlen(ps[j].wrongstring)) != 0)
							continue;
						if (strncmp(nick[i], comment[k], strlen(nick[i])) == 0)
						{
							k = 777;
							pointed_out[idx].idx = j;
							strcpy(pointed_out[idx++].nick, nick[i]);
						}
						if (k == 777)
							break;
								
					}
				}
			}
		}
	}

	for (i = 0; i < comment_idx; ++i)
	{
		if (strstr(comment[i], "[리듬 맞춤법 봇♬]"))
			continue;
		
		for (j = 0; j < ps_idx; ++j)
		{
			s = (char*)strstr(comment[i], ps[j].wrongstring);
			if (s)
			{
				if (IsImproperString(ps[j].wrongstring, s))
					continue;
				for (k = 0; k < idx; ++k)
				{
					if(j == pointed_out[k].idx && strcmp(pointed_out[k].nick, nick[i]) == 0)
					{
						k = 777;
						++overlapped;
						//printf("\r                           					\r이미 지적함 %d\n", overlapped);
						//dbg("이미 지적함 %d\n", overlapped);
						if (overlapped >= 10)
						{
							overlapped = 0;
							if (++list_idx == _countof(gallid_list))
								list_idx = 0;
							flip = 1;
							printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
							return;
						}
						break;
					}
				}
				if (k == 777)
					continue;
					
				wsprintf(buf, "%s//%s->%s %s [리듬 맞춤법 봇♬]", nick[i], ps[j].wrongstring, ps[j].correctstring, ps[j].explanation);
				if(strlen(buf) > 200)
					wsprintf(buf, "%s//%s->%s [리듬 맞춤법 봇♬]", nick[i], ps[j].wrongstring, ps[j].correctstring);
				WriteComment(gallName, articlenum, "ㅇㅇ", "0288", buf);
				wsprintf(buf2, "%s %s 에 %s이라고 씀", gallName, articlenum, buf);
				printf("\r                  \r%s\n", buf2);
				dbg("%s\n", buf2);
#ifdef WRITE_LOG
				fprintf(log_fp, "%s\n", buf2);
#endif
				pointed_out[idx].idx = j;
				strcpy(pointed_out[idx++].nick, nick[i]);
			}
		}
	}

}


unsigned int GetComments(const char *gallName, const char *articlenum)
{
	char buf[1024 * 512];

	char *s, *p;
	unsigned int i, j, currentPage = 1;

	
	__try
	{
		wsprintf(buf, "http://gall.dcinside.com/board/comment_view/?id=%s&no=%s&page=1", gallName, articlenum);
		WinHttp_Open("GET", buf);
		WinHttp_Send();
		WinHttp_ResponseBody(buf);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}

	comment_idx = 0; //새로운 글에 댓글이 없으면 0으로 초기화하고 리턴돼야 하니 여기 적음

	if (strstr(buf, "comment_error") || strstr(buf, "error/deleted"))
		return false;

	while (1)
	{
		WinHttp_Open("POST", "http://gall.dcinside.com/comment/view");

		wsprintf(buf, "http://gall.dcinside.com/board/comment_view/?id=%s&no=%s&page=1", gallName, articlenum);
		WinHttp_SetRequestHeader("Referer", buf);


		WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
		WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
		WinHttp_SetRequestHeader("X-Requested-With", "XMLHttpRequest");

		wsprintf(buf, "ci_t=%s&id=%s&no=%s&comment_page=%d", ci_t, gallName, articlenum, currentPage++);

		__try
		{
			WinHttp_Send(buf);
			memset(buf, 0, sizeof(buf));
			WinHttp_ResponseBody(buf);

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{

		}
		if (*buf == '\0')
			break;
		//삭제된글에 센드하면 리스폰스로 아무것도안옴
		s = strstr(buf, "user_name=\'");

		while (s && comment_idx < _countof(comment))
		{
			s += strlen("user_name=\'");
			for (i = 0; s[i] != '\''; ++i);

			strncpy(nick[comment_idx], s, i);
			nick[comment_idx][i] = '\0';


			s = strstr(s, "<td class=\"reply\">") + strlen("<td class=\"reply\">");
			for (i = 0; s[i] == ' ' || s[i] == '\n'; ++i);


			for (j = i; s[j] != '<'; ++j);
			for (; s[j - 1] == ' '; --j);

			strncpy(comment[comment_idx], &s[i], j - i < 200 ? j - i : 200);

			comment[comment_idx][j - i < 200 ? j - i : 200] = '\0';

			p = strstr(comment[comment_idx], "&gt;");
			if (p)
			{
				*p = '>';
				memmove(&p[1], &p[4], strlen(p) - 4);
				comment[comment_idx][strlen(comment[comment_idx]) - 3] = '\0';
			}

			s = strstr(s, "user_name=\'");
			++comment_idx;
		}
	}

	return true;
}



unsigned int GetArticleInfo(const char *gallName, const char *articlenum, article_t *param, bool check)
{
	char buf[1024 * 512];
	char *s;
	unsigned int i, j, temp;

$start:;
	__try
	{
		wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=%s", gallName, articlenum);
		WinHttp_Open("GET", buf);
		WinHttp_Send();

		WinHttp_ResponseBody(buf);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		goto $start;
	}

	if (strstr(buf, "error/deleted"))
	{
		if (upparsing == 1)
		{
			wsprintf(buf, "http://gall.dcinside.com/board/lists/?id=%s", gallName);
			WinHttp_Open("GET", buf);
			WinHttp_Send();
			WinHttp_ResponseBody(buf);

			if ((s = strstr(buf, "<td class=\"t_notice\" >")))
			{
				s += strlen("<td class=\"t_notice\" >");

				while ((strncmp(s, "공지", strlen("공지")) == 0) || (strncmp(s, "<img src", strlen("<img src")) == 0))
					s = strstr(s, "<td class=\"t_notice\" >"), s += strlen("<td class=\"t_notice\" >");
				for (i = 0; s[i] != '<'; ++i);
				temp = s[i];
				s[i] = '\0';
				param->recentnum = atoi(s);
				s[i] = temp;
			}
		}
		return false;
	}

	if ((s = strstr(buf, "<td class=\"t_notice\" >")))
	{
		s += strlen("<td class=\"t_notice\" >");
		while ((strncmp(s, "공지", strlen("공지")) == 0) || (strncmp(s, "<img src", strlen("<img src")) == 0))
			s = strstr(s, "<td class=\"t_notice\" >"), s += strlen("<td class=\"t_notice\" >");

		for (i = 0; s[i] != '<'; ++i);
		s[i] = '\0';
		param->recentnum = atoi(s);
	}

	if (check)
		return true;
	
	s = strstr(buf, "<dt>제 목</dt>");

	s += strlen("<dt>제 목</dt>");

	for (i = 0; s[i] != '>'; ++i);
	for (j = 0; s[i + j + 1] != '<'; ++j);

	strncpy(param->title, &s[i + 1], j);
	param->title[j] = '\0';


	s = strstr(buf, "user_name=\"") + strlen("user_name\"") + 1;

	for (i = 0; s[i] != '\"'; ++i);

	strncpy(param->nick, s, i);
	param->nick[i] = '\0';
	char *p;
	s = strstr(buf, "<div class=\"s_write\"");
	p = strstr(s, "<!-- //con_substance -->");
	if (p == 0)
	{
		strcpy(param->content, ""); //여기 리스폰스바디가 4분의1정도밖에 안받아와짐 수정하자
		return false; 
	}
/*	dbg("터지기전 정보 p - s : %d", p - s);
	FILE *fp = fopen("zz.txt", "wt");
	fputs(s, fp);
	fclose(fp);*/

	strncpy(param->content, s, p - s);
	param->content[p - s] = '\0'; //굳이 상세할 필요 없어서 앞뒤로 대충 자름

	return true;

}

unsigned int GetArticleIP(const char *gallName, const char *articlenum)
{
	char buf[1024 * 512];
	char *s;


	wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=%s&page=1", gallName, articlenum);
	WinHttp_Open("GET", buf);
	WinHttp_Send();
	WinHttp_ResponseBody(buf);

	if (strstr(buf, "error/deleted"))
		return -1;
	if ((s = strstr(buf, "li_ip\">")))
	{
		s += strlen("li_ip\">");

		if (strncmp(s, "182.211.*.*", 8) == 0)
			printf("나 찾았다 ㅋ %s\n", articlenum);
	}


	return 0;
}

unsigned int GetArticlePicture(const char *gallName, const char *articlenum)
{
	char buf[1024 * 512];
	char buf2[1024];
	char temp[32];
	char extension[4];
	char url[20][512];
	char idstring[20][512];
	char filename[10][MAX_PATH];
	char *s, *p;
	unsigned int i, j, k, idx = 0, ididx = 0;
	SYSTEMTIME st;


	__try
	{
		wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=%s&page=1", gallName, articlenum);
		WinHttp_Open("GET", buf);
		WinHttp_Send();
		WinHttp_ResponseBody(buf);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{

	}
	if (strstr(buf, "error/deleted"))
		return -1;



	s = strstr(buf, "http://dcimg");
	if (s == NULL)
	{
		//printf("이미지 못찾음\n");
		return 0;
	}

	p = strstr(buf, "http://image.dcinside.com/viewimagePop.php");
	if (p == NULL)
		return 0;
	while (s && idx < _countof(url))
	{

		for (i = 0; s[i] != '\"' && s[i] != '>' && s[i] != ' '; ++i);
		strncpy(url[idx], s, i);
		url[idx][i] = '\0';


		if (p)
		{
			p += strlen("http://image.dcinside.com/viewimagePop.php");
			for (i = 0; p[i] != '\'' && p[i] != '\"'; ++i);
			strncpy(idstring[ididx], p, i);
			idstring[ididx][i] = '\0';
			p = strstr(p, "http://image.dcinside.com/viewimagePop.php");
			++ididx; //이부분 대충 만들었으니 나중에 다시해보자
		}
		++s; //한칸이라도 옮겨줘야 자기자신 검색안함
		s = strstr(s, "http://dcimg");
		++idx;

	}



	p = strstr(buf, "원본 첨부파일");

	for (i = 0; i < idx; ++i)
	{
		strcpy(buf2, &url[i][40]);
		buf2[strlen(url[i]) - 40] = '\0';

		s = strstr(p, buf2);

		if (s == NULL)
		{
			if (ididx != 0)
				s = strstr(p, idstring[i]);
			//if (s == NULL)
			//	break;
		}

		if (s)
		{
			for (j = 0; s[j] != '>'; ++j);
			for (k = 0; s[j + k + 1] != '<'; ++k);
			strncpy(filename[i], &s[j + 1], k);
			filename[i][k] = '\0';
		}
		else
		{
			wsprintf(buf2, "%s.%s", _itoa(rand(), temp, 10), "jpg");
			strcpy(filename[i], buf2);
		}



		if (_access(filename[i], 0) == 0)
		{
			strncpy(extension, &filename[i][strlen(filename[i]) - 3], 3);
			extension[3] = '\0';
			while (1)
			{
				wsprintf(buf2, "%s.%s", _itoa(rand(), temp, 10), extension);
				if (_access(buf2, 0) == -1)
				{
					strcpy(filename[i], buf2);
					break;
				}
			}
		}

		GetLocalTime(&st);
		
		wsprintf(buf, "이미지\\%02u-%02u-%02u\\%s", st.wYear, st.wMonth, st.wDay, filename[i]);


		if (URLDownloadToFile(NULL, url[i], buf, 0, 0) == INET_E_DOWNLOAD_FAILURE)
			
		{
			url[i][12] = url[i][12] == '1' ? '2' : '1';
			URLDownloadToFile(NULL, url[i], buf, 0, 0);
		}


	}

	return 0;


}

void point_out(const char *gallName, const char *articlenum)
{
	pointed_out_t pointed_out[80];
	unsigned int i, j, idx = 0;
	char *s;
	char title[128];
	char nick[128];
	char content[1024 * 256];
	char buf[512], buf2[512];
	article_t param;

	param.title = title;
	param.nick = nick;
	param.content = content;


	if (!GetArticleInfo(gallName, articlenum, &param, 0))
		return;

	GetComments(gallName, articlenum);

	for (i = 0; i < ps_idx; ++i)
	{

		s = (char*)strstr(title, ps[i].wrongstring);
		if (s)
		{
			if (IsImproperString(ps[i].wrongstring, s))
				continue;
			for (j = 0; j < comment_idx; ++j)
			{
				wsprintf(buf, "%s->%s", ps[i].wrongstring, ps[i].correctstring);
				if (strstr(comment[j], "[리듬 맞춤법 봇♬]") && !strchr(comment[j], '/') && strncmp(buf, comment[j], strlen(buf)) == 0)
				{
					j = 777;
					++overlapped;
					//printf("\r                           					\r이미 지적함 %d\n", overlapped);
					//dbg("이미 지적함 %d\n", overlapped);
					if (overlapped >= 10)
					{
						overlapped = 0;
						if (++list_idx == _countof(gallid_list))
							list_idx = 0;
						flip = 1;
						printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
						return;
					}
					break;
				}
			}
			if (j == 777)
				continue;
			wsprintf(buf, "%s->%s %s [리듬 맞춤법 봇♬]", ps[i].wrongstring, ps[i].correctstring, ps[i].explanation);
			if(strlen(buf) > 200)
				wsprintf(buf, "%s->%s [리듬 맞춤법 봇♬]", ps[i].wrongstring, ps[i].correctstring);
			WriteComment(gallName, articlenum, "ㅇㅇ", "0288", buf);
			wsprintf(buf2, "%s %s 에 %s이라고 씀", gallName, articlenum, buf);
			printf("\r                  \r%s\n", buf2);
			dbg("%s\n", buf2);
#ifdef WRITE_LOG
			fprintf(log_fp, "%s\n", buf2);
#endif
			pointed_out[idx].idx = i;
			strcpy(pointed_out[idx++].nick, "");	

			if (!GetComments(gallName, articlenum))
				return;
		}

		s = (char*)strstr(content, ps[i].wrongstring);
		if (s)
		{
			if (IsImproperString(ps[i].wrongstring, s))
				continue;
			for (j = 0; j < idx; ++j)
			{
				if(i == pointed_out[j].idx && strcmp(pointed_out[j].nick, "") == 0)
				{
					j = 777;
					++overlapped;
					//printf("\r                           					\r이미 지적함 %d\n", overlapped);
					//dbg("이미 지적함 %d\n", overlapped);
					if (overlapped >= 10)
					{
						overlapped = 0;
						if (++list_idx == _countof(gallid_list))
							list_idx = 0;
						flip = 1;
						printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
						return;
					}
					break;
				}
			}
			if (j == 777)
				continue;
			for (j = 0; j < comment_idx; ++j)
			{
				wsprintf(buf, "%s->%s", ps[i].wrongstring, ps[i].correctstring);

				if (strstr(comment[j], "[리듬 맞춤법 봇♬]") && !strchr(comment[j], '/') && strncmp(buf, comment[j], strlen(buf)) == 0)
				{
					j = 777;
					++overlapped;
					//printf("\r                           					\r이미 지적함 %d\n", overlapped);
					//dbg("이미 지적함 %d\n", overlapped);
					if (overlapped >= 10)
					{
						overlapped = 0;
						if (++list_idx == _countof(gallid_list))
							list_idx = 0;
						flip = 1;
						printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
						return;
					}
					break;
				}
			}
			if (j == 777)
				continue;

			wsprintf(buf, "%s->%s %s [리듬 맞춤법 봇♬]", ps[i].wrongstring, ps[i].correctstring, ps[i].explanation);
			if(strlen(buf) > 200)
				wsprintf(buf, "%s->%s [리듬 맞춤법 봇♬]", ps[i].wrongstring, ps[i].correctstring);
			WriteComment(gallName, articlenum, "ㅇㅇ", "0288", buf);
			wsprintf(buf2, "%s %s 에 %s이라고 씀", gallName, articlenum, buf);
			printf("\r                  \r%s\n", buf2);
			dbg("%s\n", buf2);
#ifdef WRITE_LOG
			fprintf(log_fp, "%s\n", buf2);
#endif
			pointed_out[idx].idx = i;
			strcpy(pointed_out[idx++].nick, "");
			if (!GetComments(gallName, articlenum))
				return;

		}



	}

	_point_out(gallName, articlenum);

}

unsigned int RecommendVote(const char *gallName, const char *articlenum, BOOL up)
{
	char buf[1024 * 512];

	if (up)
		WinHttp_Open("POST", "http://gall.dcinside.com/forms/recommend_vote_up");
	else
		WinHttp_Open("POST", "http://gall.dcinside.com/forms/recommend_vote_down");

	wsprintf(buf, "http://gall.dcinside.com/board/view/?id=%s&no=%s&page=1", gallName, articlenum); //&exception_mode=recommend
	WinHttp_SetRequestHeader("Referer", buf);

	//WinHttp_SetRequestHeader("Origin", "http://gall.dcinside.com");
	//WinHttp_SetRequestHeader("Accept_Encoding", "gzip, deflate");
	//WinHttp_SetRequestHeader("Accept-Language", "ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4");
	WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
	WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
	WinHttp_SetRequestHeader("X-Requested-With", "XMLHttpRequest");

	if (up)
		wsprintf(buf, "ci_t=%s&id=%s&no=%s&recommend=0&vote=vote&user_id=", ci_t, gallName, articlenum);
	else
		wsprintf(buf, "ci_t=%s&id=%s&no=%s&recommend=0&vote=vote", ci_t, gallName, articlenum);
	printf("%s\n", buf);
	WinHttp_Send(buf);
	WinHttp_ResponseBody(buf);

	printf("%s", buf);

	return 0;
}



void trimstring(char *str)
{
	unsigned int i;

	for (i = 0; i < strlen(str); ++i)
	{
		if (str[i] == ')')
			str[i] = '\0';
		else if (str[i] == ',')
			str[i] = ' ';
		else if (str[i] == '\'' || str[i] == '(')
			memcpy(&str[i], &str[i + 1], strlen(str) - i), --i;
	}
}

unsigned int DeleteComment(const char *gallName, const char *articlenum, const char *keyword)
{
	char buf[1024 * 512];
	char *s;
	char param[4][16] = { 0, };


	WinHttp_Open("POST", "http://gall.dcinside.com/comment/view");
	WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
	WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
	WinHttp_SetRequestHeader("X-Requested-With", "XMLHttpRequest");

	wsprintf(buf, "ci_t=%s&id=%s&no=%s&comment_page=1", ci_t, gallName, articlenum);
	WinHttp_Send(buf);
	WinHttp_ResponseBody(buf);

	if ((s = strstr(buf, keyword)))
	{
		if ((s = strstr(s, "del_comment_orgin")))
		{

			s += strlen("del_comment_orgin");
			trimstring(s);

			sscanf(s, "%s %s %s %s", param[0], param[1], param[2], param[3]);

			WinHttp_Open("POST", "http://gall.dcinside.com/forms/comment_delete_submit");
			WinHttp_SetRequestHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
			WinHttp_SetRequestHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.93 Safari/537.36");
			WinHttp_SetRequestHeader("X-Requested-With", "XMLHttpRequest");

			wsprintf(buf, "ci_t=%s&no=%s&id=%s&p_no=%s&re_no=%s&orgin_no=%s&best_orgin=&check_7=", ci_t, param[1], gallName, param[1], param[0], param[3]);
			WinHttp_Send(buf);
		}


	}

	return 0;
}


BOOL CtrlHandler(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_C_EVENT:
		overlapped = 0;
		if (++list_idx == _countof(gallid_list))
			list_idx = 0;
		flip = 1;
		printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
		break;
	default:
#ifdef WRITE_LOG
		if (log_fp)
		{
			fflush(log_fp);
			fclose(log_fp);
			log_fp = 0;
		}
#endif
		return FALSE;
	}
	return TRUE;
}

void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	overlapped = 0;
	if (++list_idx == _countof(gallid_list))
		list_idx = 0;
	flip = 1;
	flip2 = 1;
	if (log_fp)
		fflush(log_fp);
	printf("\r                  \r갤 ID : %s\n", gallid_list[list_idx]);
}

void print_all()
{
	for (unsigned int i = 0; i < ps_idx; ++i)
		printf("%s->%s %s\n", ps[i].wrongstring, ps[i].correctstring, ps[i].explanation);
		
}

void main()
{

	
	unsigned int i, j, lastnum, current_idx;
	char temp[16];
	article_t param;
	UINT timer_id;
	char buf[256];
	SYSTEMTIME st;

	minidump_setup();
	ps_init();

	SetConsoleTitle("하이");
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
	timer_id = timeSetEvent(1000 * 60 * 5, 1, TimerProc, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
#ifdef WRITE_LOG
	log_fp = fopen("C:\\Users\\지환\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication2\\ConsoleApplication2\\로그.txt", "atc");
	if (log_fp == NULL)
	{
		printf("파일 오픈 실패!\n");
		getchar();
		exit(0);
	}
#endif

	GetLocalTime(&st);
	wsprintf(buf, "이미지\\%02u-%02u-%02u", st.wYear, st.wMonth, st.wDay);
	CreateDirectory(buf, 0);

	srand(GetCurrentProcessId() ^ 0xBADF00Du);
	CoInitialize(0);

	CoCreateInstance(CLSID_InterfaceImplementation, NULL, CLSCTX_INPROC_SERVER, IID_IManagedInterface, reinterpret_cast<void**>(&csi));


	Getci_t(gallid_list[list_idx]);
	
	
	printf("갤 ID : %s\n", gallid_list[list_idx]);
$start:;
	//printf("1. 위로 파싱 2. 아래로 파싱 : ");
	//scanf("%d", &upparsing);
	//if (upparsing != 1 && upparsing != 2)
	//goto $start;


	GetArticleInfo("watch", 0, &param, 1);
	if (param.recentnum == 0)
	{
		printf("뭔가 이상함");
		return;
	}
	j = param.recentnum; //필요할 경우 시갤도 시작할 번호 따로 입력받기

	GetArticleInfo(gallid_list[list_idx], 0, &param, 1);
	if (param.recentnum == 0)
	{
		printf("뭔가 이상함");
		return;
	}
	lastnum = param.recentnum;

	if (upparsing == 2)
	{
		//printf("시작할 번호 입력 : ");
		//scanf("%d", &lastnum);
		lastnum = 0;
		if (lastnum == 0)
			lastnum = param.recentnum;
	}

#if 0
	if (upparsing == 1)
	{
		for (i = lastnum;; ++i)
		{
			printf("\r                  \r여기도는중 %d", i);

			current_idx = list_idx;
			if (flip)
			{
				GetArticleInfo(gallid_list[current_idx], 0, &param, 1);
				i = param.recentnum;
				flip = 0;
			}


			if (GetArticleInfo(gallid_list[current_idx], _itoa(i, temp, 10), &param, 1))
				point_out(gallid_list[current_idx], temp);
			else
			{
				while (1)
				{
					GetArticleInfo(gallid_list[current_idx], "1", &param, 1);
					if (param.recentnum > i)
						break;
					Sleep(3000);
				}
			}
		}
	}
#endif
	//else
	{

		for (i = lastnum, j;; --i, --j)
		{
			printf("\r                  \r여기도는중 %d 시갤은 %d", i, j);

			current_idx = list_idx;
			if (flip)
			{
				GetArticleInfo(gallid_list[current_idx], 0, &param, 1);
				i = param.recentnum;
				flip = 0;
			}
			if (flip2)
			{
				GetArticleInfo("watch", 0, &param, 1);
				j = param.recentnum;
				flip2 = 0;
			}

			if (GetArticleInfo(gallid_list[current_idx], _itoa(i, temp, 10), &param, 1))
				point_out(gallid_list[current_idx], temp);
			else
			{
				while (!GetArticleInfo(gallid_list[current_idx], _itoa(--i, temp, 10), &param, 1))
				{
					Sleep(100);
					dbg("ㅇㅇ도는중 %d\n", i);
				}
				++i; //for 증감문때문에 한 번 더 깎이는거 상쇄시켜줌
			}
			if (GetArticleInfo("watch", _itoa(j, temp, 10), &param, 1))
				point_out("watch", temp);
			else
			{
				while (!GetArticleInfo("watch", _itoa(--j, temp, 10), &param, 1))
				{
					Sleep(100);
					dbg("ㅇㅇ시갤도는중 %d\n", j);
				}
				++j;
			}
			//GetArticlePicture(gallid_list[current_idx], _itoa(i, temp, 10));
			//GetArticlePicture("watch", _itoa(j, temp, 10));

		}
	}
#ifdef WRITE_LOG
	if (log_fp)
	{
		fflush(log_fp);
		fclose(log_fp);
		log_fp = 0;
	}
#endif
	timeKillEvent(timer_id);
	CoUninitialize();
	return;
}