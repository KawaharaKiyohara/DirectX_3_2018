#include "stdafx.h"
#include "Bloom.h"
#include "Game.h"
#include "PostEffect.h"

Bloom::Bloom()
{
	//各種レンダリングターゲットの初期化。
	InitRenderTarget();
	//シェーダーを初期化。
	InitShader();
	//αブレンドステートを初期化。
	InitAlphaBlendState();
	//定数バッファの初期化。
	InitConstantBuffer();
	//サンプラステートの初期化。
	InitSamplerState();
}


Bloom::~Bloom()
{
	if (m_disableBlendState != nullptr) {
		m_disableBlendState->Release();
	}
	if (m_blurParamCB != nullptr) {
		m_blurParamCB->Release();
	}
	if (m_samplerState != nullptr) {
		m_samplerState->Release();
	}
	if (m_finalBlendState != nullptr) {
		m_finalBlendState->Release();
	}
}
void Bloom::InitConstantBuffer()
{
	D3D11_BUFFER_DESC desc;

	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = (((sizeof(SBlurParam) - 1) / 16) + 1) * 16;	//16バイトアライメントに切りあげる。
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	g_graphicsEngine->GetD3DDevice()->CreateBuffer(&desc, NULL, &m_blurParamCB);
}
void Bloom::InitSamplerState()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	g_graphicsEngine->GetD3DDevice()->CreateSamplerState(&desc, &m_samplerState);
}
void Bloom::InitShader()
{
	m_vs.Load("Assets/shader/bloom.fx", "VSMain", Shader::EnType::VS);
	m_psLuminance.Load("Assets/shader/bloom.fx", "PSSamplingLuminance", Shader::EnType::PS);
	m_vsXBlur.Load("Assets/shader/bloom.fx", "VSXBlur", Shader::EnType::VS);
	m_vsYBlur.Load("Assets/shader/bloom.fx", "VSYBlur", Shader::EnType::VS);
	m_psBlur.Load("Assets/shader/bloom.fx", "PSBlur", Shader::EnType::PS);
	m_psFinal.Load("Assets/shader/bloom.fx", "PSFinal", Shader::EnType::PS);
}
void Bloom::InitRenderTarget()
{
	//輝度抽出用のレンダリングターゲットを作成する。
	m_luminanceRT.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);

	//ブラーをかけるためのダウンサンプリング用のレンダリングターゲットを作成。
	//横ブラー用。
	m_downSamplingRT[0].Create(
		FRAME_BUFFER_W * 0.5f,	//横の解像度をフレームバッファの半分にする。
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
	//縦ブラー用。
	m_downSamplingRT[1].Create(
		FRAME_BUFFER_W * 0.5f,	//横の解像度をフレームバッファの半分にする。
		FRAME_BUFFER_H * 0.5f,	//縦の解像度をフレームバッファの半分にする。
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
}
void Bloom::InitAlphaBlendState()
{
	CD3D11_DEFAULT defaultSettings;
	//デフォルトセッティングで初期化する。
	CD3D11_BLEND_DESC blendDesc(defaultSettings);
	auto device = g_graphicsEngine->GetD3DDevice();

	device->CreateBlendState(&blendDesc, &m_disableBlendState);

	//最終合成用のブレンドステートを作成する。
	//最終合成は加算合成。
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	device->CreateBlendState(&blendDesc, &m_finalBlendState);
}
void Bloom::Update()
{
	//ガウスフィルタの重みを更新する。
	float total = 0;
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		m_blurParam.weights[i] = expf(-0.5f*(float)(i*i) / m_blurDispersion);
		total += 2.0f*m_blurParam.weights[i];

	}
	// 規格化。重みのトータルが1.0になるように、全体の重みで除算している。
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		m_blurParam.weights[i] /= total;
	}
}

