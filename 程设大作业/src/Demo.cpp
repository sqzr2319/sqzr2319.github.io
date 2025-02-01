// Demo.cpp : 定义应用程序的入口点。
#include "Demo.h"
using namespace std;
#define _CRT_SECURE_NO_WARNINGS

#pragma region 全局变量

#define MAX_LOADSTRING 100

// 全局变量：
HINSTANCE hInst;                     // 当前实例
WCHAR szTitle[MAX_LOADSTRING];       // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING]; // 主窗口类名

// 位图资源：
HBITMAP bmp_Brick;           // 砖块图像资源
HBITMAP bmp_WhiteBackground; // 生成的纯白色背景图像
HBITMAP bmp_GameBackground;  // 生成的游戏背景图像
HBITMAP bmp_Tetris;          // 生成的标题
HBITMAP bmp_Start;           // 开始按钮
HBITMAP bmp_Blank;           // 空白按钮
HBITMAP bmp_Help;            // 帮助按钮
HBITMAP bmp_Back;            // 返回按钮
HBITMAP bmp_HelpWords;       // 帮助文字
HBITMAP bmp_Wall;            // 墙壁
HBITMAP bmp_Resume;		     // 继续按钮
HBITMAP bmp_Restart;	     // 重新开始按钮
HBITMAP bmp_Exit;		     // 退出按钮
HBITMAP bmp_Setting;	     // 设置按钮
HBITMAP bmp_SettingWords;    // 设置文字
HBITMAP bmp_Rank;			 // 排行榜
HBITMAP bmp_Reset;			 // 重置按钮
HBITMAP bmp_Preview;		 // 方块下落预览
HBITMAP bmp_Save;			 // 保存按钮

// 游戏状态：
Stage *currentStage = NULL;                     // 当前场景状态
vector<Button *> buttons;                       // 按钮
Brick *brick = NULL;                            // 砖块
Block *block = NULL;                            // 方块
vector<Brick*> base;	                        // 底部砖块
int used[BRICK_ROWS][BRICK_COLUMNS] = { 0 };    // 标记已经使用的位置
int score = 0;                                  // 分数
int random = rand() % 7;                        // 随机数
int randframe = rand() % 4;                     // 随机帧数
int speed = 1;                                  // 速度
double bonus = 1;                               // 系数
int bgm = 5;									// 背景音乐音量
int effects = 5;								// 音效音量
int cnt = 0;                                    // 计数器
int blind = 0;                                  // 盲打模式
int savetype = -1;                              // 保存方块类型
int saveframe = -1;                             // 保存帧数
int speedmode = 0;							    // 速度模式
int timer = 600;                                // 计时器
int avoidrankoverlap = 0;                       // 防止排行榜重叠
int avoidmultiplefall = 0;                      // 防止多次下落

// 存档
FILE* classicrankfile = NULL;					    // 经典模式排行榜文件
FILE* speedrankfile = NULL;						    // 速度模式排行榜文件
FILE* savefile = NULL; 							    // 存档文件
multiset<int> classicrank;					        // 经典模式排行榜
multiset<int> speedrank;							// 速度模式排行榜

// 键盘：
bool keyFallDown = false;      // 下落
bool keyDownDown = false;      // 下
bool keyLeftDown = false;      // 左
bool keyRightDown = false;     // 右
bool keyRotate = false;        // 旋转
bool keyQDown = false;         // Q
bool keyEDown = false;         // E
bool keyTDown = false;         // T

// 鼠标：
int mouseX = 0;         // 鼠标X坐标
int mouseY = 0;         // 鼠标Y坐标
bool mouseDown = false; // 鼠标按下

#pragma endregion

#pragma region Win32程序框架

// 此代码模块中包含的函数的前向声明:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_DEMO, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMO));

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

    return (int)msg.wParam;
}

