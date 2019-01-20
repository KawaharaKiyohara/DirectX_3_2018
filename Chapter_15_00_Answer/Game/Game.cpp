#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;

	//サウンドエンジンを初期化。
	m_soundEngine.Init();

	//BGM
	m_bgm[0].Init(L"Assets/sound/bgm_00.wav");
	m_bgm[0].Play(true);
	m_bgm[1].Init(L"Assets/sound/bgm_01.wav");
	

	//ワンショット再生のSE
	m_se.Init(L"Assets/sound/coinGet.wav");

	m_se2.Init(L"Assets/sound/uni1482.wav");

	//カメラを初期化。
	InitCamera();
		
	//メインとなるレンダリングターゲットを作成する。
	m_mainRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
	//カメラ空間での深度値の書き込み先となるレンダリングターゲットを作成する。
	//フォーマットはR成分のみの32ビット浮動小数点バッファ。
	m_depthRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R32_FLOAT
	);
	//メインレンダリングターゲットに描かれた絵を
	//フレームバッファにコピーするためのスプライトを初期化する。
	m_copyMainRtToFrameBufferSprite.Init(
		m_mainRenderTarget.GetRenderTargetSRV(),
		FRAME_BUFFER_W,
		FRAME_BUFFER_H
	);
	//ポストエフェクトの初期化。
	m_postEffect.Init();
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

void Game::InitCamera()
{
	g_camera3D.SetPosition({ 0.0f, 400.0f, 1000.0f });
	g_camera3D.SetTarget({ 0.0f, 0.0f, 0.0f });
	g_camera3D.Update();
	g_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	g_camera2D.SetWidth(FRAME_BUFFER_W);
	g_camera2D.SetHeight(FRAME_BUFFER_H);
	g_camera2D.SetPosition({0.0f, 0.0f, -10.0f});
	g_camera2D.SetTarget(CVector3::Zero());
	g_camera2D.Update();
}

void Game::Update()
{
	//ゲームパッドの更新。
	for (auto& pad : g_pad) {
		pad.Update();
	}
	//サウンドエンジンを更新。
	m_soundEngine.Update();

	if (g_pad[0].IsTrigger(enButtonA)) {
		//Aボタンが押されたらSEを鳴らす。
		m_se.Play(false);
	}
	if (g_pad[0].IsTrigger(enButtonB)) {
		//Bボタンが押されたらSEを鳴らす。
		m_se2.Play(false);
	}
	if (g_pad[0].IsTrigger(enButtonY)) {
		//Yボタンが押されたらBGMを切り替える。]
		m_bgm[m_playBgmNo].Pause();
		m_playBgmNo++;
		m_playBgmNo = m_playBgmNo % 2;
		m_bgm[m_playBgmNo].Play(true);
	}
	//シャドウマップを更新。
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);
	//ポストエフェクトの更新。
	m_postEffect.Update();
}

void Game::PreRender()
{
	//シャドウマップにレンダリング
	m_shadowMap.RenderToShadowMap();
}
	
void Game::ForwordRender()
{
	//MRT(Multi rendering target)
	ID3D11RenderTargetView* rts[] = {
		m_mainRenderTarget.GetRenderTargetView(),	//0番目はメインレンダリングターゲット
		m_depthRenderTarget.GetRenderTargetView(),	//1番目はカメラ空間での深度値を出力するレンダリングターゲット。
	};
	auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
	deviceContext->OMSetRenderTargets(
		2,											//第一引数はセットするレンダリングターゲットの数。2つ設定するので2を指定する。
		rts,										//第二引数にはレンダリングターゲットの配列を指定する。
		m_mainRenderTarget.GetDepthStensilView()	//第三引数は深度ステンシルバッファ。これは一つしか指定できない。
	);
	deviceContext->RSSetViewports(1, m_mainRenderTarget.GetViewport());

	//メインレンダリングターゲットをクリアする。
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_mainRenderTarget.ClearRenderTarget(clearColor);
	m_depthRenderTarget.ClearRenderTarget(clearColor);

}
void Game::PostRender()
{
	//ポストエフェクトの描画。
	m_postEffect.Draw();

	//レンダリングターゲットをフレームバッファに戻す。
	g_graphicsEngine->ChangeRenderTarget(
		m_frameBufferRenderTargetView,
		m_frameBufferDepthStencilView,
		&m_frameBufferViewports
	);
	//ドロドロ
	m_copyMainRtToFrameBufferSprite.Draw();
	
	m_frameBufferRenderTargetView->Release();
	m_frameBufferDepthStencilView->Release();
}
void Game::Render()
{
	//描画開始。
	g_graphicsEngine->BegineRender();
	//フレームバッファののレンダリングターゲットをバックアップしておく。
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	d3dDeviceContext->OMGetRenderTargets(
		1,
		&m_frameBufferRenderTargetView,
		&m_frameBufferDepthStencilView
	);
	//ビューポートもバックアップを取っておく。
	unsigned int numViewport = 1;
	d3dDeviceContext->RSGetViewports(&numViewport, &m_frameBufferViewports);

	//プリレンダリング
	PreRender();
	
	//フォワードレンダリング。
	ForwordRender();
	
	//ポストレンダリング
	PostRender();	

	//描画終了。
	g_graphicsEngine->EndRender();
}
