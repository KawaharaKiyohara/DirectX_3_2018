#pragma once

#include "C3DModelDraw.h"

/// <summary>
/// �Q�[���N���X�B
/// </summary>
class Game
{
public:
	/// <summary>
	/// �����_�����O���[�h�B
	/// </summary>
	enum EnRenderMode {
		enRenderMode_Normal,		//�ʏ탌���_�����O�B
		enRenderMode_Silhouette,	//�V���G�b�g�������_�����O�B
		enRenderMode_Num,			//�����_�����O���[�h�̐��B
	};
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
	/// �����_�����O�^�[�Q�b�g�̉���B
	/// </summary>
	void ReleaseRenderTarget();
	/// <summary>
	/// �����_�����O�^�[�Q�b�g�̍쐬�B
	/// </summary>
	void CreateRenderTarget();
	
private:
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//���j�e�B�����̍��W�B
	C3DModelDraw m_unityChanModelDraw;		//���j�e�B�����̃��f���̕`�揈���B�B
	C3DModelDraw m_bgModelDraw;				//�w�i�̃��f���̕`�揈���B
	//�������烌���_�����O�^�[�Q�b�g�֌W�̃����o�ϐ��ł��B
	ID3D11Texture2D*			m_renderTargetTex = nullptr;		//!<�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`���B
	ID3D11RenderTargetView*		m_renderTargetView = nullptr;		//!<�����_�[�^�[�Q�b�g�r���[�B
	ID3D11ShaderResourceView*	m_renderTargetSRV = nullptr;		//!<�����_�����O�^�[�Q�b�g��SRV
	ID3D11Texture2D*			m_depthStencilTex = nullptr;		//!<�f�v�X�X�e���V���ƂȂ�e�N�X�`���B
	ID3D11DepthStencilView*		m_depthStencilView = nullptr;		//!<�f�v�X�X�e���V���r���[�B
};


