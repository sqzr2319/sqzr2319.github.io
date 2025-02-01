#pragma once

#pragma region 头文件引用

// 平台版本头文件
#include "targetver.h"

// Windows 头文件
#define WIN32_LEAN_AND_MEAN // 从 Windows 头文件中排除极少使用的内容
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>

// 资源头文件
#include "resource.h"
#pragma comment(lib, "Msimg32.lib") // 解决报错：无法解析的外部符号 __imp_TransparentBlt

// TODO: 在此处引用程序需要的其他头文件
#include <cmath>
#include <vector>
#include <set>

#pragma endregion

#pragma region 宏定义

// 窗体
#define WINDOW_TITLEBARHEIGHT 32 // 标题栏高度
#define WINDOW_WIDTH 1200        // 游戏窗口宽度
#define WINDOW_HEIGHT 800        // 游戏窗口高度

// 计时器
#define TIMER_GAMETIMER 1          // 游戏的默认计时器ID
#define TIMER_GAMETIMER_ELAPSE 100  // 默认计时器刷新间隔的毫秒数

// 游戏阶段
#define STAGE_START 0   // 开始画面的ID
#define STAGE_GAME 1    // 第一个游戏场景的ID
#define STAGE_HELP 2    // 帮助画面的ID
#define STAGE_END 3     // 结束画面的ID
#define STAGE_PAUSE 4   // 暂停画面的ID
#define STAGE_SETTING 5 // 设置画面的ID
#define STAGE_RANK 6    // 排行榜画面的ID

// 背景
#define BG_CELL_WIDTH 30                       // 背景单格宽度
#define BG_CELL_HEIGHT 30                      // 背景单格高度
#define BG_COLUMNS 12						   // 背景沿X方向的单元个数
#define BG_ROWS 22							   // 背景沿Y方向的单元个数

// 砖块
#define BRICK_CELL_WIDTH 100                          // 砖块单格宽度
#define BRICK_CELL_HEIGHT 100                         // 砖块单格高度
#define BRICK_COLUMNS 12							  // 砖块沿X方向的单元个数
#define BRICK_ROWS 22								  // 砖块沿Y方向的单元个数

// 按钮
#define BUTTON_STARTGAME_ID 1001                                              // 开始游戏按钮ID
#define BUTTON_HELPGAME_ID 1002											      // 帮助游戏按钮ID
#define BUTTON_BACK_ID 1003													  // 返回按钮ID
#define BUTTON_SETTING_ID 1004												  // 设置按钮ID
#define BUTTON_RESUME_ID 1005												  // 继续按钮ID
#define BUTTON_EXIT_ID 1006													  // 退出按钮ID
#define BUTTON_RESTART_ID 1007												  // 重新开始按钮ID
#define BUTTON_RANK_ID 1008													  // 排行榜按钮ID
#define BUTTON_RESET_ID 1009												  // 重置按钮ID
#define BUTTON_SAVE_ID 1010													  // 保存按钮ID

// TODO: 添加游戏需要的更多宏定义

#pragma endregion

#pragma region 结构体声明

// 场景结构体
struct Stage
{
    int stageID; // 场景编号
    HBITMAP bg;  // 背景图片
    bool pause;  // 游戏是否被暂停（计时器是否运行）

    // TODO: 其他需要记录的信息
};

// 按钮结构体
struct Button
{
    int buttonID; // 按钮编号
    HBITMAP img;  // 图片
    int x;        // 坐标x
    int y;        // 坐标y
    int width;    // 宽度
    int height;   // 高度
};

// 砖块结构体
struct Brick
{
    HBITMAP img;     // 图片
    int frame_index; // 当前显示的是帧序列的第几个
    int cell_x;      // 单元坐标x
    int cell_y;      // 单元坐标y
};

struct Block
{
    HBITMAP img;     // 图片
	int type;        // 类型
    int frame_index; // 当前显示的是帧序列的第几个
    int uplft_x;     // 单元坐标x
    int uplft_y;     // 单元坐标y
	int shape[4][4]; // 形状
};
// TODO: 添加游戏需要的更多结构体

