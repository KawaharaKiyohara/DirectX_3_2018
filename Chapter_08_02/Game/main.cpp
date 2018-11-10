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
	g_camera3D.SetPosition({ 0.0f, 1000.0f, 2200.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();

	//ユニティちゃんの座標
	CVector3 unityChanPos;
	unityChanPos = { 0.0f, 0.0f, 0.0f };

	//キャラクタモデルを初期化。
	C3DModelDraw unitChanModelDraw;
	unitChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	
	//背景モデルを初期化。
	C3DModelDraw bgModelDraw;
	bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	
	//Hands-On 奥のモノを手前に描画する深度ステンシルステートを作ってみよう。
	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_GREATER;
	desc.StencilEnable = false;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//D3Dデバイスを取得。
	auto d3ddevice = g_graphicsEngine->GetD3DDevice();
	//デプスステンシルステートを作成。
	ID3D11DepthStencilState* depthStencilState;
	d3ddevice->CreateDepthStencilState(&desc, &depthStencilState);
	
	int renderMode = 0;	//０なら通常描画、１ならシルエット描画。


	//ゲームループ。
	while (DispatchWindowMessage() == true)
	{
		//描画開始。
		g_graphicsEngine->BegineRender();
		
		//手前に描画を行うデプスステンシルステートを設定する。
		g_graphicsEngine->GetD3DDeviceContext()->OMSetDepthStencilState(depthStencilState, 0);
		for (auto& pad : g_pad) {
			pad.Update();
		}
		//カメラを更新。
		UpdateCamera();
		//ユニティちゃんを左右に動かす。
		//ゲームパッドがない人は4と6で動かせるよ。
		if (g_pad[0].IsPress(enButtonLeft) == true) {
			unityChanPos.x += 15.0f;
		}
		if (g_pad[0].IsPress(enButtonRight) == true) {
			unityChanPos.x -= 15.0f;
		}
		if (g_pad[0].IsPress(enButtonDown) == true) {
			unityChanPos.z += 15.0f;
		}
		if (g_pad[0].IsPress(enButtonUp) == true) {
			unityChanPos.z -= 15.0f;
		}
		
		//通常描画モードにする。。
		renderMode = 0;
		//背景を描画。
		bgModelDraw.Draw(renderMode);

		//ユニティちゃんは先にシルエットを描画してから、普通に描く。
		renderMode = 1;
		//ユニティちゃんを描画。
		unitChanModelDraw.Update(unityChanPos);
		unitChanModelDraw.Draw(renderMode);

		//キャラを通常レンダリング。
		renderMode = 0;
		unitChanModelDraw.Draw(renderMode);

		//描画終了。
		g_graphicsEngine->EndRender();
	}
}