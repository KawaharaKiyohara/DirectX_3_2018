#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;

	//サウンドエンジンを初期化。
	m_soundEngine.Init();

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

	//Effekseerを初期化。
	InitEffekseer();

	//サンプルのエフェクトをロードする。
	m_sampleEffect = Effekseer::Effect::Create(m_effekseerManager, (const EFK_CHAR*)L"Assets/effect/test.efk");
	//エフェクトを再生する。
	m_playEffectHandle = m_effekseerManager->Play(m_sampleEffect, 0.0f, 0.0f, 0.0f);
}

Game::~Game()
{
	if (m_frameBufferRenderTargetView != nullptr) {
		m_frameBufferRenderTargetView->Release();
	}
	if (m_frameBufferDepthStencilView != nullptr) {
		m_frameBufferDepthStencilView->Release();
	}
	if (m_effekseerManager != nullptr) {
		m_effekseerManager->Destroy();

	}
	if (m_effekseerRenderer != nullptr) {
		m_effekseerRenderer->Destroy();
	}
	//エフェクトを破棄。
	if (m_sampleEffect != nullptr) {
		m_sampleEffect->Release();
	}
}

void Game::InitEffekseer()
{
	//レンダラーを初期化。
	m_effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
		g_graphicsEngine->GetD3DDevice(),			//D3Dデバイス。
		g_graphicsEngine->GetD3DDeviceContext(),	//D3Dデバイスコンテキスト。
		20000										//板ポリの最大数。
	);
	//エフェクトマネージャを初期化。
	m_effekseerManager = Effekseer::Manager::Create(10000);

	// 描画用インスタンスから描画機能を設定
	m_effekseerManager->SetSpriteRenderer(m_effekseerRenderer->CreateSpriteRenderer());
	m_effekseerManager->SetRibbonRenderer(m_effekseerRenderer->CreateRibbonRenderer());
	m_effekseerManager->SetRingRenderer(m_effekseerRenderer->CreateRingRenderer());
	m_effekseerManager->SetTrackRenderer(m_effekseerRenderer->CreateTrackRenderer());
	m_effekseerManager->SetModelRenderer(m_effekseerRenderer->CreateModelRenderer());

	// 描画用インスタンスからテクスチャの読込機能を設定
	// 独自拡張可能、現在はファイルから読み込んでいる。
	m_effekseerManager->SetTextureLoader(m_effekseerRenderer->CreateTextureLoader());
	m_effekseerManager->SetModelLoader(m_effekseerRenderer->CreateModelLoader());
	
}
void Game::InitCamera()
{
	g_camera3D.SetPosition({ 50.0f, 0.0f, 0.0f });
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

	//シャドウマップを更新。
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);
	//ポストエフェクトの更新。
	m_postEffect.Update();

	//Effekseerカメラ行列を設定。
	//まずはEffeseerの行列型の変数に、カメラ行列とプロジェクション行列をコピー。
	Effekseer::Matrix44 efCameraMat;
	g_camera3D.GetViewMatrix().CopyTo(efCameraMat);
	Effekseer::Matrix44 efProjMat;
	g_camera3D.GetProjectionMatrix().CopyTo(efProjMat);
	//カメラ行列とプロジェクション行列を設定。
	m_effekseerRenderer->SetCameraMatrix(efCameraMat);
	m_effekseerRenderer->SetProjectionMatrix(efProjMat);
	//Effekseerを更新。
	m_effekseerManager->Update();

	if (g_pad[0].IsTrigger(enButtonA)) {
		//再生中のエフェクトを止める。
		m_effekseerManager->StopEffect(m_playEffectHandle);
		//再生。
		m_playEffectHandle = m_effekseerManager->Play(m_sampleEffect, 0.0f, 0.0f, 0.0f);
	}
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


	//エフェクトは不透明オブジェクトを描画した後で描画する。
	m_effekseerRenderer->BeginRendering();
	m_effekseerManager->Draw();
	m_effekseerRenderer->EndRendering();
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
