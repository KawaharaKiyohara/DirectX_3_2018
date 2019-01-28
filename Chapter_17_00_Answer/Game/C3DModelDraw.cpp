#include "stdafx.h"
#include "C3DModelDraw.h"
#include "graphics/Camera.h"
#include "Game.h"

C3DModelDraw::~C3DModelDraw()
{
	//定数バッファを解放。
	if (m_cb != nullptr) {
		m_cb->Release();
	}
	//ライト用の定数バッファの解放。
	if (m_lightCb != nullptr) {
		m_lightCb->Release();
	}
	//サンプラステートを解放。
	if (m_samplerState != nullptr) {
		m_samplerState->Release();
	}
}
void C3DModelDraw::Update(CVector3 trans)
{
	m_worldMatrix.MakeTranslation(trans);
}
void C3DModelDraw::Init(const wchar_t* filePath)
{
	//定数バッファを初期化。
	InitConstantBuffer();
	//サンプラステートを初期化。
	InitSamplerState();

	//ディレクションライトの初期化。
	InitDirectionLight();

	//エフェクトファクトリを作成
	C3DModelEffectFactory effectFactory(g_graphicsEngine->GetD3DDevice());
	//テクスチャがあるフォルダを設定する。
	effectFactory.SetDirectory(L"Assets/modelData");
	//cmoファイルからDirectX::Modelを作成する。
	m_modelDx = DirectX::Model::CreateFromCMO(	//CMOファイルからモデルを作成する関数の、CreateFromCMOを実行する。
		g_graphicsEngine->GetD3DDevice(),			//第一引数はD3Dデバイス。
		filePath,									//第二引数は読み込むCMOファイルのファイルパス。
		effectFactory,								//第三引数はエフェクトファクトリ。
		false,										//第四引数はCullモード。今は気にしなくてよい。
		false
	);
}
void C3DModelDraw::Draw(EnRenderMode renderMode, CMatrix viewMatrix, CMatrix projMatrix)
{
	auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
	DirectX::CommonStates state(g_graphicsEngine->GetD3DDevice());

	auto shadowMap = g_game->GetShadowMap();
	//定数バッファを更新。
	SModelFxConstantBuffer modelFxCb;
	modelFxCb.mWorld = m_worldMatrix;
	modelFxCb.mProj = projMatrix;
	modelFxCb.mView = viewMatrix;
	modelFxCb.mLightProj = shadowMap->GetLightProjMatrix();
	modelFxCb.mLightView = shadowMap->GetLighViewMatrix();
	if (m_isShadowReciever == true) {
		modelFxCb.isShadowReciever = 1;
	}
	else {
		modelFxCb.isShadowReciever = 0;
	}
	//todo 法線マップを使用するかどうかのフラグを送る。
	if (m_normalMapSRV != nullptr) {
		modelFxCb.isHasNormalMap = true;
	}
	else {
		modelFxCb.isHasNormalMap = false;
	}
	deviceContext->UpdateSubresource(m_cb, 0, nullptr, &modelFxCb, 0, 0);
	//ライト用の定数バッファを更新。
	deviceContext->UpdateSubresource(m_lightCb, 0, nullptr, &m_dirLight, 0, 0);

	//定数バッファをシェーダースロットに設定。
	deviceContext->VSSetConstantBuffers(0, 1, &m_cb);
	deviceContext->PSSetConstantBuffers(0, 1, &m_cb);
	deviceContext->PSSetConstantBuffers(1, 1, &m_lightCb);
	//サンプラステートを設定する。
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	
	//エフェクトにクエリを行う。
	m_modelDx->UpdateEffects([&](DirectX::IEffect* material) {
		auto modelMaterial = reinterpret_cast<C3DModelEffect*>(material);
		modelMaterial->SetRenderMode(renderMode);
	});
	if (m_normalMapSRV != nullptr) {
		//法線マップが設定されていたらをレジスタt2に設定する。
		deviceContext->PSSetShaderResources(2, 1, &m_normalMapSRV);
	}
	m_modelDx->Draw(
		deviceContext,
		state,
		m_worldMatrix,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
}


void C3DModelDraw::InitDirectionLight()
{
	memset(&m_dirLight, 0, sizeof(m_dirLight));
}


void C3DModelDraw::InitSamplerState()
{
	//サンプラステートはテクスチャをサンプリングする方法を指定する。
	CD3D11_DEFAULT defDesc;
	CD3D11_SAMPLER_DESC desc(defDesc);
	//
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//U方向はラップ
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//V方向はラップ
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//W方向はラップ(W方向は3次元テクスチャの時に使用される。)
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//テクスチャフィルタはバイリニアフィルタ
	//サンプラステートを作成。
	g_graphicsEngine->GetD3DDevice()->CreateSamplerState(&desc, &m_samplerState);
}

void C3DModelDraw::InitConstantBuffer()
{
	//作成するバッファのサイズをsizeof演算子で求める。
	int bufferSize = sizeof(SModelFxConstantBuffer);
	//どんなバッファを作成するのかをせてbufferDescに設定する。
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));				//０でクリア。
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;						//バッファで想定されている、読み込みおよび書き込み方法。
	bufferDesc.ByteWidth = (((bufferSize - 1) / 16) + 1) * 16;	//バッファは16バイトアライメントになっている必要がある。
																//アライメントって→バッファのサイズが16の倍数ということです。
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//バッファをどのようなパイプラインにバインドするかを指定する。
																//定数バッファにバインドするので、D3D11_BIND_CONSTANT_BUFFERを指定する。
	bufferDesc.CPUAccessFlags = 0;								//CPU アクセスのフラグです。
																//CPUアクセスが不要な場合は0。
	//作成。
	g_graphicsEngine->GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_cb);

	//続いて、ライト用の定数バッファを作成。
	//作成するバッファのサイズを変更するだけ。
	bufferDesc.ByteWidth = sizeof(SDirectionLight);				//SDirectionLightは16byteの倍数になっているので、切り上げはやらない。
	g_graphicsEngine->GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_lightCb);
}