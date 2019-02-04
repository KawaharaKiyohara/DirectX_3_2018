#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

ID3D11ShaderResourceView* g_normalMapSRV = nullptr;
ID3D11ShaderResourceView* g_specMapSRV = nullptr;

Game::Game()
{
	//カメラを初期化。
	InitCamera();

	CVector3 ambientColor = { 0.7f, 0.7f, 0.7f };
	//Unityちゃんのモデルを初期化。
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetAmbientLight(ambientColor);
	
	m_unityChanModelDraw_NoAO.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw_NoAO.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw_NoAO.SetAmbientLight(ambientColor);

	//Unityちゃんの法線マップをロード。
	//ファイル名を使って、テクスチャをロードして、ShaderResrouceViewを作成する。
	DirectX::CreateDDSTextureFromFileEx(
		g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_nomal.dds", 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		false, nullptr, &g_normalMapSRV);

	//Unityちゃんのスペキュラマップをロード。
	//ファイル名を使って、テクスチャをロードして、ShaderResourceViewを作成する。
	DirectX::CreateDDSTextureFromFileEx(
		g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_spec.dds", 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		false, nullptr, &g_specMapSRV);

	
	//モデルに法線マップを設定する。
	m_unityChanModelDraw.SetNormalMap(g_normalMapSRV);
	m_unityChanModelDraw_NoAO.SetNormalMap(g_normalMapSRV);
	//モデルにスペキュラマップを設定する。
	m_unityChanModelDraw.SetSpecularMap(g_specMapSRV);
	m_unityChanModelDraw_NoAO.SetSpecularMap(g_specMapSRV);

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
	if (g_normalMapSRV != nullptr) {
		g_normalMapSRV->Release();
	}
	if (g_specMapSRV != nullptr) {
		g_specMapSRV->Release();
	}
}

void Game::InitCamera()
{
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
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
	CQuaternion qRot;
	qRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.1f);
	qRot.Multiply(m_ligDirection);
	
	m_unityChanModelDraw.SetDirectionLightDirection(0, m_ligDirection);
	m_unityChanModelDraw_NoAO.SetDirectionLightDirection(0, m_ligDirection);
	
	//シャドウキャスターを登録。
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw_NoAO);

	m_unityChanModelDraw.Update({ 130.0f, 0.0f, 0.0f });
	m_unityChanModelDraw_NoAO.Update({ -130.0f, 0.0f, 0.0f });

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


	m_unityChanModelDraw.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);

	m_unityChanModelDraw_NoAO.Draw(
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
