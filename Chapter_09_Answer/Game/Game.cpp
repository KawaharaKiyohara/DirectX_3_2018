#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//カメラを設定。
	g_camera3D.SetPosition({ 0.0f, 1000.0f, 2200.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();
	//Unityちゃんのモデルを初期化。
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	//背景モデルの初期化処理。
	m_bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	//レンダリングターゲットの作成。
	m_renderTarget.Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
}

Game::~Game()
{
}


void Game::Update()
{
	//ゲームパッドの更新。
	for (auto& pad : g_pad) {
		pad.Update();
	}
	m_unityChanPos.x += g_pad[0].GetLStickXF() * -50.0f;
	m_unityChanPos.z += g_pad[0].GetLStickYF() * -50.0f;

	m_unityChanModelDraw.Update(m_unityChanPos);
}
void Game::Render()
{
	//レンダリングモード。
	EnRenderMode renderMode = enRenderMode_Normal;
	//描画開始。
	g_graphicsEngine->BegineRender();

	///////////////////////////////////////////////
	//まずはオフスクリーンレンダリング
	///////////////////////////////////////////////
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	//現在のレンダリングターゲットをバックアップしておく。
	ID3D11RenderTargetView* oldRenderTargetView;
	ID3D11DepthStencilView* oldDepthStencilView;
	d3dDeviceContext->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);
	//レンダリングターゲットを切り替える。
	ID3D11RenderTargetView* rts[] = {
		m_renderTarget.GetRenderTargetView()
	};
	d3dDeviceContext->OMSetRenderTargets(1, rts, m_renderTarget.GetDepthStensilView());
	//レンダリングターゲットをクリア。
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	m_renderTarget.ClearRenderTarget(clearColor);

	//背景を描画。
	m_bgModelDraw.Draw(renderMode);
	//ユニティちゃんを通常レンダリング。
	m_unityChanModelDraw.Draw(renderMode);


	///////////////////////////////////////////////
	//ここからオンスクリーンレンダリング。
	///////////////////////////////////////////////
	//レンダリングターゲットを元に戻す。
	d3dDeviceContext->OMSetRenderTargets(1, &oldRenderTargetView, oldDepthStencilView);
	//レンダリングターゲットとデプスステンシルの参照カウンタを下す。
	oldRenderTargetView->Release();
	oldDepthStencilView->Release();

	//背景モデルで使用するテクスチャをオフスクリーンレンダリングしたものに差し替えていく。
	m_bgModelDraw.QueryMaterials([&](C3DModelEffect* material) {
		//差し替え前のテクスチャをスタックに退避。
		material->PushAlbedoTexture();
		//テクスチャを差し替える。
		material->SetAlbedoTexture(m_renderTarget.GetRenderTargetSRV());
	});
	//レンダリングターゲットを戻したので、背景とユニティちゃんを
	//画面に描画する。
	m_bgModelDraw.Draw(renderMode);
	m_unityChanModelDraw.Draw(renderMode);

	//背景のテクスチャを元に戻す。
	m_bgModelDraw.QueryMaterials([&](C3DModelEffect* material) {
		//差し替え前のテクスチャをスタックから復帰。
		material->PopAlbedoTexture();	
	});

	//描画終了。
	g_graphicsEngine->EndRender();
}
