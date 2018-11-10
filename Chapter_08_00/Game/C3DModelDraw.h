#pragma once
#include "graphics/Shader.h"



const int NUM_DIRECTION_LIG = 4;
/*!
*@brief	�f�B���N�V�������C�g�B
*/
struct SDirectionLight {
	CVector4 direction[NUM_DIRECTION_LIG];		//���C�g�̕����B
	CVector4 color[NUM_DIRECTION_LIG];			//���C�g�̃J���[�B
};

/*!
*@brief	���f���G�t�F�N�g�B
*@details
* DirectX::Model�̕`�揈���Ŏg�p�����V�F�[�_�[�������ւ��邽�߂̃N���X
*/
class C3DModelEffect : public DirectX::IEffect {
private:
	Shader m_vsShader;
	Shader m_psShader;
	ID3D11ShaderResourceView* m_albedoTexture = nullptr;
public:
	//�R���X�g���N�^�B
	C3DModelEffect()
	{
		//���_�V�F�[�_�[�����[�h�B
		m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
		m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	}
	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	~C3DModelEffect()
	{
		if (m_albedoTexture != nullptr) {
			m_albedoTexture->Release();
		}
	}
	//���̊֐���DirectX::Model::Draw�����̃h���[�R�[���̒��O�ɌĂ΂��B
	//�Ȃ̂ŁA���̊֐��̂Ȃ��ŁA�V�F�[�_�[�̐ݒ��A�e�N�X�`���̐ݒ�Ȃǂ��s���Ƃ悢�B
	void __cdecl Apply(ID3D11DeviceContext* deviceContext) override
	{
		//�V�F�[�_�[��K�p����B
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	}
	//���̊֐���DirectX::Model�̏�������������Ă΂��B
	//���_�V�F�[�_�[�̃o�C�g�R�[�h�ƃR�[�h�̒�����ݒ肷��K�v������B
	void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
	{
		*pShaderByteCode = m_vsShader.GetByteCode();
		*pByteCodeLength = m_vsShader.GetByteCodeSize();
	}
	void SetAlbedoTexture(ID3D11ShaderResourceView* texSRV)
	{
		m_albedoTexture = texSRV;
		//�Q�ƃJ�E���^���グ��B
		m_albedoTexture->AddRef();
	}
};
/*!
*@brief
*  �G�t�F�N�g�t�@�N�g���B
*@detail
* �G�t�F�N�g�̃t�@�N�g���B�}�e���A���̏��ɉ����č쐬����G�t�F�N�g��ύX����N���X�B
*
*/
class C3DModelEffectFactory : public DirectX::EffectFactory {
public:
	C3DModelEffectFactory(ID3D11Device* device) :
		EffectFactory(device) {}
	//�G�t�F�N�g���쐬�B
	std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(const EffectInfo& info, ID3D11DeviceContext* deviceContext)override
	{
		std::shared_ptr<C3DModelEffect> effect = std::make_shared< C3DModelEffect>();

		
		if (info.diffuseTexture && *info.diffuseTexture)
		{
			//�A���x�h�e�N�X�`�����\���Ă���B
			ID3D11ShaderResourceView* texSRV;
			DirectX::EffectFactory::CreateTexture(info.diffuseTexture, deviceContext, &texSRV);
			effect->SetAlbedoTexture(texSRV);
			texSRV->Release();
		}

		return effect;
	}
};
/*!
*@brief
*  3D���f���`��N���X�B
*/
class C3DModelDraw {
	//�萔�o�b�t�@�B
	struct SVSConstantBuffer {
		CMatrix mWorld;
		CMatrix mView;
		CMatrix mProj;
	};

	std::unique_ptr<DirectX::Model>		m_modelDx;				//DirectXTK���񋟂��郂�f���B
	ID3D11Buffer*						m_cb = nullptr;			//!<�萔�o�b�t�@�B
	ID3D11Buffer*						m_lightCb = nullptr;	//!<���C�g�p�̒萔�o�b�t�@�B
	SDirectionLight						m_dirLight;				//!<�f�B���N�V�������C�g�B
	ID3D11SamplerState* m_samplerState = nullptr;			//!<�T���v���[�X�e�[�g�B
	CMatrix m_worldMatrix = CMatrix::Identity();			//!<���[���h�s��B
public:
	//�f�X�g���N�^�B
	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	~C3DModelDraw();

	/// <summary>
	/// �������B
	/// </summary>
	/// <param name="filePath">���[�h����cmo�t�@�C���̃t�@�C���p�X�B</param>
	void Init(const wchar_t* filePath);

	/// <summary>
	/// �X�V�B
	/// </summary>
	/// <param name="trans">���s�ړ�</param>
	/// <param name="qRot">��]</param>
	/// <param name="scale">�g�嗦</param>
	void Update(CVector3 trans);
	/// <summary>
	/// �`��B
	/// </summary>
	void Draw();
	
private:
	//�f�B���N�V�������C�g�̏������B
	/// <summary>
	/// �f�B���N�V�������C�g�̏������B
	/// </summary>
	void InitDirectionLight();

	/// <summary>
	/// �T���v���X�e�[�g�̏������B
	/// </summary>
	void InitSamplerState();

	/// <summary>
	/// �萔�o�b�t�@�̏������B
	/// </summary>
	void InitConstantBuffer();
	
};