//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMO));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DEMO);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 将实例句柄存储在全局变量中

    HWND hWnd = CreateWindowW(
        szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, // 设置窗口样式，不可改变大小，不可最大化
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WINDOW_WIDTH,
        WINDOW_HEIGHT + WINDOW_TITLEBARHEIGHT,
        nullptr, nullptr, hInstance, nullptr);

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
//  目标: 处理主窗口的消息。
//
//  WM_CREATE   - 处理窗口创建事件
//  WM_TIMER    - 处理定时器TimeOut事件
//  WM_PAINT    - 绘制主窗口
//  WM_COMMAND  - 处理应用程序菜单
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        // 初始化游戏窗体
        InitGame(hWnd, wParam, lParam);
        // 打开背景音乐文件
        mciSendString(L"open \"res\\background.mp3\" type mpegvideo alias bgm", NULL, 0, NULL);
        // 播放背景音乐
        mciSendString(L"play bgm repeat", NULL, 0, NULL);
		// 设置背景音乐音量
		mciSendString(L"setaudio bgm volume to 250", NULL, 0, NULL);
		// 打开音效文件
		mciSendString(L"open \"res\\fall.mp3\" type mpegvideo alias fall", NULL, 0, NULL);
		mciSendString(L"open \"res\\eliminate.mp3\" type mpegvideo alias eliminate", NULL, 0, NULL);
		// 设置音效音量
		mciSendString(L"setaudio fall volume to 500", NULL, 0, NULL);
		mciSendString(L"setaudio eliminate volume to 500", NULL, 0, NULL);
    }
    break;
    case WM_KEYDOWN:
    {
        // 键盘按下事件
        KeyDown(hWnd, wParam, lParam);
    }
    break;
    case WM_KEYUP:
    {
        // 键盘松开事件
        KeyUp(hWnd, wParam, lParam);
    }
    break;
    case WM_MOUSEMOVE:
    {
        // 鼠标移动事件
        MouseMove(hWnd, wParam, lParam);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        // 鼠标左键按下事件
        LButtonDown(hWnd, wParam, lParam);
    }
    break;
    case WM_LBUTTONUP:
    {
        // 鼠标左键松开事件
        LButtonUp(hWnd, wParam, lParam);
    }
    break;
    case WM_TIMER:
    {
        // 判断游戏是否暂停
        if (currentStage != NULL && !currentStage->pause)
        {
            TimerUpdate(hWnd, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        // 绘图
        Paint(hWnd);
    }
    break;

    // TODO：其他消息
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_DESTROY:
    {
        // 停止播放背景音乐
        mciSendString(L"stop bgm", NULL, 0, NULL);
        // 关闭背景音乐
        mciSendString(L"close bgm", NULL, 0, NULL);
		// 关闭音效
		mciSendString(L"close fall", NULL, 0, NULL);
		mciSendString(L"close eliminate", NULL, 0, NULL);
        // 窗口退出
        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
    }
    return (INT_PTR)FALSE;
}

#pragma endregion

#pragma region 事件处理函数

// 初始化游戏窗体函数
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // 加载图像资源
    bmp_Brick = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_BRICK));
	bmp_Tetris = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_TETRIS));
	bmp_Start = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_GAME_START));
	bmp_Blank = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_GAME_BLANK));
	bmp_Help = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_HELP));
	bmp_Back = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_BACK));
	bmp_HelpWords = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_HELPWORDS));
	bmp_Wall = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_WALL));
	bmp_Resume = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_RESUME));
	bmp_Restart = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_RESTART));
	bmp_Exit = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_EXIT));
	bmp_Setting = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_SETTING));
	bmp_SettingWords = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_SETTINGWORDS));
	bmp_Rank = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_RANK));
	bmp_Reset = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_RESET));
	bmp_Preview = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_PREVIEW));
	bmp_Save = LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP_SAVE));
    bmp_WhiteBackground = CreateWhiteBackground(hWnd);
    bmp_GameBackground = CreateGameBackground(hWnd);

    // 初始化开始场景
    InitStage(hWnd, STAGE_START);

    // 初始化主计时器
    SetTimer(hWnd, TIMER_GAMETIMER, TIMER_GAMETIMER_ELAPSE, NULL);
}

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_SPACE:    // Space
		if (avoidmultiplefall == 0)
            keyFallDown = true;
        break;
    case 0x53:        // S
        keyDownDown = true;
        break;
    case 0x41:        // A
        keyLeftDown = true;
        break;
    case 0x44:        // D
        keyRightDown = true;
        break;
	case 0x57:        // W
		keyRotate = true;
		break;
	case 0x51:        // Q
		keyQDown = true;
		break;
	case 0x45:        // E
		keyEDown = true;
		break;
	case VK_ESCAPE:   // ESC
		// 暂停游戏
        if (currentStage->stageID == STAGE_GAME) {
            currentStage->pause = false;
			savetype = block->type;
			saveframe = block->frame_index;
            InitStage(hWnd, STAGE_PAUSE);
        }
		break;
	case 0x54:        // T
		keyTDown = true;
		break;
    // TODO：其他按键
    default:
        break;
    }
}

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_SPACE:    // Space
		avoidmultiplefall = 0;
        keyFallDown = false;
        break;
    case 0x53:        // S
        keyDownDown = false;
        break;
    case 0x41:        // A
        keyLeftDown = false;
        break;
    case 0x44:        // D
        keyRightDown = false;
        break;
    // TODO：其他按键
    default:
        break;
    }
}

// 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
}

// 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
    mouseDown = true;

    // 判断鼠标是否点击按钮
    for (int i = 0; i < buttons.size(); i++)
    {
        Button *button = buttons[i];
        if (button->x <= mouseX && button->x + button->width >= mouseX && button->y <= mouseY && button->y + button->height >= mouseY)
        {
            switch (button->buttonID)
            {
            case BUTTON_STARTGAME_ID:
                score = 0;
                timer = 600;
                for (int i = 0; i < BRICK_ROWS; i++)
                    for (int j = 0; j < BRICK_COLUMNS; j++)
                        used[i][j] = 0;
                InitStage(hWnd, STAGE_GAME);
                break;
			case BUTTON_HELPGAME_ID:
				InitStage(hWnd, STAGE_HELP);
				break;
			case BUTTON_BACK_ID:
				InitStage(hWnd, STAGE_START);
				break;
			case BUTTON_RESUME_ID:
                if (currentStage->stageID == STAGE_PAUSE)
                {
                    InitStage(hWnd, STAGE_GAME);
                    currentStage->pause = false;
                }
                else
                {
                    // 读档
					fopen_s(&savefile, "save.txt", "r");
					for (int i = 0; i < BRICK_ROWS; i++)
					{
						for (int j = 0; j < BRICK_COLUMNS; j++)
							fscanf_s(savefile, "%d", &used[i][j]);
					}
					fscanf_s(savefile, "%d %d %d %d %d %d %d", &score, &timer, &savetype, &saveframe, &speedmode, &blind, &speed);
					fclose(savefile);
					InitStage(hWnd, STAGE_GAME);
                }
				break;
			case BUTTON_RESTART_ID:
                score = 0;
				timer = 600;
				for (int i = 0; i < BRICK_ROWS; i++)
					for (int j = 0; j < BRICK_COLUMNS; j++)
						used[i][j] = 0;
				random = rand() % 7;
				randframe = rand() % 4;
                avoidrankoverlap = 0;
                InitStage(hWnd, STAGE_GAME);
                currentStage->pause = false;
				break;
			case BUTTON_EXIT_ID:
				if (currentStage->stageID == STAGE_START)
					PostQuitMessage(0);
                else if (currentStage->stageID == STAGE_END)
                {
					avoidrankoverlap = 0;
					InitStage(hWnd, STAGE_START);
                }
                else if (currentStage->stageID == STAGE_PAUSE)
                {
                    savetype = -1;
					saveframe = -1;
                    InitStage(hWnd, STAGE_START);
                }
				break;
			case BUTTON_SETTING_ID:
				InitStage(hWnd, STAGE_SETTING);
				break;
			case BUTTON_RANK_ID:
				InitStage(hWnd, STAGE_RANK);
				break;
			case BUTTON_RESET_ID:
				// 重置排行榜
                classicrank.clear();
				speedrank.clear();
				fopen_s(&classicrankfile,"classicrank.txt", "w");
				for (int i = 1; i <= 5; i++)
				{
					classicrank.insert(0);
					fprintf(classicrankfile, "0\n");
				}
				fclose(classicrankfile);
				fopen_s(&speedrankfile,"speedrank.txt", "w");
				for (int i = 1; i <= 5; i++)
				{
					speedrank.insert(0);
					fprintf(speedrankfile, "0\n");
				}
				fclose(speedrankfile);
                break;
			case BUTTON_SAVE_ID:
				// 存档
				fopen_s(&savefile, "save.txt", "w");
                for (int i = 0; i < BRICK_ROWS; i++)
                {
					for (int j = 0; j < BRICK_COLUMNS; j++)
						fprintf(savefile, "%d ", used[i][j]);
					fprintf(savefile, "\n");
                }
				fprintf(savefile, "%d %d %d %d %d %d %d", score, timer, savetype, saveframe, speedmode, blind, speed);
				fclose(savefile);
				MessageBox(hWnd, L"Save Succeeded!", L"提示", MB_OK);
				break;
            // TODO: 其他按钮
            default:
                break;
            }
        }
    }
}

