#pragma once

#include "C3DModelDraw.h"
#include "graphics/RenderTarget.h"
#include "graphics/ShadowMap.h"
#include "graphics/Sprite.h"
#include "graphics/postEffect/PostEffect.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

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
	/// <summary>
	/// �V���h�E�}�b�v���擾�B
	/// </summary>
	/// <returns></returns>
	ShadowMap* GetShadowMap()
	{
		return &m_shadowMap;
	}
	/// <summary>
	/// ���C�������_�����O�^�[�Q�b�g���擾�B
	/// </summary>
	/// <returns></returns>
	RenderTarget* GetMainRenderTarget()
	{
		return &m_mainRenderTarget;
	}
private:
	/// <summary>
	/// �v�������_�����O�B
	/// </summary>
	void PreRender();
	/// <summary>
	/// �t�H���[�h�����_�����O(�ʏ�̕`�悾�ƍl����OK)
	/// </summary>
	void ForwordRender();
	/// <summary>
	/// �|�X�g�����_�����O
	/// </summary>
	void PostRender();
		/// <summary>
	/// �J�������������B
	/// </summary>
	void InitCamera();
private:
	CSoundEngine m_soundEngine;				//�T�E���h�G���W���B
	RenderTarget m_mainRenderTarget;		//���C�������_�����O�^�[�Q�b�g�B
	RenderTarget m_depthRenderTarget;		//�[�x�l�̏������ݐ�ƂȂ郌���_�����O�^�[�Q�b�g�B
	ShadowMap m_shadowMap;					//�V���h�E�}�b�v�B
	PostEffect m_postEffect;				//�|�X�g�G�t�F�N�g�B
	int m_playBgmNo = 0;					//�Đ�����BGM�̔ԍ��B
	CSoundSource m_bgm[2];						//BGM�B
	CSoundSource m_se;						//SE�B
	CSoundSource m_se2;
	Sprite m_copyMainRtToFrameBufferSprite;			//���C�������_�����O�^�[�Q�b�g�ɕ`���ꂽ�G���t���[���o�b�t�@�ɃR�s�[���邽�߂̃X�v���C�g�B
	D3D11_VIEWPORT m_frameBufferViewports;			//�t���[���o�b�t�@�̃r���[�|�[�g�B
	ID3D11RenderTargetView* m_frameBufferRenderTargetView = nullptr;	//�t���[���o�b�t�@�̃����_�����O�^�[�Q�b�g�r���[�B
	ID3D11DepthStencilView* m_frameBufferDepthStencilView = nullptr;	//�t���[���o�b�t�@�̃f�v�X�X�e���V���r���[�B
};


