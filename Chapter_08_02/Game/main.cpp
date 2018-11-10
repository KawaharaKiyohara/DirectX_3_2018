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
	g_camera3D.SetPosition({ 0.0f, 1000.0f, 2200.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();

	//���j�e�B�����̍��W
	CVector3 unityChanPos;
	unityChanPos = { 0.0f, 0.0f, 0.0f };

	//�L�����N�^���f�����������B
	C3DModelDraw unitChanModelDraw;
	unitChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	
	//�w�i���f�����������B
	C3DModelDraw bgModelDraw;
	bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	
	//Hands-On ���̃��m����O�ɕ`�悷��[�x�X�e���V���X�e�[�g������Ă݂悤�B
	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_GREATER;
	desc.StencilEnable = false;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//D3D�f�o�C�X���擾�B
	auto d3ddevice = g_graphicsEngine->GetD3DDevice();
	//�f�v�X�X�e���V���X�e�[�g���쐬�B
	ID3D11DepthStencilState* depthStencilState;
	d3ddevice->CreateDepthStencilState(&desc, &depthStencilState);
	
	int renderMode = 0;	//�O�Ȃ�ʏ�`��A�P�Ȃ�V���G�b�g�`��B


	//�Q�[�����[�v�B
	while (DispatchWindowMessage() == true)
	{
		//�`��J�n�B
		g_graphicsEngine->BegineRender();
		
		//��O�ɕ`����s���f�v�X�X�e���V���X�e�[�g��ݒ肷��B
		g_graphicsEngine->GetD3DDeviceContext()->OMSetDepthStencilState(depthStencilState, 0);
		for (auto& pad : g_pad) {
			pad.Update();
		}
		//�J�������X�V�B
		UpdateCamera();
		//���j�e�B���������E�ɓ������B
		//�Q�[���p�b�h���Ȃ��l��4��6�œ��������B
		if (g_pad[0].IsPress(enButtonLeft) == true) {
			unityChanPos.x += 15.0f;
		}
		if (g_pad[0].IsPress(enButtonRight) == true) {
			unityChanPos.x -= 15.0f;
		}
		if (g_pad[0].IsPress(enButtonDown) == true) {
			unityChanPos.z += 15.0f;
		}
		if (g_pad[0].IsPress(enButtonUp) == true) {
			unityChanPos.z -= 15.0f;
		}
		
		//�ʏ�`�惂�[�h�ɂ���B�B
		renderMode = 0;
		//�w�i��`��B
		bgModelDraw.Draw(renderMode);

		//���j�e�B�����͐�ɃV���G�b�g��`�悵�Ă���A���ʂɕ`���B
		renderMode = 1;
		//���j�e�B������`��B
		unitChanModelDraw.Update(unityChanPos);
		unitChanModelDraw.Draw(renderMode);

		//�L������ʏ탌���_�����O�B
		renderMode = 0;
		unitChanModelDraw.Draw(renderMode);

		//�`��I���B
		g_graphicsEngine->EndRender();
	}
}