// 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    mouseX = LOWORD(lParam);
    mouseY = HIWORD(lParam);
    mouseDown = false;
}

// 定时器事件处理函数
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    // 状态更新
    switch (currentStage->stageID)
    {
    case STAGE_START:
        break;
    case STAGE_GAME:
    {
        UpdateBlock(hWnd);
		CheckLine(hWnd);
		IsEnd(hWnd);
        if (speedmode)
            Timer(hWnd);
    }
    break;
    case STAGE_HELP:
		break;
	case STAGE_END:
		break;
	case STAGE_PAUSE:
		break;
	case STAGE_SETTING:
        Setting(hWnd);
        break;
    // TODO：添加多个游戏场景
    default:
        break;
    }

    // 刷新显示
    InvalidateRect(hWnd, NULL, FALSE);
}

#pragma endregion

#pragma region 其它游戏状态处理函数

// 初始化游戏场景函数
void InitStage(HWND hWnd, int stageID)
{
    // 初始化场景实例
    if (currentStage != NULL)
        delete currentStage;
    currentStage = new Stage();
    currentStage->stageID = stageID;

    switch (stageID)
    {
    case STAGE_START:
    {
        currentStage->bg = bmp_WhiteBackground;
        currentStage->pause = false;

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button *button = buttons[i];
            delete button;
        }
        buttons.clear();

        // 添加新的开始按钮
        Button *startButton = CreateButton(
            BUTTON_STARTGAME_ID, bmp_Blank,
            (WINDOW_WIDTH - 376 * 0.75) / 2-200, 275,
            376*0.75, 111*0.75);
        buttons.push_back(startButton);

		// 添加新的帮助按钮
        Button* helpButton = CreateButton(
            BUTTON_HELPGAME_ID, bmp_Blank,
            (WINDOW_WIDTH - 358 * 0.75) / 2-200, 400,
            358 * 0.75, 111 * 0.75);
        buttons.push_back(helpButton);

		// 添加新的设置按钮
		Button* settingButton = CreateButton(
            BUTTON_SETTING_ID, bmp_Blank,
            (WINDOW_WIDTH - 635 * 0.6) / 2-200, 525,
            635 * 0.6, 142 * 0.6);
		buttons.push_back(settingButton);

		// 添加新的退出按钮
		Button* exitButton = CreateButton(
			BUTTON_EXIT_ID, bmp_Blank,
			(WINDOW_WIDTH - 368 * 0.6) / 2+200, 525,
			368 * 0.6, 142 * 0.6);
		buttons.push_back(exitButton);

        // 添加新的继续按钮
		Button* resumeButton = CreateButton(
			BUTTON_RESUME_ID, bmp_Blank,
			(WINDOW_WIDTH - 635 * 0.6) / 2+200, 270,
			635 * 0.6, 153 * 0.6);
		buttons.push_back(resumeButton);

        // 添加新的排行榜按钮
		Button* rankButton = CreateButton(
			BUTTON_RANK_ID, bmp_Blank,
			(WINDOW_WIDTH - 306 * 0.75) / 2 + 200, 400,
			306 * 0.75, 108 * 0.75);
		buttons.push_back(rankButton);
    }
    break;
    case STAGE_GAME:
    {
        currentStage->bg = bmp_GameBackground;
        currentStage->pause = false;

        srand((unsigned)time(NULL));
        int random = rand() % 7;
        int randframe = rand() % 4;

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button *button = buttons[i];
            delete button;
        }
        buttons.clear();

		// 删除旧的方块，创建新的方块
		if (block != NULL)
			delete block;
		if (savetype != -1)
			block = CreateBlock(bmp_Brick, savetype, saveframe);
		else
			block = CreateBlock(bmp_Brick, random, randframe);
    }
    break;
    case STAGE_HELP:
	{
		currentStage->bg = bmp_WhiteBackground;
		currentStage->pause = false;

		// 删除旧的按钮
		for (size_t i = 0; i < buttons.size(); i++)
		{
			Button* button = buttons[i];
			delete button;
		}
		buttons.clear();

		// 添加新的返回按钮
		Button* backButton = CreateButton(
			BUTTON_BACK_ID, bmp_Blank,
			(WINDOW_WIDTH - 324 * 0.75) / 2, 600,
			324 * 0.75, 100 * 0.75);
		buttons.push_back(backButton);
	}
    break;
	case STAGE_END:
	{
		currentStage->bg = bmp_WhiteBackground;
		currentStage->pause = false;

        // 清空存档
        fopen_s(&savefile, "save.txt", "w");
        for (int i = 0; i < BRICK_ROWS; i++)
        {
            for (int j = 0; j < BRICK_COLUMNS; j++)
                fprintf(savefile, "%d ", 0);
            fprintf(savefile, "\n");
        }
        fprintf(savefile, "0 600 -1 -1 0 0 1");
        fclose(savefile);

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button* button = buttons[i];
            delete button;
        }
        buttons.clear();

        // 添加新的重新开始按钮
        Button* restartButton = CreateButton(
            BUTTON_RESTART_ID, bmp_Blank,
            (WINDOW_WIDTH - 690 * 0.5) / 2, 250,
            690 * 0.5, 155 * 0.5);
        buttons.push_back(restartButton);

        // 添加新的退出按钮
        Button* exitButton = CreateButton(
            BUTTON_EXIT_ID, bmp_Blank,
            (WINDOW_WIDTH - 368 * 0.5) / 2, 400,
            368 * 0.5, 142 * 0.5);
        buttons.push_back(exitButton);

        // 排行榜

        // 读入
        speedrank.clear();
		classicrank.clear();
        fopen_s(&classicrankfile, "classicrank.txt", "r");
        for (int i = 1; i <= 5; i++)
        {
            int temp;
            fscanf_s(classicrankfile, "%d", &temp);
            classicrank.insert(temp);
        }
        fclose(classicrankfile);
        fopen_s(&speedrankfile, "speedrank.txt", "r");
        for (int i = 1; i <= 5; i++)
        {
            int temp;
            fscanf_s(speedrankfile, "%d", &temp);
            speedrank.insert(temp);
        }
        fclose(speedrankfile);

		// 录入
        if (avoidrankoverlap == 0)
        {
            if (speedmode)
                speedrank.insert(score);
            else
                classicrank.insert(score);
			avoidrankoverlap = 1;
        }
        
        // 写入
        fopen_s(&classicrankfile, "classicrank.txt", "w");
        int i = 1;
        for (multiset<int>::reverse_iterator it = classicrank.rbegin(); it != classicrank.rend() && i <= 5; it++, i++)
        {
            fprintf(classicrankfile, "%d\n", *it);
        }
        fclose(classicrankfile);
        fopen_s(&speedrankfile, "speedrank.txt", "w");
        i = 1;
        for (multiset<int>::reverse_iterator it = speedrank.rbegin(); it != speedrank.rend() && i <= 5; it++, i++)
        {
            fprintf(speedrankfile, "%d\n", *it);
        }
        fclose(speedrankfile);
	}
	break;
	case STAGE_PAUSE:
	{
		currentStage->bg = bmp_WhiteBackground;
		currentStage->pause = true;

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button* button = buttons[i];
            delete button;
        }
        buttons.clear();

		// 添加新的继续按钮
        Button* resumeButton = CreateButton(
            BUTTON_RESUME_ID, bmp_Blank,
            (WINDOW_WIDTH - 635 * 0.5) / 2, 100,
            635 * 0.5, 153 * 0.5);
        buttons.push_back(resumeButton);

		// 添加新的重新开始按钮
		Button* restartButton = CreateButton(
			BUTTON_RESTART_ID, bmp_Blank,
			(WINDOW_WIDTH - 690 * 0.5) / 2, 200,
			690 * 0.5, 155 * 0.5);
		buttons.push_back(restartButton);

		// 添加新的保存按钮
		Button* saveButton = CreateButton(
			BUTTON_SAVE_ID, bmp_Blank,
			(WINDOW_WIDTH - 341 * 0.5) / 2, 300,
			341 * 0.5, 138 * 0.5);
		buttons.push_back(saveButton);

		// 添加新的退出按钮
		Button* exitButton = CreateButton(
			BUTTON_EXIT_ID, bmp_Blank,
			(WINDOW_WIDTH - 368 * 0.5) / 2, 400,
			368 * 0.5, 142 * 0.5);
		buttons.push_back(exitButton);
	}
	break;
	case STAGE_SETTING:
	{
		currentStage->bg = bmp_WhiteBackground;
		currentStage->pause = false;

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button* button = buttons[i];
            delete button;
        }
        buttons.clear();

        // 添加新的返回按钮
        Button* backButton = CreateButton(
            BUTTON_BACK_ID, bmp_Blank,
            (WINDOW_WIDTH - 324 * 0.75) / 2, 600,
            324 * 0.75, 100 * 0.75);
        buttons.push_back(backButton);
	}
	break;
	case STAGE_RANK:
	{
        currentStage->bg = bmp_WhiteBackground;
        currentStage->pause = false;

        // 删除旧的按钮
        for (size_t i = 0; i < buttons.size(); i++)
        {
            Button* button = buttons[i];
            delete button;
        }
        buttons.clear();

		// 添加新的重置按钮
		Button* resetButton = CreateButton(
			BUTTON_RESET_ID, bmp_Blank,
			(WINDOW_WIDTH - 385 * 0.75) / 2-200, 600,
			385 * 0.75, 108 * 0.75);
		buttons.push_back(resetButton);

		// 添加新的返回按钮
        Button* backButton = CreateButton(
            BUTTON_BACK_ID, bmp_Blank,
            (WINDOW_WIDTH - 324 * 0.75) / 2+200, 600,
            324 * 0.75, 100 * 0.75);
        buttons.push_back(backButton);
	}
	break;
    // TODO：添加多个游戏场景
    default:
        break;
    }

    // 刷新显示
    InvalidateRect(hWnd, NULL, FALSE);
}

