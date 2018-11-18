#pragma once

#include "C3DModelDraw.h"
#include "RenderTarget.h"

/// <summary>
/// �Q�[���N���X�B
/// </summary>
class Game
{
	
public:
	/// <summary>
	/// �R���X�g���N�^�B
	/// </summary>
	Game();
	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	~Game();

	/// <summary>
	/// �X�V�����B
	/// </summary>
	void Update();
	/// <summary>
	/// �`�揈���B
	/// </summary>
	void Render();
private:
	/// <summary>
	/// �V���h�E�}�b�v�Ƀ����_�����O�B
	/// </summary>
	void RenderToShadowMap();
	/// <summary>
	/// �V���h�E�}�b�v�֌W�̍X�V�����B
	/// </summary>
	/// <remarks>
	/// ���C�g�r���[�E�v���W�F�N�V�����s��Ȃǂ��v�Z����܂��B
	/// </remarks>
	void UpdateShadowMap();
private:
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//���j�e�B�����̍��W�B
	C3DModelDraw m_unityChanModelDraw;		//���j�e�B�����̃��f���̕`�揈���B�B
	C3DModelDraw m_bgModelDraw;				//�w�i�̃��f���̕`�揈���B

	CVector3 m_lightCameraPosition;				//���C�g�J�����̎��_�B
	CVector3 m_lightCameraTarget;				//���C�g�J�����̒����_�B
	CMatrix m_lightViewMatrix;					//���C�g�r���[�s��B
	CMatrix m_lightProjMatrix;					//���C�g�v���W�F�N�V�����s��B
	RenderTarget m_shadowMapRT;					//�V���h�E�}�b�v��`�悷�郌���_�����O�^�[�Q�b�g�B
};


