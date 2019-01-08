#include "stdafx.h"
#include "system/system.h"
#include "Game.h"

using namespace std;
Game* g_game = nullptr;

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");
	
	//ゲームクラスのインスタンスを定義する。
	Game game;
	g_game = &game;
	//ゲームループ。
	while (DispatchWindowMessage() == true)
	{
		//ゲームの更新処理を実行。
		game.Update();
		//ゲームの描画処理を実行。
		game.Render();
	}
}