// 添加按钮函数
Button *CreateButton(int buttonID, HBITMAP img, int x, int y, int width, int height)
{
    Button *button = new Button();
    button->buttonID = buttonID;
    button->img = img;
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    return button;
}

// 添加方块函数
Block* CreateBlock(HBITMAP img, int type, int frame_index)
{
	Block* block = new Block();
	block->img = img;
	block->type = type;
	block->frame_index = frame_index;
	block->uplft_x = BRICK_COLUMNS / 2 - 1;
    if ((type == 0 && frame_index == 2) || (type == 1 && frame_index == 2) || (type == 2 && frame_index == 2) || (type == 3 && frame_index == 2) || (type == 4 && frame_index == 2) || (type == 6 && frame_index == 0))
        block->uplft_y = 0;
	else if (type == 6 && frame_index == 2)
		block->uplft_y = -1;
	else
        block->uplft_y = 1;
    memcpy(block->shape, shape[type][frame_index], sizeof(shape[type][frame_index]));
	return block;
}

// 消行检测函数
void CheckLine(HWND hWnd)
{
	for (int i = BRICK_ROWS - 2; i > 0; i--)
	{
		bool flag = true;
		for (int j = 1; j < BRICK_COLUMNS - 1; j++)
		{
			if (used[i][j] == 0)
			{
				flag = false;
				break;
			}
		}
		if (flag)
		{
			// 播放音效
			mciSendString(L"seek eliminate to start", NULL, 0, NULL);
            mciSendString(L"play eliminate", NULL, 0, NULL);

			// 消行
			for (int k = i; k > 1; k--)
			{
				for (int j = 1; j < BRICK_COLUMNS - 1; j++)
				{
					used[k][j] = used[k - 1][j];
				}
			}
			for (int j = 1; j < BRICK_COLUMNS - 1; j++)
			{
				used[1][j] = 0;
			}
			score = score + 100*bonus;
		}
	}
}

