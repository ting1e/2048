// 2048.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "2048.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

RECT rectName, rectScreen;
RECT rectScore, rectScoreTitle,rectScoreNum;
RECT rectBest, rectBestTitle, rectBestNum;
RECT rect[4][4],rectText ,rectMain;


int cxScreen, cyScreen;
int cxWin, cyWin;
int cxWinInit, cyWinInit;
int bsave=0;


int num[4][4] = {0,2,4,8,
	16,32,64,128,
	256,512,1024,2048,
	0,0,0,0};

int copynum[4][4];
unsigned long score = 0;
unsigned long copyscore = 0;
unsigned long high_score = 0;
unsigned long copyhigh_score = 0;
double Timescx, Timescy,Times;
const int Round = 5;


TCHAR buffer_temp[10];
const TCHAR FontName[] = TEXT("楷体");

struct MyStruct
{
	TCHAR rankname[32];
	unsigned long rankscore;
} RankingList[6];

const unsigned long cr2 = RGB(238, 228, 218);
const unsigned long cr4 = RGB(237, 224, 200);
const unsigned long cr8 = RGB(242, 177, 121);
const unsigned long cr16 = RGB(245, 149, 99);
const unsigned long cr32 = RGB(246, 124, 95);
const unsigned long cr64 = RGB(246, 94, 59);
#define cr128 RGB(237,204,97)
const unsigned long cr256 = cr128;
const unsigned long cr512 = cr128;
const unsigned long cr1024 = cr128;
const unsigned long cr2048 = RGB(237, 194, 46);


// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    NewRecord(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Ranking(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void DrawTextAdvance(HDC hdc, TCHAR text[], RECT *rect, int FontSize, int FontWeight,
	unsigned long color, const TCHAR FontName[], UINT format);

void FillRectAdvance(HDC hdc, RECT *rect, unsigned long color);

unsigned long Num2Color(unsigned int num);

TCHAR * int2ptchar(unsigned int i);

void newone(PVOID pvoid);

void Thread1(PVOID pvoid);

void FreshMain(HWND hWnd);

void Bedead(HWND hwnd);

//void NewRecord(void);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MY2048, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2048));

    MSG msg;

    // 主消息循环: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2048));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(RGB(250, 248, 239));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MY2048);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中
   SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
   cxScreen = rectScreen.right - rectScreen.left;
   cyScreen = rectScreen.bottom - rectScreen.top;
   cxWinInit = 4 * cxScreen / 14;
   cyWinInit = cyScreen * 3 / 4;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      5*cxScreen /14, cyScreen /8,cxWinInit,cyWinInit, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//

void DrawTextAdvance(HDC hdc, TCHAR text[], RECT *rect, int FontSize, int FontWeight,
	unsigned long color, const TCHAR FontName[], UINT format)
{
	HFONT hf; 
	hf = CreateFont(FontSize, 0, 0, 0, FontWeight, 0, 0, 0, 0, 0, 0, 0, 0, FontName);
	SelectObject(hdc, hf);
	SetTextColor(hdc, color);
	DrawText(hdc, text, -1, rect, format);
	DeleteObject(hf);
}

void FillRectAdvance(HDC hdc, RECT *rect, unsigned long color)
{
	HBRUSH hBrush;
	hBrush = CreateSolidBrush(color);
	FillRect(hdc, rect, hBrush);
	DeleteObject(hBrush);
}

unsigned long Num2Color(unsigned int num)
{
	switch (num)
	{
	case 2:return cr2; break;
	case 4:return cr4; break;
	case 8:return cr8; break;
	case 16:return cr16; break;
	case 32:return cr32; break;
	case 64:return cr64; break;
	case 128:return cr128; break;
	case 256:return cr256; break;
	case 512:return cr512; break;
	case 1024:return cr1024; break;
	case 2048:return cr2048; break;
	default:return cr2048; break;
	}
}

TCHAR * int2ptchar(unsigned int i)
{
	wsprintf(buffer_temp, TEXT("%d"), i);
	return buffer_temp;
}

