#pragma once

#include "C3DModelDraw.h"
#include "RenderTarget.h"
#include "ShadowMap.h"

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
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//���j�e�B�����̍��W�B
	C3DModelDraw m_unityChanModelDraw;		//���j�e�B�����̃��f���̕`�揈���B�B
	C3DModelDraw m_bgModelDraw;				//�w�i�̃��f���̕`�揈���B
	ShadowMap m_shadowMap;					//�V���h�E�}�b�v�B
};