// 判断是否碰撞
bool CollisionDetection(HWND hWnd,int type)
{
    int temp = 0, tempshape[4][4] = { 0 };
	switch (type)
	{
    case 1:
		temp = block->uplft_y+1;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (block->shape[i][j] == 1)
				{
					if (used[temp + i][block->uplft_x + j] == 1)
						return true;
				}
		return false;
		break;
	case 2:
		temp = block->uplft_x - 1;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (block->shape[i][j] == 1)
				{
					if (used[block->uplft_y + i][temp + j] == 1)
						return true;
				}
		return false;
		break;
	case 3:
		temp = block->uplft_x + 1;
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (block->shape[i][j] == 1)
				{
					if (used[block->uplft_y + i][temp + j] == 1)
						return true;
				}
		return false;
		break;
	case 4:
		temp = block->frame_index;
		temp = (temp + 1) % 4;
        memcpy(tempshape, shape[block->type][temp], sizeof(shape[block->type][temp]));
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (tempshape[i][j] == 1)
				{
					if (used[block->uplft_y + i][block->uplft_x + j] == 1)
						return true;
				}
		return false;
		break;
	default:
		break;
	}
}

// 计算下落位置
int FallPosition(HWND hWnd)
{
	int temp = block->uplft_y;
	bool flag = true;
	while (flag)
	{
        temp++;
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (block->shape[i][j] == 1)
                {
					if (used[temp + i][block->uplft_x + j] == 1)
						flag = false;
                }
	}
	return temp - 1;
}

// 判断是否结束游戏
void IsEnd(HWND hWnd)
{
	for (int i = 1; i < BRICK_COLUMNS - 1; i++)
		if (used[1][i] == 1)
            InitStage(hWnd, STAGE_END);
    return;
}

// 刷新方块状态函数
void UpdateBlock(HWND hWnd)
{
	// 自动下落
	cnt++;
	if (cnt % (6-speed) == 0 ) 
    {
        if (!CollisionDetection(hWnd, 1))
        {
            block->uplft_y++;
            cnt = 0;
        }
        else
        {
			// 播放音效
            mciSendString(L"seek fall to start", NULL, 0, NULL);
            mciSendString(L"play fall", NULL, 0, NULL);

            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (block->shape[i][j] == 1)
                        used[block->uplft_y + i][block->uplft_x + j] = 1;
            delete block;
            block = CreateBlock(bmp_Brick, random, randframe);
            random = rand() % 7;
            randframe = rand() % 4;
            cnt = 0;
        }
	}

	// 位置更新
	if (keyFallDown)
	{
		avoidmultiplefall = 1;
		keyFallDown = false;
		block->uplft_y= FallPosition(hWnd);
        // 播放音效
        mciSendString(L"seek fall to start", NULL, 0, NULL);
        mciSendString(L"play fall", NULL, 0, NULL);

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (block->shape[i][j] == 1)
                    used[block->uplft_y + i][block->uplft_x + j] = 1;
        delete block;
        block = CreateBlock(bmp_Brick, random, randframe);
        random = rand() % 7;
        randframe = rand() % 4;
        cnt = 0;
	}
	if (keyDownDown&& !CollisionDetection(hWnd,1))
	{
	    block->uplft_y++;
	}
	if (keyLeftDown&& !CollisionDetection(hWnd,2))
	{
	    block->uplft_x--;
	}
	if (keyRightDown&& !CollisionDetection(hWnd,3))
	{
	    block->uplft_x++;
	}

	// 旋转
    int tempx = block->uplft_x;
	if (keyRotate)
	{
        if (!CollisionDetection(hWnd, 4))
        {
            block->frame_index = (block->frame_index + 1) % 4;
            memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
            keyRotate = false;
        }
        else
        {
            block->uplft_x = tempx + 1;
            if (!CollisionDetection(hWnd, 4))
            {
                block->frame_index = (block->frame_index + 1) % 4;
                memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                keyRotate = false;
			}
            else
            {
                block->uplft_x = tempx - 1;
                if (!CollisionDetection(hWnd, 4))
                {
                    block->frame_index = (block->frame_index + 1) % 4;
                    memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                    keyRotate = false;
                }
                else
                {
                    block->uplft_x = tempx + 2;
                    if (!CollisionDetection(hWnd, 4))
                    {
                        block->frame_index = (block->frame_index + 1) % 4;
                        memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                        keyRotate = false;
                    }
                    else
                    {
                        block->uplft_x = tempx - 2;
                        if (!CollisionDetection(hWnd, 4))
                        {
                            block->frame_index = (block->frame_index + 1) % 4;
                            memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                            keyRotate = false;
                        }
                        else
                        {
                            block->uplft_x = tempx + 3;
                            if (!CollisionDetection(hWnd, 4))
                            {
                                block->frame_index = (block->frame_index + 1) % 4;
                                memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                                keyRotate = false;
                            }
                            else
                            {
                                block->uplft_x = tempx - 3;
                                if (!CollisionDetection(hWnd, 4))
                                {
                                    block->frame_index = (block->frame_index + 1) % 4;
                                    memcpy(block->shape, shape[block->type][block->frame_index], sizeof(shape[block->type][block->frame_index]));
                                    keyRotate = false;
                                }
                                else
									block->uplft_x = tempx;
                            }
                        }
                    }
                }
            }
        }
	}
}

