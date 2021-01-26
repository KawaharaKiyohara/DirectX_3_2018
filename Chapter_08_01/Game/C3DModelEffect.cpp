#include "stdafx.h"
#include "C3DModelEffect.h"

C3DModelEffect::C3DModelEffect()
{
	//頂点シェーダーをロード。
	m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
	m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	m_psSilhouette.Load("Assets/shader/model.fx", "PSMain_Silhouette", Shader::EnType::PS);

	//デプスステンシルの初期化。
	InitSilhouettoDepthStepsilState();

}

C3DModelEffect::~C3DModelEffect()
{
	if (m_albedoTexture != nullptr) {
		m_albedoTexture->Release();
	}
	if (m_silhouettoDepthStepsilState != nullptr) {
		m_silhouettoDepthStepsilState->Release();
	}
}

void C3DModelEffect::InitSilhouettoDepthStepsilState()
{
    //D3Dデバイスを取得。
	auto pd3d = g_graphicsEngine->GetD3DDevice();
	//作成する深度ステンシルステートの定義を設定していく。
	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = true;						   //Zテストが有効。
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //ZバッファにZ値を描き込まない。
	desc.DepthFunc = D3D11_COMPARISON_GREATER;		   //Z値が大きければフレームバッファに描き込む。

	pd3d->CreateDepthStencilState(&desc, &m_silhouettoDepthStepsilState);
}
void __cdecl C3DModelEffect::Apply(ID3D11DeviceContext* deviceContext) 
{
	//シェーダーを適用する。
	deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
	switch (m_renderMode) {
	case 0:
		//通常描画。
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTexture);
		break;
	case 1:
		//シルエット描画。
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psSilhouette.GetBody(), NULL, 0);
		//デプスステンシルステートを切り替える。
		deviceContext->OMSetDepthStencilState(m_silhouettoDepthStepsilState, 0);
		break;
	}
}