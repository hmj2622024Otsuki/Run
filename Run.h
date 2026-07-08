#pragma once

// 構造体の宣言

struct OBJECT
{
	int x;
	int y;
	int vx;
	int vy;
	int state;
	int image;
	int timer;
};

// 関数プロトタイプ宣言
// ここにプロトタイプ宣言を記述する

int LoadGraphWithCheck(const char* file);
int LoadSoundMemWithCheck(const char* file);
void DrawText(int x, int y, const char* txt, int val, int col, int siz);
void DrawTextC(int x, int y, const char* txt, int col, int siz);