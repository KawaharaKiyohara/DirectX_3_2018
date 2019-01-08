#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	//カメラを初期化。
	InitCamera();
	//Unityちゃんのモデルを初期化。
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetDirectionLightColor(1, m_ligPower);
/*	m_unityChanModelDraw.SetDirectionLightColor(2, { 2.0f,2.0f, 2.0f, 1.0f });
	m_unityChanModelDraw.SetDirectionLightColor(3, { 2.0f,2.0f, 2.0f, 1.0f });*/

	//背景モデルの初期化処理。
	m_bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	//地面をシャドウレシーバーにする。
	m_bgModelDraw.SetShadowReciever(true);
	
	//メインとなるレンダリングターゲットを作成する。
	m_mainRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);

	//メインレンダリングターゲットに描かれた絵を
	//フレームバッファにコピーするためのスプライトを初期化する。
	m_copyMainRtToFrameBufferSprite.Init(
		m_mainRenderTarget.GetRenderTargetSRV(),
		FRAME_BUFFER_W,
		FRAME_BUFFER_H
	);
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
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 100.0f, 0.0f });
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
	m_ligPower.x += g_pad[0].IsPress(enButtonB) * 0.1f;
	m_ligPower.y += g_pad[0].IsPress(enButtonA) * 0.1f;
	m_ligPower.z += g_pad[0].IsPress(enButtonX) * 0.1f;
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetDirectionLightColor(1, m_ligPower);
	
	//シャドウキャスターを登録。
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_bgModelDraw);
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
	//レンダリングターゲットをメインに変更する。
	g_graphicsEngine->ChangeRenderTarget(&m_mainRenderTarget, &m_frameBufferViewports);
	//メインレンダリングターゲットをクリアする。
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_mainRenderTarget.ClearRenderTarget(clearColor);

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
	Stopwatch sw;
	sw.Start();
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
	sw.Stop();
	char text[256];
	sprintf(text, "frame delta time(単位：ミリ秒) = %f\n", sw.GetElapsedMillisecond());
	OutputDebugString(text);
}
