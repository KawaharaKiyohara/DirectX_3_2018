#include "stdafx.h"
#include "C3DModelEffect.h"
#include "Game.h"

C3DModelEffect::C3DModelEffect()
{
	//頂点シェーダーをロード。
	m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
	m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	//todo シャドウマップ用のシェーダーをロード。
	m_psShadowMap.Load("Assets/shader/model.fx", "PSMain_ShadowMap", Shader::EnType::PS);
	m_vsShadowMap.Load("Assets/shader/model.fx", "VSMain_ShadowMap", Shader::EnType::VS);
}

C3DModelEffect::~C3DModelEffect()
{
	if (m_albedoTexture != nullptr) {
		m_albedoTexture->Release();
	}
}


void __cdecl C3DModelEffect::Apply(ID3D11DeviceContext* deviceContext) 
{
	
	switch (m_renderMode) {
	case enRenderMode_Normal: {
		//通常描画。
		//シェーダーを適用する。
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
		//todo シェーダーリソースビューを一気に設定する。
		ID3D11ShaderResourceView* srvArray[] = {
			m_albedoTexture,							//アルベドテクスチャ。
			g_game->GetShadowMap()->GetShadowMapSRV()	//シャドウマップ。
		};
		deviceContext->PSSetShaderResources(0, 2, srvArray);
	}break;
	case enRenderMode_CreateShadowMap:
		//シャドウマップ生成。
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShadowMap.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShadowMap.GetBody(), NULL, 0);
		break;
	}
}