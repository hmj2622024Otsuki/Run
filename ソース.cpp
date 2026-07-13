#include "DxLib.h"
#include "stdlib.h"
#include "math.h"
#include "time.h"
#include "Run.h" // ヘッダーファイルをインクルード

// 定数の宣言
const int WIDTH = 960, HEIGHT = 640; // ウィンドウの幅と高さのピクセル数
const int WHITE = GetColor(255, 255, 255); // 白
const int RED = GetColor(255, 0, 0); // 赤
const int BLACK = GetColor(0, 0, 0); // 黒
const int BULLET_MAX = 100; // プレイヤーが発射する弾の弾数制限
enum { TITLE, PLAY, OVER }; // シーンを分けるための列挙定数

// 変数の宣言
float jumpPower = -25.0f;
float gravity = 2.5f;
bool jump = false;
bool over = false;
static int addScore = 0;

int APIENTRY WinMain(
	_In_ HINSTANCE hInstance, // 現在のインスタンスのハンドル
	_In_opt_ HINSTANCE hPrevInstance, // 前のインスタンスのハンドル
	_In_ LPSTR lpCmdLine, // コマンドライン引数
	_In_ int nCmdShow // ウィンドウの表示状態
)

{
	SetWindowText("Run"); // ウィンドウのタイトル

	SetGraphMode(WIDTH, HEIGHT, 32); // ウィンドウの大きさとカラービット数の指定
	ChangeWindowMode(true); // ウィンドウモードで起動
	if (DxLib_Init() == -1) return -1; // ライブラリ初期化 エラーが起きたら終了
	SetBackgroundColor(0, 255, 0); // 背景色の指定
	SetDrawScreen(DX_SCREEN_BACK); // 描画面を裏画面にする

	int score = 0;
	int HighScore = 101000;
	int timer = 0;
	int setSpeed = 8;
	int scene = TITLE;
	int scrollSpeed = 24;

	// 画像の用意

	// プレイヤーの画像
	int imgPlayer[4] = {
		LoadGraphWithCheck("image/run01.png"),
		LoadGraphWithCheck("image/run02.png"),
		LoadGraphWithCheck("image/run03.png"),
		LoadGraphWithCheck("image/run04.png")
	};
	int imgPlayerJump = LoadGraphWithCheck("image/jump.png"); // プレイヤーのジャンプ画像
	int imgPlayerOver = LoadGraphWithCheck("image/over.png"); // プレイヤーのゲームオーバー時の画像
	int imgPlayerJumpOver = LoadGraphWithCheck("image/over2.png"); // プレイヤーのゲームオーバー時の画像(2)

	int imgBG = LoadGraphWithCheck("image/bg.png"); // 背景画像を読み込む
	int imgGround = LoadGraphWithCheck("image/ground.png"); // 地面の画像を読み込む
	int imgGreen = LoadGraphWithCheck("image/green.png"); // 木々の画像を読み込む

	// 敵の画像
	int imgEnemy1[3] = {
		LoadGraphWithCheck("image/enemy01.png"),
		LoadGraphWithCheck("image/enemy02.png"),
		LoadGraphWithCheck("image/enemy03.png")
	};

	// 効果音・BGMを読み込む
	int bgm = LoadSoundMemWithCheck("sound/bgm.mp3"); // BGM
	int jumpSE = LoadSoundMemWithCheck("sound/jump.mp3"); // ジャンプの効果音
	int overSE = LoadSoundMemWithCheck("sound/over.mp3"); // ゲームオーバー時の効果音
	int decideSE = LoadSoundMemWithCheck("sound/decide.mp3"); // 決定時の効果音
	int backSE = LoadSoundMemWithCheck("sound/back.mp3"); // タイトルシーン遷移時の効果音
	int runSE = LoadSoundMemWithCheck("sound/run.mp3"); // ゲーム開始時の走る効果音
	int windSE = LoadSoundMemWithCheck("sound/wind.mp3"); // 風の効果音

	// プレイヤーの座標用の変数
	float playerX = 200, playerY = 400;
	float playerVY = 0.0f;

	// 敵用の変数
	int enemyX = 980;
	int enemyY = 400;

	while (1)
	{
		ClearDrawScreen(); // 画面をクリアする

		// 背景
		int spd = 1; // スクロール速度の初期値
		static int bgX;
		if (scene == OVER)
		{
			bgX = (bgX) % WIDTH;
		}
		else
		{
			bgX = (bgX - spd * 1) % WIDTH;
		}
		DrawGraph(bgX + WIDTH, 0, imgBG, false); // 背景の表示
		DrawGraph(bgX, 0, imgBG, false); //

		// 木々
		static int greenX;

		if (scene == OVER)
		{
			greenX = (greenX) % WIDTH;
		}
		else
		{
			greenX = (greenX - spd * 2) % WIDTH;
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
		DrawGraph(greenX + WIDTH, 0, imgGreen, true); // 木々の表示
		DrawGraph(greenX, 0, imgGreen, true); //
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		// 地面
		static int groundX;

		if (scene == OVER)
		{
			groundX = (groundX) % WIDTH;
		}
		else
		{
			groundX = (groundX - spd * 7) % WIDTH;
		}
		DrawGraph(groundX + WIDTH, 0, imgGround, true); // 地面の表示
		DrawGraph(groundX, 0, imgGround, true); //

		// プレイヤーの画像関連
		if (jump == true)
		{
			DrawGraph(playerX, playerY, imgPlayerJump, true); // ジャンプの時の画像に切り替え
			if (scene == OVER)
			{
				DrawGraph(playerX, playerY, imgPlayerJumpOver, true);
			}
		}
		else if (scene == OVER)
		{
			DrawGraph(playerX, playerY, imgPlayerOver, true); // ゲームオーバー時の画像に切り替え
		}
		else
		{
			DrawGraph(playerX, playerY, imgPlayer[(timer / 3) % 4], true); // プレイヤの表示
		}

		// 【仮】時間のカウント、マウス座標など諸々の情報表示。不要になったらコメントアウトする
		int mouseX, mouseY;
		GetMousePoint(&mouseX, &mouseY);
		SetFontSize(18);
		DrawFormatString(0, 0, BLACK, "(%d, %d)", mouseX, mouseY);

		timer++;
		SetFontSize(18);
		DrawFormatString(0, 20, BLACK, "%d", timer);

		SetFontSize(18);
		DrawFormatString(0, 40, BLACK, "Enemy_Speed:%d", setSpeed);

		SetFontSize(18);
		DrawFormatString(0, 60, BLACK, "Scene:%d", scene);

		// 当たり判定
		int x1 = playerX + 24, y1 = playerY + 40, r1 = 18, col1 = GetColor(255, 0, 0);
		int x2 = enemyX + 22, y2 = enemyY + 40, r2 = 28, col2 = GetColor(0, 0, 255);
		int d = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

		// シーン遷移(仮)
		switch (scene)
		{

		case(TITLE): // タイトル画面の処理

			DrawTextC(WIDTH * 0.5, HEIGHT * 0.3, "Run Game", 0xff6600, 80);

			if (timer % 50 < 25)
			{
				DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 200, "Rキーを押してスタート", 0xffffff, 60);
			}

			if (CheckHitKey(KEY_INPUT_R) == 1)
			{
				// ここで値を初期化
				scene = PLAY;
				enemyX = 980;
				enemyY = 400;
				setSpeed = 8;
				score = 0;
				PlaySoundMem(decideSE, DX_PLAYTYPE_BACK);
				PlaySoundMem(runSE, DX_PLAYTYPE_BACK);

				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP); // BGM再生開始(ループ)
				ChangeVolumeSoundMem(255, bgm);
			}
			break;

		case(PLAY): // ゲームプレイ画面の処理

			// スコアの表示
			SetFontSize(30);
			DrawFormatString(700, 0, BLACK, "スコア：%d", score);

			// ハイスコアの表示
			if (score > HighScore)
			{
				HighScore = score; // ハイスコアを超えた時の処理
				DrawTextB(747, 60, "ハイスコア：%d", HighScore, 0xffff00, 30);
			}
			else
			{
				DrawTextB(747, 60, "ハイスコア：%d", HighScore, 0xffffff, 30);
			}

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

			// 敵を動かす処理
			enemyX = enemyX - spd * setSpeed;
			if (enemyX < 0 - 100)
			{
				setSpeed = GetRand(27) + 8;
				enemyX = 980;

				if (setSpeed >= 28)
				{
					PlaySoundMem(windSE, DX_PLAYTYPE_BACK);
				}
			}
			DrawGraph(enemyX, enemyY, imgEnemy1[(timer / 3) % 3], true);

			// プレイヤーと敵が通過したことによるスコアの加算
			if (playerX >= enemyX)
			{
				if (addScore == 0)
				{
					score += 100;
					addScore = 1;
				}
				else
				{
					addScore = 0;
				}
			}

			if (score <= 10000)
			{
				scrollSpeed = 24;
			}
			else if (score <= 20000)
			{
				scrollSpeed = 23;
			}
			else if (score <= 30000)
			{
				scrollSpeed = 22;
			}
			else if (score <= 40000)
			{
				scrollSpeed = 21;
			}
			else if (score <= 50000)
			{
				scrollSpeed = 20;
			}
			else if (score <= 60000)
			{
				scrollSpeed = 18;
			}
			else if (score <= 70000)
			{
				scrollSpeed = 16;
			}
			else if (score <= 80000)
			{
				scrollSpeed = 14;
			}
			else if (score <= 90000)
			{
				scrollSpeed = 12;
			}
			else if (score <= 100000)
			{
				scrollSpeed = 10;
			}
			else if (score >= 100000)
			{
				scrollSpeed = 0;
			}

			// 当たり判定の描画
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 0);

			DrawCircle(x1, y1, r1, col1, true);
			DrawCircle(x2, y2, r2, col2, true);

			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			if (d <= r1 + r2)
			{
				scene = OVER;
				timer = 0;
				PlaySoundMem(overSE, DX_PLAYTYPE_BACK);

				ChangeVolumeSoundMem(0, bgm);
			}
			break;

		case(OVER): // ゲームオーバー画面の処理

			// ゲームオーバーテキスト表示
			DrawTextC(WIDTH * 0.5, HEIGHT * 0.3 - 80, "ゲームオーバー", 0xff0000, 80);

			// スコア別ランク付け
			if (score <= 3000)
			{
				DrawTextA(WIDTH * 0.5 + 290, HEIGHT / 2 - 120, "スコア：%d　出直してこい", score, 0x000000, 30);
			}
			else if (score <= 10000)
			{
				DrawTextA(WIDTH * 0.5 + 220, HEIGHT / 2 - 120, "スコア：%d　もずく級", score, 0x000000, 30);
			}
			else if (score <= 30000)
			{
				DrawTextA(WIDTH * 0.5 + 250, HEIGHT / 2 - 120, "スコア：%d　三色団子級", score, 0x000000, 30);
			}
			else if (score <= 50000)
			{
				DrawTextA(WIDTH * 0.5 + 200, HEIGHT / 2 - 120, "スコア：%d　徒桜級", score, 0x000000, 30);
			}
			else if (score <= 100000)
			{
				DrawTextA(WIDTH * 0.5 + 200, HEIGHT / 2 - 120, "スコア：%d　落雁級", score, 0x000000, 30);
			}
			else if (score >= 100000)
			{
				DrawTextA(WIDTH * 0.5 + 250, HEIGHT / 2 - 120, "スコア：%d　一望無限級", score, 0x000000, 30);
			}

			DrawTextC(WIDTH * 0.5, HEIGHT / 2, "Tキーでタイトルに戻る", 0xffffff, 30);
			DrawTextC(WIDTH * 0.5, HEIGHT / 2 + 45, "Rキーでもう一度やり直す", 0xffffff, 30);

			spd = 0;
			
			// 敵を止まったまま描画(アニメーションは有り)
			DrawGraph(enemyX, enemyY, imgEnemy1[(timer / 3) % 3], true);

			// Tキーが押されたらタイトルシーンへ遷移する
			if (CheckHitKey(KEY_INPUT_T) == 1)
			{
				scene = TITLE;
				PlaySoundMem(backSE, DX_PLAYTYPE_BACK);
				StopSoundMem(bgm); //BGM再生停止
			}
			// Rキーが押されたら再びゲームシーンへ遷移する
			else if (CheckHitKey(KEY_INPUT_R) == 1)
			{
				// ここで値を初期化
				scene = PLAY;
				enemyX = 980;
				enemyY = 400;
				setSpeed = 8;
				score = 0;
				PlaySoundMem(decideSE, DX_PLAYTYPE_BACK);
				PlaySoundMem(runSE, DX_PLAYTYPE_BACK);
				ChangeVolumeSoundMem(255, bgm);
			}
			break;
		}

		ScreenFlip(); // 裏画面の内容を表画面に反映させる
		WaitTimer(scrollSpeed); // 一定時間待つ 初期値は24
		if (ProcessMessage() == -1) break; // Windowsから情報を受け取りエラーが起きたら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) break; // ESCキーが押されたら終了
	}

	DxLib_End(); // DXライブラリ使用の終了処理
	return 0; // ソフトの終了
}