// 设置函数
void Setting(HWND hWnd)
{
	if (keyLeftDown)
	{
		if(speed<5) speed++;
		keyLeftDown = false;
	}
	if (keyRightDown)
	{
		if (speed > 1) speed--;
		keyRightDown = false;
	}
	if (keyDownDown)
	{
        if (bgm > 0) bgm -= 1;
		keyDownDown = false;
	}
	if (keyRotate)
	{
		if (bgm < 10) bgm += 1;
		keyRotate = false;
	}
	if (keyQDown)
	{
        if (effects < 10) effects++;
		keyQDown = false;
	}
	if (keyEDown)
	{
		if (effects > 0) effects--;
		keyEDown = false;
	}
	if (keyFallDown)
	{
        blind = !blind;
		keyFallDown = false;
	}
	if (keyTDown)
	{
		speedmode = !speedmode;
		keyTDown = false;
	}
	bonus = 1 + (speed - 1) * 0.5;
    wchar_t temp[100];
    wsprintf(temp, L"setaudio bgm volume to %d", bgm*50);
    mciSendString(temp, NULL, 0, NULL);
	wsprintf(temp, L"setaudio fall volume to %d", effects * 100);
	mciSendString(temp, NULL, 0, NULL);
}

// 计时器函数
void Timer(HWND hWnd)
{
    timer--;
    if(timer==0)
		InitStage(hWnd, STAGE_END);
}
#pragma endregion

#pragma region 绘图函数

