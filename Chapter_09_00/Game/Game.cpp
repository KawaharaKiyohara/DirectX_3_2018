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
	CreateRenderTarget();
}

Game::~Game()
{
	//レンダリングターゲットの解放。
	ReleaseRenderTarget();
}
void Game::CreateRenderTarget()
{
	//D3Dデバイスを取得。
	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	//1.レンダリングターゲットとなるテクスチャを作成。
	D3D11_TEXTURE2D_DESC texDesc = {0};
	{
		//////////////////////////////////////////////////////////////
		//ここからテクスチャ作成。
		//////////////////////////////////////////////////////////////
		//テクスチャの横の解像度。
		texDesc.Width = 1280;
		//テクスチャの縦の解像度。
		texDesc.Height = 720;
		//ミップマップレベル。1でいい。
		texDesc.MipLevels = 1;
		//テクスチャ配列のサイズ。1でいい。
		texDesc.ArraySize = 1;
		//テクスチャのフォーマット。
		//RGBAの各要素8bitの32bitフォーマットにする。
		//このパラメータは今後変更するので、注目。
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//テクスチャの使用用途を決める。
		//レンダリングターゲットのテクスチャは、
		//レンダリングターゲット、ShaderResourceView、UnorderedAccessViewとしてバインドする。
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//マルチサンプリングの数。1でいい。
		texDesc.SampleDesc.Count = 1;
		//マルチサンプリングの品質。0でいい。
		texDesc.SampleDesc.Quality = 0;
		//テクスチャの読み書きのされ方。D3D11_USAGE_DEFAULTでいい。
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		//CPUからのアクセスの仕方。アクセスしないので0でよい。
		texDesc.CPUAccessFlags = 0;
		//0でよい。
		texDesc.MiscFlags = 0;
		//テクスチャを作成。
		d3dDevice->CreateTexture2D(&texDesc, nullptr, &m_renderTargetTex);
	}
	//2.レンダリングターゲットビューの作成
	{
		//////////////////////////////////////////////////////////////
		//ここからレンダリングターゲットビューの作成。
		//////////////////////////////////////////////////////////////
		//レンダリングターゲットビューを作成するためのデータを設定する。。
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
		//フォーマットはテクスチャと同じにする。
		viewDesc.Format = texDesc.Format;
		//2Dテクスチャを扱うことを指定する。
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//0でいい。
		viewDesc.Texture2D.MipSlice = 0;
		//レンダリングターゲットビューの作成。
		d3dDevice->CreateRenderTargetView(m_renderTargetTex, &viewDesc, &m_renderTargetView);
	}
	//3.シェーダーリソースビューの作成
	{
		
		//////////////////////////////////////////////////////////////
		//ここからシェーダーリソースビューの作成。
		//////////////////////////////////////////////////////////////
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//フォーマットはテクスチャと同じでよい。
		srvDesc.Format = texDesc.Format;
		//2次元テクスチャとして扱う。
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		//ミップマップレベルもテクスチャと同じでよい。
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		//0でいい。
		srvDesc.Texture2D.MostDetailedMip = 0;
		//SRVを作成する。
		d3dDevice->CreateShaderResourceView(m_renderTargetTex, &srvDesc, &m_renderTargetSRV);
	}
	//4.デプスステンシルテクスチャの作成
	D3D11_TEXTURE2D_DESC depthTexDesc = texDesc;
	{
		//////////////////////////////////////////////////////////////
		//ここからデプスステンシルテクスチャの作成。
		//////////////////////////////////////////////////////////////
		//デプスステンシルビューにバインドする。
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//32bit浮動小数点のデプスステンシルバッファを作成する。
		depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
		//デプスステンシルテクスチャを作成する。
		d3dDevice->CreateTexture2D(&depthTexDesc, nullptr, &m_depthStencilTex);
	}
	//5.デプスステンシルビューの作成
	{
		//////////////////////////////////////////////////////////////
		//ここからデプスステンシルビューの作成。
		//////////////////////////////////////////////////////////////
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		//フォーマットは深度ステンシルと同じにする。
		depthStencilViewDesc.Format = depthTexDesc.Format;
		//2Dテクスチャを扱うことを指定する。
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//0でいい。
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		//0でいい。
		depthStencilViewDesc.Flags = 0;
		//デプスステンシルビューを作成。
		d3dDevice->CreateDepthStencilView(m_depthStencilTex, &depthStencilViewDesc, &m_depthStencilView);
	}
}
void Game::ReleaseRenderTarget()
{
	if (m_depthStencilView != nullptr) {
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	if (m_depthStencilTex != nullptr) {
		m_depthStencilTex->Release();
		m_depthStencilTex = nullptr;
	}
	
	if (m_renderTargetView != nullptr) {
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
	if (m_renderTargetTex != nullptr) {
		m_renderTargetTex->Release();
		m_renderTargetTex = nullptr;
	}
	if (m_renderTargetSRV != nullptr) {
		m_renderTargetSRV->Release();
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
		m_renderTargetView
	};
	d3dDeviceContext->OMSetRenderTargets(1, rts, m_depthStencilView);
	//レンダリングターゲットをクリア。
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
	d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

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
		material->SetAlbedoTexture(m_renderTargetSRV);
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