// ここから下は自作した関数を記述するスペースです

// 画像の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadGraphWithCheck(const char* file)
{
	int res = LoadGraph(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "log", MB_OK | MB_ICONSTOP); }
	return res;
}

// 音声の読み込み。読み込み失敗時はエラーウィンドウを表示
int LoadSoundMemWithCheck(const char* file)
{
	int res = LoadSoundMem(file);
	if (res == -1) { MessageBox(GetMainWindowHandle(), file, "log", MB_OK | MB_ICONSTOP); }
	return res;
}

// 文字列と値をセンタリングして表示する関数
void DrawTextA(int x, int y, const char* txt, int val, int col, int siz)
{
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	SetFontSize(siz);
	DrawFormatString(x, y, col, txt, val);
}

// 影をつけた文字列と値をセンタリングして表示する関数
void DrawTextB(int x, int y, const char* txt, int val, int col, int siz)
{
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	SetFontSize(siz);
	DrawFormatString(x + 1, y + 1, 0x000000, txt, val);
	DrawFormatString(x, y, col, txt, val);
}

// 影をつけた文字列をセンタリングして表示する関数
void DrawTextC(int x, int y, const char* txt, int col, int siz)
{
	SetFontSize(siz);
	int strWidth = GetDrawStringWidth(txt, strlen(txt));
	x -= strWidth / 2;
	y -= siz / 2;
	DrawString(x + 1, y + 1, txt, 0x000000);
	DrawString(x, y, txt, col);
}