// 绘图函数
void Paint(HWND hWnd)
{
    // 开始绘制
    PAINTSTRUCT ps;
    HDC hdc_window = BeginPaint(hWnd, &ps);

    // 创建缓存
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
    HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);

    // 初始化缓存
    HBITMAP blankBmp = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, blankBmp);

    // 绘制背景到缓存
    SelectObject(hdc_loadBmp, currentStage->bg);
    BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);

    // 按场景分类绘制内容到缓存
    switch (currentStage->stageID)
    {
    case STAGE_START:
    {
        // 绘制标题
        SelectObject(hdc_loadBmp, bmp_Tetris);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH-439)/2, 100,
            439, 116,
            hdc_loadBmp, 0, 0, 878, 232,
            RGB(255, 255, 255));

		// 绘制开始按钮
        SelectObject(hdc_loadBmp, bmp_Start);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 376*0.75) / 2-200, 275,
            376*0.75, 111*0.75,
            hdc_loadBmp, 0, 0, 752, 222,
            RGB(255, 255, 255));

		// 绘制帮助按钮
        SelectObject(hdc_loadBmp, bmp_Help);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 358 * 0.75) / 2-200, 400,
            358 * 0.75, 111 * 0.75,
            hdc_loadBmp, 0, 0, 716, 222,
            RGB(255, 255, 255));

		// 绘制设置按钮
		SelectObject(hdc_loadBmp, bmp_Setting);
		TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 635 * 0.6) / 2-200, 525,
            635 * 0.6, 142 * 0.6,
            hdc_loadBmp, 0, 0, 635, 142,
            RGB(255, 255, 255));

		// 绘制退出按钮
		SelectObject(hdc_loadBmp, bmp_Exit);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 368 * 0.6) / 2+200, 525,
			368 * 0.6, 142 * 0.6,
			hdc_loadBmp, 0, 0, 368, 142,
			RGB(255, 255, 255));

        // 绘制排行榜按钮
		SelectObject(hdc_loadBmp, bmp_Rank);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 306 * 0.75) / 2+200, 400,
			306 * 0.75, 108 * 0.75,
			hdc_loadBmp, 0, 0, 306, 108,
			RGB(255, 255, 255));

        // 绘制继续按钮
		SelectObject(hdc_loadBmp, bmp_Resume);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 635 * 0.6) / 2+200, 270,
			635 * 0.6, 153 * 0.6,
			hdc_loadBmp, 0, 0, 635, 153,
			RGB(255, 255, 255));
    }
    break;
    case STAGE_GAME:
    {
		// 绘制墙
        for (int i = 0; i <= BRICK_COLUMNS - 1; i++)
        {
            used[0][i] = 1;
            used[BRICK_ROWS - 1][i] = 1;
        }
        for (int i = 0; i <= BRICK_ROWS - 1; i++)
        {
            used[i][0] = 1;
            used[i][BRICK_COLUMNS - 1] = 1;
        }

        // 绘制底部
        if (!blind)
        {
            for (int i = 1; i < BRICK_ROWS - 1; i++)
                for (int j = 1; j < BRICK_COLUMNS - 1; j++)
                {
                    if (used[i][j] == 1)
                    {
                        SelectObject(hdc_loadBmp, bmp_Brick);
                        TransparentBlt(
                            hdc_memBuffer, j * BG_CELL_WIDTH, i * BG_CELL_HEIGHT,
                            BG_CELL_WIDTH, BG_CELL_HEIGHT,
                            hdc_loadBmp, 0, 0, BRICK_CELL_WIDTH, BRICK_CELL_WIDTH,
                            RGB(255, 255, 255));
                    }
                }
        }

        // 绘制方块下落预览
        if (!blind)
        {
            SelectObject(hdc_loadBmp, bmp_Preview);
            int temp = FallPosition(hWnd);
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (shape[block->type][block->frame_index][i][j] == 1)
                        TransparentBlt(
                            hdc_memBuffer, (block->uplft_x + j) * BG_CELL_WIDTH, (temp + i) * BG_CELL_HEIGHT,
                            BG_CELL_WIDTH, BG_CELL_HEIGHT,
                            hdc_loadBmp, 0, 0, BRICK_CELL_WIDTH, BRICK_CELL_WIDTH,
                            RGB(255, 255, 255));
        }

		// 绘制方块
		SelectObject(hdc_loadBmp, block->img);
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				if (block->shape[i][j] == 1)
				{
					TransparentBlt(
						hdc_memBuffer, (block->uplft_x + j) * BG_CELL_WIDTH, (block->uplft_y + i) * BG_CELL_HEIGHT,
						BG_CELL_WIDTH, BG_CELL_HEIGHT,
						hdc_loadBmp, 0, 0, BRICK_CELL_WIDTH, BRICK_CELL_WIDTH,
						RGB(255, 255, 255));
				}
        
		// 绘制分数
        wchar_t str[100];
        wsprintf(str, L"Score : %d", score);
        TextDraw(hdc_memBuffer, 400, 300, str, 80, 400);

		// 绘制时间
        if (speedmode)
        {
            wchar_t time[100];
            int second = timer / 10;
            int millisecond = timer % 10;
			wsprintf(time, L"Time : %d.%d", second, millisecond);
			TextDraw(hdc_memBuffer, 400, 400, time, 80, 400);
        }

		// 绘制下一个方块
		SelectObject(hdc_loadBmp, bmp_Brick);
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (shape[random][0][i][j] == 1)
                    TransparentBlt(
                        hdc_memBuffer, (BRICK_COLUMNS + 4 + j) * BG_CELL_WIDTH, (3 + i) * BG_CELL_HEIGHT,
                        BG_CELL_WIDTH, BG_CELL_HEIGHT,
                        hdc_loadBmp, 0, 0, BRICK_CELL_WIDTH, BRICK_CELL_WIDTH,
                        RGB(255, 255, 255));
    }
    break;
    case STAGE_HELP:
    {
        // 绘制帮助文字
        SelectObject(hdc_loadBmp, bmp_HelpWords);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 1243 * 0.75) / 2, 100,
            1243 * 0.75, 577 * 0.75,
            hdc_loadBmp, 0, 0, 1243, 577,
            RGB(255, 255, 255));

		// 绘制返回按钮
		SelectObject(hdc_loadBmp, bmp_Back);
		TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 324 * 0.75) / 2, 600,
            324 * 0.75, 100 * 0.75,
            hdc_loadBmp, 0, 0, 648, 201,
            RGB(255, 255, 255));
    }
	break;
    case STAGE_PAUSE:
    {
		// 绘制继续按钮
		SelectObject(hdc_loadBmp, bmp_Resume);
		TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 635 * 0.5) / 2, 100,
            635 * 0.5, 153 * 0.5,
            hdc_loadBmp, 0, 0, 635, 153,
            RGB(255, 255, 255));

		// 绘制重新开始按钮
		SelectObject(hdc_loadBmp, bmp_Restart);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 690 * 0.5) / 2, 200,
			690 * 0.5, 155 * 0.5,
			hdc_loadBmp, 0, 0, 690, 155,
			RGB(255, 255, 255));

		// 绘制保存按钮
		SelectObject(hdc_loadBmp, bmp_Save);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 341 * 0.5) / 2, 300,
			341 * 0.5, 138 * 0.5,
			hdc_loadBmp, 0, 0, 341, 138,
			RGB(255, 255, 255));

		// 绘制退出按钮
		SelectObject(hdc_loadBmp, bmp_Exit);
		TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 368 * 0.5) / 2, 400,
            368 * 0.5, 142 * 0.5,
            hdc_loadBmp, 0, 0, 368, 142,
            RGB(255, 255, 255));
    }
	break;
	case STAGE_SETTING:
    {
        // 绘制返回按钮
        SelectObject(hdc_loadBmp, bmp_Back);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 324 * 0.75) / 2, 600,
            324 * 0.75, 100 * 0.75,
            hdc_loadBmp, 0, 0, 648, 201,
            RGB(255, 255, 255));

        // 绘制提示文字
        SelectObject(hdc_loadBmp, bmp_SettingWords);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 1683 * 0.5) / 2, 50,
            1683 * 0.5, 404 * 0.5,
            hdc_loadBmp, 0, 0, 1683, 404,
            RGB(255, 255, 255));

		// 绘制速度
        wchar_t speed_w[100],bgm_w[100],effects_w[100];
        wsprintf(speed_w, L"Speed : %d", speed);
		TextDraw(hdc_memBuffer, 200, 275, speed_w, 60,400);

		// 绘制系数
        switch (speed)
        {
        case 1:
            TextDraw(hdc_memBuffer, 600, 275, L"Bonus : 1.0", 60,400);
            break;
        case 2:
            TextDraw(hdc_memBuffer, 600, 275, L"Bonus : 1.5", 60,400);
            break;
        case 3:
            TextDraw(hdc_memBuffer, 600, 275, L"Bonus : 2.0", 60,400);
            break;
        case 4:
            TextDraw(hdc_memBuffer, 600, 275, L"Bonus : 2.5", 60,400);
            break;
        case 5:
            TextDraw(hdc_memBuffer, 600, 275, L"Bonus : 3.0", 60,400);
            break;
        default:
            break;
        }

        // 绘制背景音乐音量
		wsprintf(bgm_w, L"BGM : %d", bgm);
		TextDraw(hdc_memBuffer, 200, 350, bgm_w, 60,400);

		// 绘制音效音量
		wsprintf(effects_w, L"Effects : %d", effects);
		TextDraw(hdc_memBuffer, 600, 350, effects_w, 60,400);

        // 绘制盲打模式开关
		if (blind)
			TextDraw(hdc_memBuffer, 200, 425, L"Blind : ON (Press Space)", 60,800);
		else
			TextDraw(hdc_memBuffer, 200, 425, L"Blind : OFF (Press Space)", 60,800);

		// 绘制速度模式开关
		if (speedmode)
			TextDraw(hdc_memBuffer, 200, 500, L"Mode : Speed (Press T)", 60, 800);
		else
			TextDraw(hdc_memBuffer, 200, 500, L"Mode : Classic (Press T)", 60, 800);
    }
	break;
	case STAGE_END:
    {
        // 绘制分数
        wchar_t str[100];
        wsprintf(str, L"Score : %d", score);
		TextDraw(hdc_memBuffer, 400, 100, str, 80,400);

        // 绘制重新开始按钮
        SelectObject(hdc_loadBmp, bmp_Restart);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 690 * 0.5) / 2, 250,
            690 * 0.5, 155 * 0.5,
            hdc_loadBmp, 0, 0, 690, 155,
            RGB(255, 255, 255));

        // 绘制退出按钮
        SelectObject(hdc_loadBmp, bmp_Exit);
        TransparentBlt(
            hdc_memBuffer, (WINDOW_WIDTH - 368 * 0.5) / 2, 400,
            368 * 0.5, 142 * 0.5,
            hdc_loadBmp, 0, 0, 368, 142,
            RGB(255, 255, 255));
    }
	break;
	case STAGE_RANK:
	{
		// 绘制返回按钮
		SelectObject(hdc_loadBmp, bmp_Back);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 324 * 0.75) / 2 + 200, 600,
			324 * 0.75, 100 * 0.75,
			hdc_loadBmp, 0, 0, 648, 201,
			RGB(255, 255, 255));

		// 绘制重置按钮
		SelectObject(hdc_loadBmp, bmp_Reset);
		TransparentBlt(
			hdc_memBuffer, (WINDOW_WIDTH - 385 * 0.75) / 2 - 200, 600,
			385 * 0.75, 108 * 0.75,
			hdc_loadBmp, 0, 0, 385, 108,
			RGB(255, 255, 255));

		// 绘制排行榜
		wchar_t rank[100];
        int temp;

        fopen_s(&classicrankfile, "classicrank.txt", "r");
		wsprintf(rank, L"Classic Mode :");
		TextDraw(hdc_memBuffer, 200, 100, rank, 60, 400);
		for (int i = 1; i <= 5; i++)
		{
			fscanf_s(classicrankfile, "%d", &temp);
            wsprintf(rank, L"%d. %d", i, temp);
            TextDraw(hdc_memBuffer, 200, 100 + i * 75, rank, 60, 400);
		}
		fclose(classicrankfile);

		fopen_s(&speedrankfile, "speedrank.txt", "r");
		wsprintf(rank, L"Speed Mode :");
		TextDraw(hdc_memBuffer, 600, 100, rank, 60, 400);
        for (int i = 1; i <= 5; i++)
        {
            fscanf_s(speedrankfile, "%d", &temp);
            wsprintf(rank, L"%d. %d", i, temp);
            TextDraw(hdc_memBuffer, 600, 100 + i * 75, rank, 60, 400);
        }
		fclose(speedrankfile);
	}
    // TODO：添加多个游戏场景
    default:
        break;
    }

    // 绘制按钮到缓存
    for (int i = 0; i < buttons.size(); i++)
    {
        Button *button = buttons[i];
        SelectObject(hdc_loadBmp, button->img);
        TransparentBlt(
            hdc_memBuffer, button->x, button->y,
            button->width, button->height,
            hdc_loadBmp, 0, 0, button->width, button->height,
            RGB(255, 255, 255));
    }

    // 最后将所有的信息绘制到屏幕上
    BitBlt(hdc_window, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_memBuffer, 0, 0, SRCCOPY);

    // 回收资源所占的内存（非常重要）
    DeleteObject(blankBmp);
    DeleteDC(hdc_loadBmp);
    DeleteDC(hdc_memBuffer);

    // 结束绘制
    EndPaint(hWnd, &ps);
}

