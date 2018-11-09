#include "stdafx.h"
#include "system/system.h"
#include "graphics/Shader.h"
#include "C3DModelDraw.h"

using namespace std;


/// <summary>
/// カメラの更新
/// </summary>
void UpdateCamera()
{
	//注視点から視点に向かって伸びるベクトルを計算する。
	CVector3 toPos = g_camera3D.GetPosition() - g_camera3D.GetTarget();
	//カメラを回すクォータニオンを作成する。
	CQuaternion qAddRot;
	//ゲームパッドの左スティックでカメラが回る。
	//ゲームパッドがない人はキーボードの'a'と'd'で回せるよ。
	qAddRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.01f);
	//注視点から視点に向かって伸びるベクトルを回す。
	qAddRot.Multiply(toPos);
	//新しい視点を計算する。
	g_camera3D.SetPosition(g_camera3D.GetTarget() + toPos);
	g_camera3D.Update();
}

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");
	
	//カメラを設定。
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();
	//オブジェクトの座標
	CVector3 pos1, pos2;
	pos1 = { 0.0f, 0.0f, 0.0f };
	pos2 = { 100.0f, 0.0f, -100.0f };

	//手前に描くモデルを初期化。
	C3DModelDraw modelDraw1;
	modelDraw1.Init(L"Assets/modelData/unityChan1.cmo");

	//奥に描くモデルを初期化。
	C3DModelDraw modelDraw2;
	modelDraw2.Init(L"Assets/modelData/unityChan2.cmo");
	//ゲームループ。
	while (DispatchWindowMessage() == true)
	{
		//描画開始。
		g_graphicsEngine->BegineRender();
		
		for (auto& pad : g_pad) {
			pad.Update();
		}
		//カメラを更新。
		UpdateCamera();
		//奥のモデルだけ左右に動かす。
		//ゲームパッドがない人は4と6で動かせるよ。
		if (g_pad[0].IsPress(enButtonLeft) == true) {
			pos2.x += 2.0f;
		}
		if (g_pad[0].IsPress(enButtonRight) == true) {
			pos2.x -= 2.0f;
		}
		//Chapter 8.1.2 Hands-On 描画する順番を変えることで正しく描画されることを確認する。

		//手前に描画。
		modelDraw1.Update(pos1);
		modelDraw1.Draw();

		//奥に描画。
		modelDraw2.Update(pos2);
		modelDraw2.Draw();
		
		//描画終了。
		g_graphicsEngine->EndRender();
	}
}