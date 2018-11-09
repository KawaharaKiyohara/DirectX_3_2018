#include "stdafx.h"
#include "system/system.h"
#include "graphics/Shader.h"
#include "C3DModelDraw.h"

using namespace std;


/// <summary>
/// �J�����̍X�V
/// </summary>
void UpdateCamera()
{
	//�����_���王�_�Ɍ������ĐL�т�x�N�g�����v�Z����B
	CVector3 toPos = g_camera3D.GetPosition() - g_camera3D.GetTarget();
	//�J�������񂷃N�H�[�^�j�I�����쐬����B
	CQuaternion qAddRot;
	//�Q�[���p�b�h�̍��X�e�B�b�N�ŃJ���������B
	//�Q�[���p�b�h���Ȃ��l�̓L�[�{�[�h��'a'��'d'�ŉ񂹂��B
	qAddRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.01f);
	//�����_���王�_�Ɍ������ĐL�т�x�N�g�����񂷁B
	qAddRot.Multiply(toPos);
	//�V�������_���v�Z����B
	g_camera3D.SetPosition(g_camera3D.GetTarget() + toPos);
	g_camera3D.Update();
}

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐��B
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");
	
	//�J������ݒ�B
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();
	//�I�u�W�F�N�g�̍��W
	CVector3 pos1, pos2;
	pos1 = { 0.0f, 0.0f, 0.0f };
	pos2 = { 100.0f, 0.0f, -100.0f };

	//��O�ɕ`�����f�����������B
	C3DModelDraw modelDraw1;
	modelDraw1.Init(L"Assets/modelData/unityChan1.cmo");

	//���ɕ`�����f�����������B
	C3DModelDraw modelDraw2;
	modelDraw2.Init(L"Assets/modelData/unityChan2.cmo");
	//�Q�[�����[�v�B
	while (DispatchWindowMessage() == true)
	{
		//�`��J�n�B
		g_graphicsEngine->BegineRender();
		
		for (auto& pad : g_pad) {
			pad.Update();
		}
		//�J�������X�V�B
		UpdateCamera();
		//���̃��f���������E�ɓ������B
		//�Q�[���p�b�h���Ȃ��l��4��6�œ��������B
		if (g_pad[0].IsPress(enButtonLeft) == true) {
			pos2.x += 2.0f;
		}
		if (g_pad[0].IsPress(enButtonRight) == true) {
			pos2.x -= 2.0f;
		}
		//Chapter 8.1.2 Hands-On �`�悷�鏇�Ԃ�ς��邱�ƂŐ������`�悳��邱�Ƃ��m�F����B

		//��O�ɕ`��B
		modelDraw1.Update(pos1);
		modelDraw1.Draw();

		//���ɕ`��B
		modelDraw2.Update(pos2);
		modelDraw2.Draw();
		
		//�`��I���B
		g_graphicsEngine->EndRender();
	}
}