// 方块常量
const int shape[7][4][4][4] = 
{
	{
		{
			{0,1,0,0},
			{1,1,1,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{0,1,1,0},
			{0,1,0,0},
			{0,0,0,0}
		},

		{
			{0,0,0,0},
			{1,1,1,0},
			{0,1,0,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{1,1,0,0},
			{0,1,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{0,0,1,0},
			{1,1,1,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{0,1,0,0},
			{0,1,1,0},
			{0,0,0,0}
		},

		{
			{0,0,0,0},
			{1,1,1,0},
			{1,0,0,0},
			{0,0,0,0}
		},

		{
			{1,1,0,0},
			{0,1,0,0},
			{0,1,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{1,0,0,0},
			{1,1,1,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,1,1,0},
			{0,1,0,0},
			{0,1,0,0},
			{0,0,0,0}
		},

		{
			{0,0,0,0},
			{1,1,1,0},
			{0,0,1,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{0,1,0,0},
			{1,1,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{0,1,1,0},
			{1,1,0,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{0,1,1,0},
			{0,0,1,0},
			{0,0,0,0}
		},

		{
			{0,0,0,0},
			{0,1,1,0},
			{1,1,0,0},
			{0,0,0,0}
		},

		{
			{1,0,0,0},
			{1,1,0,0},
			{0,1,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{1,1,0,0},
			{0,1,1,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,0,1,0},
			{0,1,1,0},
			{0,1,0,0},
			{0,0,0,0}
		},

		{
			{0,0,0,0},
			{1,1,0,0},
			{0,1,1,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{1,1,0,0},
			{1,0,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{1,1,0,0},
			{1,1,0,0},
			{0,0,0,0},
			{0,0,0,0}
		},
	
		{
			{1,1,0,0},
			{1,1,0,0},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{1,1,0,0},
			{1,1,0,0},
			{0,0,0,0},
			{0,0,0,0}
		},
	
		{
			{1,1,0,0},
			{1,1,0,0},
			{0,0,0,0},
			{0,0,0,0}
		}
	},

	{
		{
			{0,0,0,0},
			{1,1,1,1},
			{0,0,0,0},
			{0,0,0,0}
		},

		{
			{0,1,0,0},
			{0,1,0,0},
			{0,1,0,0},
			{0,1,0,0}
		},

		{
			{0,0,0,0},
			{0,0,0,0},
			{1,1,1,1},
			{0,0,0,0}
		},

		{
			{0,0,1,0},
			{0,0,1,0},
			{0,0,1,0},
			{0,0,1,0}
		}
	}
};
#pragma endregion

#pragma region 事件处理函数声明

// 初始化游戏窗体函数
void InitGame(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 键盘按下事件处理函数
void KeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 键盘松开事件处理函数
void KeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标移动事件处理函数
void MouseMove(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标左键按下事件处理函数
void LButtonDown(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 鼠标左键松开事件处理函数
void LButtonUp(HWND hWnd, WPARAM wParam, LPARAM lParam);

// 定时器事件处理函数
void TimerUpdate(HWND hWnd, WPARAM wParam, LPARAM lParam);

// TODO: 添加游戏需要的更多事件处理函数

#pragma endregion

#pragma region 其它游戏状态处理函数声明

// 初始化场景函数
void InitStage(HWND hWnd, int stageID);

// 添加按钮函数
Button *CreateButton(int buttonID, HBITMAP img, int width, int height, int x, int y);

// 添加方块函数
Block* CreateBlock(HBITMAP img, int type, int frame_index);

// 刷新方块状态函数
void UpdateBlock(HWND hWnd);

// 碰撞检测函数
bool CollisionDetection(HWND hWnd,int type);

// 下落位置计算函数
int FallPosition(HWND hWnd);

// 消行检测函数
void CheckLine(HWND hWnd);

// 判断是否结束函数
void IsEnd(HWND hWnd);

// 设置函数
void Setting(HWND hWnd);

// 计时器函数
void Timer(HWND hWnd);
// TODO: 添加游戏需要的更多函数

#pragma endregion

#pragma region 绘图函数声明

// 绘图函数
void Paint(HWND hWnd);

// 创建纯白背景图像
HBITMAP CreateWhiteBackground(HWND hWnd);

// 创建游戏背景资源
HBITMAP CreateGameBackground(HWND hWnd);

// 绘制文字函数
void TextDraw(HDC hdc, int x, int y, const TCHAR* text, int size, int length);

// TODO: 添加游戏需要的更多绘图函数

#pragma endregion