void Bloom::Draw(PostEffect& postEffect)
{
	auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
	deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	//まずは輝度を抽出する。
	{
		//αブレンドを無効にする。
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->OMSetBlendState(m_disableBlendState, blendFactor, 0xffffffff);

		//輝度抽出用のレンダリングターゲットに変更する。
		g_graphicsEngine->ChangeRenderTarget(&m_luminanceRT, m_luminanceRT.GetViewport());
		//レンダリングターゲットのクリア。
		float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_luminanceRT.ClearRenderTarget(clearColor);
		
		//シーンをテクスチャとする。
		auto mainRTTexSRV = g_game->GetMainRenderTarget()->GetRenderTargetSRV();
		deviceContext->PSSetShaderResources(0, 1, &mainRTTexSRV);

		//フルスクリーン描画。
		postEffect.DrawFullScreenQuadPrimitive(deviceContext, m_vs, m_psLuminance);
	}
	
	//輝度を抽出したテクスチャにXブラーをかける。
	{
		//Xブラー用のレンダリングターゲットに変更する。
		g_graphicsEngine->ChangeRenderTarget(&m_downSamplingRT[0], m_downSamplingRT[0].GetViewport());
		
		//輝度テクスチャをt0レジスタに設定する。
		auto luminanceTexSRV = m_luminanceRT.GetRenderTargetSRV();
		deviceContext->VSSetShaderResources(0, 1, &luminanceTexSRV);
		deviceContext->PSSetShaderResources(0, 1, &luminanceTexSRV);
		// 定数バッファを更新。
		m_blurParam.offset.x = 16.0f / m_luminanceRT.GetWidth();
		m_blurParam.offset.y = 0.0f;
		deviceContext->UpdateSubresource(m_blurParamCB, 0, nullptr, &m_blurParam, 0, 0);
		//ブラー用の定数バッファを設定する。
		deviceContext->PSSetConstantBuffers(0, 1, &m_blurParamCB);

		//フルスクリーン描画。
		postEffect.DrawFullScreenQuadPrimitive(deviceContext, m_vsXBlur, m_psBlur);
	}
	//XブラーをかけたテクスチャにYブラーをかける。
	{
		//Yブラー用のレンダリングターゲットに変更する。
		g_graphicsEngine->ChangeRenderTarget(&m_downSamplingRT[1], m_downSamplingRT[1].GetViewport());
		
		//Xブラーをかけたテクスチャをt0レジスタに設定する。
		auto xBlurSRV = m_downSamplingRT[0].GetRenderTargetSRV();
		deviceContext->VSSetShaderResources(0, 1, &xBlurSRV);
		deviceContext->PSSetShaderResources(0, 1, &xBlurSRV);

		// 定数バッファを更新。
		m_blurParam.offset.x = 0.0f;
		m_blurParam.offset.y = 16.0f / m_luminanceRT.GetHeight();
		deviceContext->UpdateSubresource(m_blurParamCB, 0, nullptr, &m_blurParam, 0, 0);
		//ブラー用の定数バッファを設定する。
		deviceContext->PSSetConstantBuffers(0, 1, &m_blurParamCB);

		//フルスクリーン描画。
		postEffect.DrawFullScreenQuadPrimitive(deviceContext, m_vsYBlur, m_psBlur);
	}
	//最後にぼかした絵を加算合成でメインレンダリングターゲットに合成して終わり。
	{
		auto mainRT = g_game->GetMainRenderTarget();
		g_graphicsEngine->ChangeRenderTarget(mainRT, mainRT->GetViewport());

		//XYブラーをかけたテクスチャをt0レジスタに設定する。
		auto srv = m_downSamplingRT[1].GetRenderTargetSRV();
		deviceContext->PSSetShaderResources(0, 1, &srv);

		//加算合成用のブレンディングステートを設定する。
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->OMSetBlendState(m_finalBlendState, blendFactor, 0xffffffff);
		//フルスクリーン描画。
		postEffect.DrawFullScreenQuadPrimitive(deviceContext, m_vs, m_psFinal);

		//ブレンディングステートを戻す。
		deviceContext->OMSetBlendState(m_disableBlendState, blendFactor, 0xffffffff);

	}

}