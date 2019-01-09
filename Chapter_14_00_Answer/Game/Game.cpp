#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;
	//カメラを初期化。
	InitCamera();
	//Unityちゃんのモデルを初期化。
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);

	m_unityChanModelDraw2.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw2.SetDirectionLightColor(0, m_ligPower);

	m_unityChanModelDraw3.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw3.SetDirectionLightColor(0, m_ligPower);

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
	//カメラを回すぜ
	CQuaternion qRot;
	qRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.01f);
	auto toPos = g_camera3D.GetPosition() - g_camera3D.GetTarget() ;
	qRot.Multiply(toPos);
	g_camera3D.SetPosition(g_camera3D.GetTarget() + toPos);
	g_camera3D.Update();

	m_ligPower.x += g_pad[0].IsPress(enButtonB) * 0.1f;
	m_ligPower.y += g_pad[0].IsPress(enButtonA) * 0.1f;
	m_ligPower.z += g_pad[0].IsPress(enButtonX) * 0.1f;
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetDirectionLightColor(1, m_ligPower);
	m_unityChanModelDraw2.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw2.SetDirectionLightColor(1, m_ligPower);
	m_unityChanModelDraw3.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw3.SetDirectionLightColor(1, m_ligPower);

	m_unityChanModelDraw2.Update({ 400.0f, 0.0f, -1000.0f });
	m_unityChanModelDraw3.Update({ -400.0f, 0.0f, 500.0f });

	//シャドウキャスターを登録。
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw2);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw3);
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
	//レンダリングターゲットを複数設定する。
	//     MRT(Multi rendering target)
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
	m_unityChanModelDraw2.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
	m_unityChanModelDraw3.Draw(
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
