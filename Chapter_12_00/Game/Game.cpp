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
	//地面をシャドウレシーバーにする。
	m_bgModelDraw.SetShadowReciever(true);

	//フレームバッファののレンダリングターゲットをバックアップしておく。
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	d3dDeviceContext->OMGetRenderTargets(
		1,
		&m_frameBufferRenderTargetView,
		&m_frameBufferDepthStencilView
	);
	//ビューポートもバックアップを取っておく。
	unsigned int numViewport = 1;
	D3D11_VIEWPORT oldViewports;
	d3dDeviceContext->RSGetViewports(&numViewport, &m_frameBufferViewports);

}

Game::~Game()
{
	if (m_frameBufferRenderTargetView != nullptr) {
		m_frameBufferRenderTargetView->Release();
	}
	if (m_frameBufferDepthStencilView != nullptr) {
		m_frameBufferDepthStencilView->Release();
	}
}


void Game::ChangeRenderTarget(ID3D11DeviceContext* d3dDeviceContext, RenderTarget* renderTarget, D3D11_VIEWPORT* viewport)
{
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	ID3D11RenderTargetView* rtTbl[] = {
		renderTarget->GetRenderTargetView()
	};
	//レンダリングターゲットの切り替え。
	d3dDeviceContext->OMSetRenderTargets(1, rtTbl, renderTarget->GetDepthStensilView());
	if (viewport != nullptr) {
		//ビューポートが指定されていたら、ビューポートも変更する。
		d3dDeviceContext->RSSetViewports(1, viewport);
	}
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
	//シャドウキャスターを登録。
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_bgModelDraw);
	//シャドウマップを更新。
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);

	//メインとなるレンダリングターゲットを作成する。
	m_mainRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R8G8B8A8_UNORM
	);
}

void Game::PreRender()
{
	//シャドウマップにレンダリング
	m_shadowMap.RenderToShadowMap();
}
	
void Game::ForwordRender()
{
	//レンダリングターゲットをメインに変更する。
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	ChangeRenderTarget(d3dDeviceContext, &m_mainRenderTarget, &m_frameBufferViewports);
	
	m_bgModelDraw.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
	m_unityChanModelDraw.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
}
void Game::PostRender()
{
}
void Game::Render()
{
	//レンダリングターゲットをフレームバッファに戻す。
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	//元に戻す。
	d3dDeviceContext->OMSetRenderTargets(
		1,
		&m_frameBufferRenderTargetView,
		m_frameBufferDepthStencilView
	);
	d3dDeviceContext->RSSetViewports(1, &m_frameBufferViewports);

	//描画開始。
	g_graphicsEngine->BegineRender();
	//プリレンダリング
	PreRender();
	
	//フォワードレンダリング。
	ForwordRender();
	
	//ポストレンダリング
	PostRender();

	//描画終了。
	g_graphicsEngine->EndRender();
}
