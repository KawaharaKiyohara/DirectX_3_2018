#include "stdafx.h"
#include "Game.h"


Game::Game()
{
	//�J������ݒ�B
	g_camera3D.SetPosition({ 0.0f, 1000.0f, 2200.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();
	//Unity�����̃��f�����������B
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	//�w�i���f���̏����������B
	m_bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	//�����_�����O�^�[�Q�b�g�̍쐬�B
	m_renderTarget.Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
}

Game::~Game()
{
}


void Game::Update()
{
	//�Q�[���p�b�h�̍X�V�B
	for (auto& pad : g_pad) {
		pad.Update();
	}
	m_unityChanPos.x += g_pad[0].GetLStickXF() * -50.0f;
	m_unityChanPos.z += g_pad[0].GetLStickYF() * -50.0f;

	m_unityChanModelDraw.Update(m_unityChanPos);
}
void Game::Render()
{
	//�����_�����O���[�h�B
	EnRenderMode renderMode = enRenderMode_Normal;
	//�`��J�n�B
	g_graphicsEngine->BegineRender();

	///////////////////////////////////////////////
	//�܂��̓I�t�X�N���[�������_�����O
	///////////////////////////////////////////////
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	//���݂̃����_�����O�^�[�Q�b�g���o�b�N�A�b�v���Ă����B
	ID3D11RenderTargetView* oldRenderTargetView;
	ID3D11DepthStencilView* oldDepthStencilView;
	d3dDeviceContext->OMGetRenderTargets(1, &oldRenderTargetView, &oldDepthStencilView);
	//�����_�����O�^�[�Q�b�g��؂�ւ���B
	ID3D11RenderTargetView* rts[] = {
		m_renderTarget.GetRenderTargetView()
	};
	d3dDeviceContext->OMSetRenderTargets(1, rts, m_renderTarget.GetDepthStensilView());
	//�����_�����O�^�[�Q�b�g���N���A�B
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	m_renderTarget.ClearRenderTarget(clearColor);

	//�w�i��`��B
	m_bgModelDraw.Draw(renderMode);
	//���j�e�B������ʏ탌���_�����O�B
	m_unityChanModelDraw.Draw(renderMode);


	///////////////////////////////////////////////
	//��������I���X�N���[�������_�����O�B
	///////////////////////////////////////////////
	//�����_�����O�^�[�Q�b�g�����ɖ߂��B
	d3dDeviceContext->OMSetRenderTargets(1, &oldRenderTargetView, oldDepthStencilView);
	//�����_�����O�^�[�Q�b�g�ƃf�v�X�X�e���V���̎Q�ƃJ�E���^�������B
	oldRenderTargetView->Release();
	oldDepthStencilView->Release();

	//�w�i���f���Ŏg�p����e�N�X�`�����I�t�X�N���[�������_�����O�������̂ɍ����ւ��Ă����B
	m_bgModelDraw.QueryMaterials([&](C3DModelEffect* material) {
		//�����ւ��O�̃e�N�X�`�����X�^�b�N�ɑޔ��B
		material->PushAlbedoTexture();
		//�e�N�X�`���������ւ���B
		material->SetAlbedoTexture(m_renderTarget.GetRenderTargetSRV());
	});
	//�����_�����O�^�[�Q�b�g��߂����̂ŁA�w�i�ƃ��j�e�B������
	//��ʂɕ`�悷��B
	m_bgModelDraw.Draw(renderMode);
	m_unityChanModelDraw.Draw(renderMode);

	//�w�i�̃e�N�X�`�������ɖ߂��B
	m_bgModelDraw.QueryMaterials([&](C3DModelEffect* material) {
		//�����ւ��O�̃e�N�X�`�����X�^�b�N���畜�A�B
		material->PopAlbedoTexture();	
	});

	//�`��I���B
	g_graphicsEngine->EndRender();
}