void newone(PVOID pvoid)
{
	HWND hwnd = (HWND)pvoid;
	srand(GetTickCount());
	int x, y,temp=0;
	int i = rand() % 16;
	int j = rand() % 2;
	while (1)
	{
		for (x = 0; x < 4; x++)
		{
			for (y = 0; y < 4; y++)
			{
				if (num[x][y] == 0)
				{
					if (temp++ == i)
					{
						num[x][y] = (j + 1) * 2;
						InvalidateRect(hwnd, &rect[x][y], FALSE);
						return;
					}
				}
			}
		}
		if (temp == 0)
			break;
	}
	
}
void Thread1(PVOID pvoid)
{

	Sleep(150);
	newone(pvoid);
	//FreshMain((HWND )pvoid);
	_endthread();
}

void FreshMain(HWND hWnd)
{
	int i, j;
	HBRUSH hBrush;
	HDC hdc = GetDC(hWnd);

	SetBkMode(hdc, TRANSPARENT);

	hBrush = CreateSolidBrush(RGB(187, 173, 160));
	SelectObject(hdc, hBrush);//分数块背景

	HPEN hPen = CreatePen(PS_NULL, 0, 0);
	SelectObject(hdc, hPen);//去掉画笔
	RoundRect(hdc, rectMain.left, rectMain.top, rectMain.right,
		rectMain.bottom, Round, Round);
	for (i = 0; i<4; i++)
		for (j = 0; j<4; j++)
		{
			if (num[i][j] != 0)
			{
				FillRectAdvance(hdc, &rect[i][j], Num2Color(num[i][j]));
				DrawTextAdvance(hdc, int2ptchar(num[i][j]), &rect[i][j],
					(int)(((num[i][j]<1024) ? 52 : 42)*Times), 700,
					(num[i][j] <= 8) ? RGB(119, 110, 101) : RGB(255, 255, 255), FontName,
					DT_CENTER | DT_SINGLELINE | DT_VCENTER);
			}
		}
	DeleteObject(hBrush);
	DeleteObject(hPen);
	ReleaseDC(hWnd,hdc);

}

void Bedead(HWND hWnd)
{
	int i, j,flag1,flag2,flag3,flag4;
	flag1 = 1;
	flag2 = 1;
	flag3 = 1;
	flag4 = 1;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			if (num[i][j] == 0)
				flag1 = 0;
			if (num[i][j] != copynum[i][j])
				flag1 = 0;
			if (j < 3)
			{
				if (num[i][j] == num[i][j + 1])
					flag3 = 0;
			}
			if (i < 3)
			{
				if (num[i][j] == num[i + 1][j])
					flag4 = 0;
			}
		}
	}
	if (flag1&&flag1&&flag3&&flag4)
	{
		if (bsave)
		{
			if (score > RankingList[5].rankscore)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, NewRecord);
				//bsave = 0;
			}
		}

		int choice = MessageBox(hWnd, TEXT("你已经没有可移动的方块了！\n是否开始一句新的游戏？"),
			TEXT("游戏结束"),
			MB_YESNO | MB_ICONQUESTION);
		if (choice == IDYES)
		{
			SendMessage(hWnd, WM_COMMAND, IDM_NEWGAME, 0);
		}

	}

}

