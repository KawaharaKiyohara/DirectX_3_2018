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
	//�n�ʂ��V���h�E���V�[�o�[�ɂ���B
	m_bgModelDraw.SetShadowReciever(true);
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
	//�V���h�E�L���X�^�[��o�^�B
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_bgModelDraw);
	//�V���h�E�}�b�v���X�V�B
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);
}


void Game::Render()
{
	//�`��J�n�B
	g_graphicsEngine->BegineRender();
	///////////////////////////////////////////////
	//�V���h�E�}�b�v�Ƀ����_�����O
	///////////////////////////////////////////////
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	//���݂̃����_�����O�^�[�Q�b�g���o�b�N�A�b�v���Ă����B
	ID3D11RenderTargetView* oldRenderTargetView;
	ID3D11DepthStencilView* oldDepthStencilView;
	d3dDeviceContext->OMGetRenderTargets(
		1, 
		&oldRenderTargetView, 
		&oldDepthStencilView
	);
	//�r���[�|�[�g���o�b�N�A�b�v������Ă����B
	unsigned int numViewport = 1;
	D3D11_VIEWPORT oldViewports;
	d3dDeviceContext->RSGetViewports(&numViewport, &oldViewports);

	//�V���h�E�}�b�v�Ƀ����_�����O
	m_shadowMap.RenderToShadowMap();

	//���ɖ߂��B
	d3dDeviceContext->OMSetRenderTargets(
		1, 
		&oldRenderTargetView, 
		oldDepthStencilView
	);
	d3dDeviceContext->RSSetViewports(numViewport, &oldViewports);
	//�����_�����O�^�[�Q�b�g�ƃf�v�X�X�e���V���̎Q�ƃJ�E���^�������B
	oldRenderTargetView->Release();
	oldDepthStencilView->Release();

	///////////////////////////////////////////////
	//��������ʏ탌���_�����O�B
	///////////////////////////////////////////////
	m_bgModelDraw.Draw(
		enRenderMode_Normal, 
		g_camera3D.GetViewMatrix(), 
		g_camera3D.GetProjectionMatrix()
	);
	m_unityChanModelDraw.Draw(
		enRenderMode_Normal, 
		g_camera3D.GetViewMatrix(), 
		g_camera3D.GetProjectionMatrix()
	);

	//�`��I���B
	g_graphicsEngine->EndRender();
}
