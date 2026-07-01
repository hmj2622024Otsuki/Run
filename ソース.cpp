#include "DxLib.h"
#include "stdlib.h"

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance, // 現在のインスタンスのハンドル
	_In_opt_ HINSTANCE hPrevInstance, // 前のインスタンスのハンドル
	_In_ LPSTR lpCmdLine, // コマンドライン引数
	_In_ int nCmdShow // ウィンドウの表示状態
)

{
	// 定数の宣言
	const int WIDTH = 960, HEIGHT = 640; // ウィンドウの幅と高さのピクセル数
	const int WHITE = GetColor(255, 255, 255); // 白
	const int RED = GetColor(255, 0, 0); // 赤

	// 関数の宣言
	int timer = 0;


	SetWindowText("Run"); // ウィンドウのタイトル

	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 255, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	while (1)
	{
		ClearDrawScreen(); // 図形をマウスポインタの位置にする
		timer++;

		int size = 200;
		SetFontSize(size);
		if (timer % 30 < 15)
		{
			DrawString(WIDTH / 2 - size - 50, HEIGHT / 2 - size + 100, "アホ！", RED);
		}

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(33); // 一定時間待つ
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}