void cheat()
{
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			num[i][j] = 2048;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i, j;
	double length, hight;
    switch (message)
    {
	case WM_CREATE:
		bsave = 1;
	//	cheat();
		SendMessage(hWnd, WM_COMMAND, IDM_NEWGAME,NULL);
		break;
	case WM_SIZE:
		cxWin = LOWORD(lParam);
		cyWin = HIWORD(lParam);

		Timescx = (double)cxWin / cxWinInit;
		Timescy = (double)cyWin / cyWinInit;
		Times = min(sqrt(Timescx*Timescy), (min(Timescx, Timescy)));

		rectName.left = (int)(10*Timescx);
		rectName.right = (int)(145 * Timescx);
		rectName.top = (int)(26*Timescy);
		rectName.bottom =  (int)(126 * Timescy);


		rectScore.left = (int)(165 * Timescx);
		rectScore.right = (int)(285 * Timescx);
		rectScore.top = (int)(26 * Timescy);
		rectScore.bottom = (int)(126 * Timescy);

		rectScoreTitle.left = (int)(165 * Timescx);
		rectScoreTitle.right = (int)(285 * Timescx);
		rectScoreTitle.top = (int)(26 * Timescy);
		rectScoreTitle.bottom = (int)(71 * Timescy);

		rectScoreNum.left = (int)(165 * Timescx);
		rectScoreNum.right = (int)(285 * Timescx);
		rectScoreNum.top = (int)(81 * Timescy);
		rectScoreNum.bottom = (int)(126 * Timescy);


		rectBest.left = (int)(305 * Timescx);
		rectBest.right = (int)(425 * Timescx);
		rectBest.top = (int)(26 * Timescy);
		rectBest.bottom = (int)(126 * Timescy);

		rectBestTitle.left = (int)(305 * Timescx);
		rectBestTitle.right = (int)(425 * Timescx);
		rectBestTitle.top = (int)(26 * Timescy);
		rectBestTitle.bottom = (int)(71 * Timescy);

		rectBestNum.left = (int)(305 * Timescx);
		rectBestNum.right = (int)(425 * Timescx);
		rectBestNum.top = (int)(81 * Timescy);
		rectBestNum.bottom = (int)(126 * Timescy);

		rectText.left = (int)(8 * Timescx);
		rectText.right = (int)(435 * Timescx);
		rectText.top= (int)(150 * Timescy);
		rectText.bottom = (int)(170 * Timescy);

		rectMain.left = (int)(10 * Timescx);
		rectMain.right = (int)(cxWin-10 * Timescx);
		rectMain.top = (int)(185 * Timescy);
		rectMain.bottom = (int)(cyWin-10 * Timescy);

		length = (cxWin - 70*Timescx)/4;
		hight = (cyWin - 245 * Timescy) / 4;
		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				rect[i][j].left = (int)(20 * Timescx + +length*j+10 * Timescx*j);
				rect[i][j].right= (int)(20 * Timescx + length*j+length + 10 * Timescx*j);
				rect[i][j].top = (int)(195 * Timescy + hight*i + 10 * Timescy*i);
				rect[i][j].bottom= (int)(195 * Timescy + hight*i+hight+ 10 * Timescy*i);
			}
		}
			
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
			case IDM_NEWGAME:
				bsave = 1;
				for (int i = 0; i<4; i++)
					for (int j = 0; j<4; j++)
						num[i][j] = 0;
				score = 0;
				newone(hWnd);
				InvalidateRect(hWnd, &rectMain, FALSE);
				InvalidateRect(hWnd, &rectScore, TRUE);
				break;
			case IDM_DELETE:
				for (int i = 0; i<4; i++)
					for (int j = 0; j<4; j++)
						num[i][j] = copynum[i][j];
				score = copyscore;
				high_score = copyhigh_score;
				InvalidateRect(hWnd, &rectMain, FALSE);
				InvalidateRect(hWnd, &rectScore, TRUE);
				InvalidateRect(hWnd, &rectBest, TRUE);
				break;
			case IDM_RANKING:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, Ranking);
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
		/*cxWin = LOWORD(wParam);
		cyWin = HIWORD(wParam);*/
			HBRUSH hBrush;
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

			SetBkMode(hdc, TRANSPARENT);

			hBrush = CreateSolidBrush(RGB(187, 173, 160));
			SelectObject(hdc, hBrush);//分数块背景
			
			HPEN hPen = CreatePen(PS_NULL, 0, 0);
			SelectObject(hdc, hPen);//去掉画笔

			//2048
			DrawTextAdvance(hdc, TEXT("2048"), &rectName, (INT)(65*Times), 0,
				RGB(115, 106, 98), TEXT("楷体"), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			

			//score
			RoundRect(hdc, rectScore.left, rectScore.top, 
				rectScore.right, rectScore.bottom, Round, Round);
			DrawTextAdvance(hdc, TEXT("分数"), &rectScoreTitle, (int)(30* Times),
				700, RGB(245, 235, 226), FontName, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			DrawTextAdvance(hdc, int2ptchar(score), &rectScoreNum, (int)(35 * Times),
				700, RGB(245, 235, 226), FontName, DT_CENTER | DT_SINGLELINE | DT_TOP);

			//top
			RoundRect(hdc, rectBest.left, rectBest.top,
				rectBest.right, rectBest.bottom, Round, Round);
			DrawTextAdvance(hdc, TEXT("最高分"), &rectBestTitle, (int)(30 * Times),
				700, RGB(245, 235, 226), FontName, DT_CENTER | DT_SINGLELINE | DT_BOTTOM);
			DrawTextAdvance(hdc, int2ptchar(high_score), &rectBestNum, (int)(35 * Times),
				700, RGB(245, 235, 226), FontName, DT_CENTER | DT_SINGLELINE | DT_TOP);

			//two sentences
			DrawTextAdvance(hdc, TEXT("说明：上下左右，移动色块，合出更大的数字！"),
				&rectText, (int)(20*Times),0,
				RGB(115, 106, 98), FontName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			/*DrawTextAdvance(hdc, TEXT("把这些数字合出2048！"), &rectVCenterText, 13, 0,
				crText, FontName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);*/
			
			//main rect
			RoundRect(hdc, rectMain.left, rectMain.top, rectMain.right,
				rectMain.bottom, Round, Round);
			SetBkMode(hdc, TRANSPARENT);
			for (i = 0; i<4; i++)
				for ( j = 0; j<4; j++)
				{
					if (num[i][j] != 0)
					{
						FillRectAdvance(hdc, &rect[i][j], Num2Color(num[i][j]));
						DrawTextAdvance(hdc, int2ptchar(num[i][j]), &rect[i][j], 
							(int)(((num[i][j]<1024) ? 52 : 42)*Times), 700, 
							(num[i][j] <= 8) ? RGB(119, 110, 101) : RGB(255,255,255), FontName,
							DT_CENTER | DT_SINGLELINE | DT_VCENTER);
						if (num[i][j] > 10000)
						{
							FillRectAdvance(hdc, &rect[i][j], Num2Color(num[i][j]));
							DrawTextAdvance(hdc, int2ptchar(num[i][j]), &rect[i][j],
								(int)(35*Times), 700,
								(num[i][j] <= 8) ? RGB(119, 110, 101) : RGB(255, 255, 255), FontName,
								DT_CENTER | DT_SINGLELINE | DT_VCENTER);
						}
					}
				}
			DeleteObject(hBrush);
			DeleteObject(hPen);
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_KEYDOWN:
	//	FreshMain(hWnd);
		int flag, temp, i, j;
		switch (wParam)
		{
		case VK_LEFT:
			flag = 0;
			copyscore = score;
			copyhigh_score = high_score;
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 4; j++)
				{
					copynum[i][j] = num[i][j];
				}
			}
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 3; j++)
				{
					if (num[i][j] == 0)
					{
						for (temp = j; temp < 3; temp++)
						{
							num[i][temp] = num[i][temp + 1];
							
						}
						num[i][3] = 0;
					}
				}
			}
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 3; j++)
				{
					if (num[i][j] == 0 && num[i][j + 1] != 0)
					{
						for (temp = j; temp < 3; temp++)
							num[i][temp] = num[i][temp + 1];
					}
				}
			}
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 3; j++)
				{
					if (num[i][j] == num[i][j + 1])
					{
						num[i][j] += num[i][j + 1];
						score += num[i][j];
						high_score = score>high_score?score:high_score;
						for (temp = j + 1; temp < 3; temp++)
						{
							num[i][temp] = num[i][temp + 1];
						}
						num[i][3] = 0;
				
					}
				}
			}
			
			//move done

			break;

		case VK_RIGHT:
			flag = 0;
			copyscore = score;
			copyhigh_score = high_score;
			for (i = 0; i < 4; i++)
			{
				for (j = 0; j < 4; j++)
				{
					copynum[i][j] = num[i][j];
				}
			}
			for (i = 0; i < 4; i++)
			{
				for (j = 3; j>0; j--)
				{
					if (num[i][j] == 0)
					{
						for (temp = j; temp >0; temp--)
						{
							num[i][temp] = num[i][temp - 1];

						}
						num[i][0] = 0;
			
					}
				}
			}

			for (i = 0; i < 4; i++)
			{
				for (j = 3; j > 0; j--)
				{
					if (num[i][j] == 0 && num[i][j - 1] != 0)
					{
						for (temp = j; temp > 0; temp--)
							num[i][temp] = num[i][temp - 1];
					}
				}
			}

			for (i = 0; i < 4; i++)
			{
				for (j = 3; j >0; j--)
				{
					if (num[i][j] == num[i][j - 1])
					{
						num[i][j] += num[i][j - 1];
						score += num[i][j];
						high_score = score>high_score ? score : high_score;
						for (temp = j - 1; temp > 0; temp--)
						{
							num[i][temp] = num[i][temp - 1];

						}
						num[i][0] = 0;
			
					}
				}
			}
			
			//move done

			break;
		case VK_UP:
			flag = 0;
			copyscore = score;
			copyhigh_score = high_score;
			for (j = 0; j < 4; j++)
			{
				for (i = 0; i < 4; i++)
				{
					copynum[i][j] = num[i][j];
				}
			}
			for (j = 0; j < 4; j++)
			{
				for (i = 0; i< 3; i++)
				{
					if (num[i][j] == 0)
					{
						for (temp = i; temp < 3; temp++)
						{
							num[temp][j] = num[temp+1][j];
						}
						num[3][j] = 0;
					
					}
				}
			}

			for (j = 0; j < 4; j++)
			{
				for (i = 0; i < 3; i++)
				{
					if (num[i][j] == 0 && num[i + 1][j] != 0)
					{
						for (temp = i; temp < 3; temp++)
							num[temp][j] = num[temp + 1][j];
					}
				}
			}

			for (j = 0; j < 4; j++)
			{
				for (i = 0; i < 3; i++)
				{
					if (num[i][j] == num[i+1][j])
					{
						num[i][j] += num[i+1][j];
						score += num[i][j];
						high_score = score>high_score ? score : high_score;
						for (temp = i + 1; temp < 3; temp++)
						{
							num[temp][j] = num[temp+1][j];

						}
						num[3][j] = 0;
		
					}
				}
			}
		
			//move done

			break;

		case VK_DOWN:
			flag = 0;
			copyscore = score;
			copyhigh_score = high_score;
			for (j = 0; j < 4; j++)
			{
				for (i = 0; i < 4; i++)
				{
					copynum[i][j] = num[i][j];
				}
			}
			for (j = 0; j < 4; j++)
			{
				for (i = 3; i> 0; i--)
				{
					if (num[i][j] == 0)
					{
						for (temp = i; temp >0 ; temp--)
						{
							num[temp][j] = num[temp -1][j];
						}
						num[0][j] = 0;
	
					}
				}
			}

			
			for (j = 0; j < 4; j++)
			{
				for (i = 3; i > 0; i--)
				{
					if (num[i][j] == 0 && num[i - 1][j] != 0)
					{
						for (temp = i; temp > 0; temp--)
							num[temp][j] = num[temp - 1][j];
					}

				}
			}
			for (j = 0; j < 4; j++)
			{
				for (i = 3; i > 0; i--)
				{
					if (num[i][j] == num[i - 1][j])
					{
						num[i][j] += num[i - 1][j];
						score += num[i][j];
						high_score = score>high_score ? score : high_score;
						for (temp = i - 1; temp > 0; temp--)
						{
							num[temp][j] = num[temp - 1][j];

						}
						num[0][j] = 0;
		
					}
				}
			}
			//move done

			break;
		default:
			break;
		}

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				if (num[i][j] != copynum[i][j])
				{
					InvalidateRect(hWnd, &rect[i][j], FALSE);
					flag = 1;
				}
			}
		}
		if (flag)
		{
			//FreshMain(hWnd);
			flag = 0;
			_beginthread(Thread1, NULL, hWnd);
			
		}

		if (score > copyscore)
			InvalidateRect(hWnd, &rectScore, FALSE);
		if (high_score > copyhigh_score)
			InvalidateRect(hWnd, &rectBest, FALSE);

		Bedead(hWnd);
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
	
    return 0;
}