// 创建纯白背景图像
HBITMAP CreateWhiteBackground(HWND hWnd)
{
    // 开始绘制
    PAINTSTRUCT ps;
    HDC hdc_window = BeginPaint(hWnd, &ps);

    // 创建缓存
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);

    // 初始化缓存
    HBITMAP bmp_output = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, bmp_output);

    // 创建纯白色画刷
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));

    // 创建一个矩形
    RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};

    // 将缓存填充为白色
    FillRect(hdc_memBuffer, &rect, hBrush);

    // 回收资源所占的内存（非常重要）
    DeleteObject(hBrush);
    DeleteDC(hdc_memBuffer);

    // 结束绘制
    EndPaint(hWnd, &ps);

    return bmp_output;
}

HBITMAP CreateGameBackground(HWND hWnd)
{
    // 开始绘制
    PAINTSTRUCT ps;
    HDC hdc_window = BeginPaint(hWnd, &ps);

    // 创建缓存
    HDC hdc_memBuffer = CreateCompatibleDC(hdc_window);
    HDC hdc_loadBmp = CreateCompatibleDC(hdc_window);

    // 初始化缓存
    HBITMAP bmp_output = CreateCompatibleBitmap(hdc_window, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(hdc_memBuffer, bmp_output);

    // 初始化为白色背景
    SelectObject(hdc_loadBmp, bmp_WhiteBackground);
    BitBlt(hdc_memBuffer, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);

    // 画四周石头
    SelectObject(hdc_loadBmp, bmp_Wall);
    for (int i = 0; i < BG_ROWS; i++)
    {
        BitBlt(hdc_memBuffer, 0, i * BG_CELL_HEIGHT, BG_CELL_WIDTH, BG_CELL_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
        BitBlt(hdc_memBuffer, (BG_COLUMNS - 1) * BG_CELL_WIDTH, i * BG_CELL_HEIGHT, BG_CELL_WIDTH, BG_CELL_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
    }
    for (int j = 0; j < BG_COLUMNS; j++)
    {
        BitBlt(hdc_memBuffer, j * BG_CELL_WIDTH, 0, BG_CELL_WIDTH, BG_CELL_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
        BitBlt(hdc_memBuffer, j * BG_CELL_WIDTH, (BG_ROWS - 1) * BG_CELL_HEIGHT, BG_CELL_WIDTH, BG_CELL_HEIGHT, hdc_loadBmp, 0, 0, SRCCOPY);
    }

    // 回收资源所占的内存（非常重要）
    DeleteDC(hdc_loadBmp);
    DeleteDC(hdc_memBuffer);

    // 结束绘制
    EndPaint(hWnd, &ps);

    return bmp_output;
}

void TextDraw(HDC hdc, int x, int y, const TCHAR* text, int size,int length)
{
    // 创建一个字体，设置高度为 size（单位是逻辑单元）
    HFONT hFont = CreateFont(
        size,                                 // 字体高度
        0,                                    // 字体宽度（0 表示自动计算）
        0,                                    // 字体的倾斜角度
        0,                                    // 字体的基线倾斜角度
        FW_NORMAL,                            // 字体的粗细
        FALSE,                                // 是否斜体
        FALSE,                                // 是否下划线
        FALSE,                                // 是否删除线
        DEFAULT_CHARSET,                      // 字符集
        OUT_DEFAULT_PRECIS,                   // 输出精度
        CLIP_DEFAULT_PRECIS,                  // 剪裁精度
        DEFAULT_QUALITY,                      // 输出质量
        DEFAULT_PITCH | FF_SWISS,             // 字体家族和间距
        TEXT("Fredericka the Great")          // 字体名称
    );
    // 选择自定义字体到设备上下文
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    // 设置字体区域
    RECT rect = {
        x,
        y,
        x + length,
        y + 300 };
    // 绘制
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    DrawText(hdc, text, -1, &rect, DT_CENTER);
    // 恢复原来的字体
    SelectObject(hdc, hOldFont);
    // 删除自定义字体以释放资源
    DeleteObject(hFont);
}
#pragma endregion