#pragma once

#include "C3DModelEffect.h"


const int NUM_DIRECTION_LIG = 4;
/*!
*@brief	�f�B���N�V�������C�g�B
*/
struct SDirectionLight {
	CVector4 direction[NUM_DIRECTION_LIG];		//���C�g�̕����B
	CVector4 color[NUM_DIRECTION_LIG];			//���C�g�̃J���[�B
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
	/// <param name="renderStep">
	///	�`�惂�[�h�BGraphicsEngine.h��EnRenderMode���Q�ƁB
	/// </param>
	/// <param name="viewMatrix">�r���[�s��</param>
	/// <param name="projMatrix">�v���W�F�N�V�����s��s��</param>
	void Draw(EnRenderMode renderMode, CMatrix viewMatrix, CMatrix projMatrix);
	/// <summary>
	/// �}�e���A���ɑ΂��ăN�G�����s���B
	/// </summary>
	/// <param name="func">�₢���킹�֐�</param>
	void QueryMaterials(std::function<void(C3DModelEffect*)> func)
	{
		m_modelDx->UpdateEffects([&](DirectX::IEffect* material) {
			func(reinterpret_cast<C3DModelEffect*>(material));
		});
	}
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