INT_PTR CALLBACK NewRecord(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{

	case WM_INITDIALOG:

		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK )
		{
			int i, j;
			for (i = 0; i < 6; i++)
			{
				if (score > RankingList[i].rankscore)
				{
					for (j = 5; j >i; j--)
					{
						lstrcpy(RankingList[j].rankname, RankingList[j-1].rankname);
						if (RankingList[j].rankname[0] == '\0')
							lstrcpy(RankingList[j].rankname, TEXT("佚名"));
						RankingList[j].rankscore = RankingList[j-1].rankscore;
					}
					RankingList[i].rankscore = score;
					GetWindowText(GetDlgItem(hDlg, IDC_EDIT1),
						RankingList[i].rankname, 32);
					if (RankingList[i].rankname[0] == '\0')
						lstrcpy(RankingList[i].rankname, TEXT("佚名"));
					bsave = 0;
					break;
				}
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Ranking(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{

	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{

	case WM_INITDIALOG:
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	case WM_PAINT:
		RECT rect;
		PAINTSTRUCT ps;
		TCHAR szBuffer[32];
		for (int i = 0; i < 6; i++)
		{
			HWND hwnd = GetDlgItem(hDlg, IDC_RANK1 + i);
			GetClientRect(hwnd, &rect);
			HDC hdc = BeginPaint(hwnd, &ps);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, RankingList[i].rankname, -1, &rect, 
				DT_LEFT | DT_SINGLELINE | DT_VCENTER|DT_TOP);
			rect.left = 85;
			wsprintf(szBuffer, TEXT("%d"), RankingList[i].rankscore);
			DrawText(hdc, szBuffer, -1, &rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			ReleaseDC(hwnd, hdc);
			EndPaint(hwnd, &ps);

		}
		break;

	}
	return (INT_PTR)FALSE;
}




// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	/*BITMAP bitmap;
	HINSTANCE hInstance ;
	static HBITMAP hBitmap ;
	static HWND hCtrl;*/
	
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
		
    case WM_INITDIALOG:
	//	hCtrl = GetDlgItem(hDlg, IDC_PAINT);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
	/*case WM_PAINT:
	{
		//LPWSTR x = MAKEINTRESOURCE(IDI_ICON1);
		//hInstance = GetModuleHandleW(0);
		//hBitmap = LoadBitmap(hInstance, MAKEINTRESOURCE(IDI_SMALL));
		//GetObject(hBitmap, sizeof(BITMAP), &bitmap);
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hCtrl, &ps);
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
		//HDC hdcMem = CreateCompatibleDC(hdc);
		//SelectObject(hdcMem, hBitmap);
		//TextOut(hdc,0, 0, TEXT("0.0"), 3);
		//SetStretchBltMode(hdc, STRETCH_HALFTONE);
		//StretchBlt(hdc, 0, 0, 10, 10, hdcMem, 0, 0,10, 10,SRCCOPY );

		Ellipse(hdc, 0, 0, 10, 10);
		EndPaint(hCtrl, &ps);
	}
	break;*/

    }
    return (INT_PTR)FALSE;
}
