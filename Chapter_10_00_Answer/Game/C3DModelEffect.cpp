#include "stdafx.h"
#include "C3DModelEffect.h"

C3DModelEffect::C3DModelEffect()
{
	//���_�V�F�[�_�[�����[�h�B
	m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
	m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	//todo �V���h�E�}�b�v�p�̃V�F�[�_�[�����[�h�B
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
	case enRenderMode_Normal:
		//�ʏ�`��B
		//�V�F�[�_�[��K�p����B
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTexture);
		break;
	case enRenderMode_CreateShadowMap:
		//todo �V���h�E�}�b�v�����p�̃V�F�[�_�[��ݒ�B
		//�V���h�E�}�b�v�����B
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShadowMap.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShadowMap.GetBody(), NULL, 0);
		break;
	}
}