#pragma once

#include "RenderTarget.h"
#include "GaussianBlur.h"

class PostEffect;

/// <summary>
/// ��ʊE�[�x
/// </summary>
/// <remarks>
/// Dof�N���X�̃C���X�^���X��PostEffect�N���X���ێ����Ă��܂��B
/// </remarks>
class Dof
{
public:

	/// <summary>
	/// �f�X�g���N�^�B
	/// </summary>
	/// <remarks>
	/// �I���������L�q���Ă��������B
	/// </remarks>
	~Dof();
	/// <summary>
	/// �������B
	/// </summary>
	/// <remarks>
	/// PostEffect::Init�֐�����Ăяo����Ă��܂��B
	/// �������������L�q���Ă��������B
	/// </remarks>
	void Init();
	/// <summary>
	/// �`��B
	/// </summary>
	/// <param name="postEffect"></param>
	void Draw(PostEffect& postEffect);
private:
	//�K�v�ȃ����o�ϐ��͒ǉ����Ă����ĂˁB
};

