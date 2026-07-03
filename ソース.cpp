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
	const int BLACK = GetColor(0, 0, 0); // 黒

	float jumpPower = -20.0f;
	float gravity = 1.8f;
	bool jump = false;

	SetWindowText("Run"); // ウィンドウのタイトル

	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 255, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	int timer = 0;

	// 画像の用意
	int imgPlayer[4] = { // 配列にプレイヤー(女の子)の画像を読み込む
		LoadGraph("image/run01.png"),
		LoadGraph("image/run02.png"),
		LoadGraph("image/run03.png"),
		LoadGraph("image/run04.png")
	};
	int imgPlayerJump = LoadGraph("image/jump.png"); // プレイヤーのジャンプ画像を読み込む
	int imgBG = LoadGraph("image/bg.png"); // 変数に背景画像を読み込む
	int imgGround = LoadGraph("image/ground.png"); // 変数に地面の画像を読み込む

	// 効果音・BGMの用意
	int jumpSE = LoadSoundMem("sound/jump.mp3");
	int overSE = LoadSoundMem("sound/over.mp3");

	// プレイヤーの座標用の変数
	float playerX = 200, playerY = 400;
	float playerVY = 0.0f;

	while (1)
	{
		ClearDrawScreen(); // 画面をクリアする

		// 背景
		int spd = 1; // スクロール速度の初期値
		static int bgX;
		bgX = (bgX - spd * 2) % WIDTH;
		DrawGraph(bgX + WIDTH, 0, imgBG, false); // 背景の表示
		DrawGraph(bgX, 0, imgBG, false); //

		// 地面
		static int groundX;
		groundX = (groundX - spd * 9) % WIDTH;
		DrawGraph(groundX + WIDTH, 0, imgGround, true); // 地面の表示
		DrawGraph(groundX, 0, imgGround, true); //

		// プレイヤー
		if (jump == true)
		{
			DrawGraph(playerX, playerY, imgPlayerJump, true);
		}
		else
		{
			DrawGraph(playerX, playerY, imgPlayer[(timer / 3) % 4], true); // プレイヤーの表示
		}

		timer++; // 時間のカウント
		DrawFormatString(0, 0, BLACK, "%d", timer);

		// ジャンプの処理
		if (CheckHitKey(KEY_INPUT_SPACE) == 1 && !jump)
		{
			playerVY = jumpPower;
			jump = true;
			PlaySoundMem(jumpSE, DX_PLAYTYPE_BACK);
		}

		if (jump == true)
		{
			playerVY += gravity;
			playerY += playerVY;

			if (playerY >= 400.0f)
			{
				playerY = 400.0f;
				playerVY = 0.0f;
				jump = false;
			}
		}



		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(33); // 一定時間待つ
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}