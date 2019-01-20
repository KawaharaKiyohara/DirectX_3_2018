#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;

	//�T�E���h�G���W�����������B
	m_soundEngine.Init();

	//BGM
	m_bgm[0].Init(L"Assets/sound/bgm_00.wav");
	m_bgm[0].Play(true);
	m_bgm[1].Init(L"Assets/sound/bgm_01.wav");
	

	//�����V���b�g�Đ���SE
	m_se.Init(L"Assets/sound/coinGet.wav");

	m_se2.Init(L"Assets/sound/uni1482.wav");

	//�J�������������B
	InitCamera();
		
	//���C���ƂȂ郌���_�����O�^�[�Q�b�g���쐬����B
	m_mainRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);
	//�J������Ԃł̐[�x�l�̏������ݐ�ƂȂ郌���_�����O�^�[�Q�b�g���쐬����B
	//�t�H�[�}�b�g��R�����݂̂�32�r�b�g���������_�o�b�t�@�B
	m_depthRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R32_FLOAT
	);
	//���C�������_�����O�^�[�Q�b�g�ɕ`���ꂽ�G��
	//�t���[���o�b�t�@�ɃR�s�[���邽�߂̃X�v���C�g������������B
	m_copyMainRtToFrameBufferSprite.Init(
		m_mainRenderTarget.GetRenderTargetSRV(),
		FRAME_BUFFER_W,
		FRAME_BUFFER_H
	);
	//�|�X�g�G�t�F�N�g�̏������B
	m_postEffect.Init();
}

Game::~Game()
{
	if (m_frameBufferRenderTargetView != nullptr) {
		m_frameBufferRenderTargetView->Release();
	}
	if (m_frameBufferDepthStencilView != nullptr) {
		m_frameBufferDepthStencilView->Release();
	}
}

void Game::InitCamera()
{
	g_camera3D.SetPosition({ 0.0f, 400.0f, 1000.0f });
	g_camera3D.SetTarget({ 0.0f, 0.0f, 0.0f });
	g_camera3D.Update();
	g_camera2D.SetUpdateProjMatrixFunc(Camera::enUpdateProjMatrixFunc_Ortho);
	g_camera2D.SetWidth(FRAME_BUFFER_W);
	g_camera2D.SetHeight(FRAME_BUFFER_H);
	g_camera2D.SetPosition({0.0f, 0.0f, -10.0f});
	g_camera2D.SetTarget(CVector3::Zero());
	g_camera2D.Update();
}

void Game::Update()
{
	//�Q�[���p�b�h�̍X�V�B
	for (auto& pad : g_pad) {
		pad.Update();
	}
	//�T�E���h�G���W�����X�V�B
	m_soundEngine.Update();

	if (g_pad[0].IsTrigger(enButtonA)) {
		//A�{�^���������ꂽ��SE��炷�B
		m_se.Play(false);
	}
	if (g_pad[0].IsTrigger(enButtonB)) {
		//B�{�^���������ꂽ��SE��炷�B
		m_se2.Play(false);
	}
	if (g_pad[0].IsTrigger(enButtonY)) {
		//Y�{�^���������ꂽ��BGM��؂�ւ���B]
		m_bgm[m_playBgmNo].Pause();
		m_playBgmNo++;
		m_playBgmNo = m_playBgmNo % 2;
		m_bgm[m_playBgmNo].Play(true);
	}
	//�V���h�E�}�b�v���X�V�B
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);
	//�|�X�g�G�t�F�N�g�̍X�V�B
	m_postEffect.Update();
}

void Game::PreRender()
{
	//�V���h�E�}�b�v�Ƀ����_�����O
	m_shadowMap.RenderToShadowMap();
}
	
void Game::ForwordRender()
{
	//MRT(Multi rendering target)
	ID3D11RenderTargetView* rts[] = {
		m_mainRenderTarget.GetRenderTargetView(),	//0�Ԗڂ̓��C�������_�����O�^�[�Q�b�g
		m_depthRenderTarget.GetRenderTargetView(),	//1�Ԗڂ̓J������Ԃł̐[�x�l���o�͂��郌���_�����O�^�[�Q�b�g�B
	};
	auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
	deviceContext->OMSetRenderTargets(
		2,											//�������̓Z�b�g���郌���_�����O�^�[�Q�b�g�̐��B2�ݒ肷��̂�2���w�肷��B
		rts,										//�������ɂ̓����_�����O�^�[�Q�b�g�̔z����w�肷��B
		m_mainRenderTarget.GetDepthStensilView()	//��O�����͐[�x�X�e���V���o�b�t�@�B����͈�����w��ł��Ȃ��B
	);
	deviceContext->RSSetViewports(1, m_mainRenderTarget.GetViewport());

	//���C�������_�����O�^�[�Q�b�g���N���A����B
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_mainRenderTarget.ClearRenderTarget(clearColor);
	m_depthRenderTarget.ClearRenderTarget(clearColor);

}
void Game::PostRender()
{
	//�|�X�g�G�t�F�N�g�̕`��B
	m_postEffect.Draw();

	//�����_�����O�^�[�Q�b�g���t���[���o�b�t�@�ɖ߂��B
	g_graphicsEngine->ChangeRenderTarget(
		m_frameBufferRenderTargetView,
		m_frameBufferDepthStencilView,
		&m_frameBufferViewports
	);
	//�h���h��
	m_copyMainRtToFrameBufferSprite.Draw();
	
	m_frameBufferRenderTargetView->Release();
	m_frameBufferDepthStencilView->Release();
}
void Game::Render()
{
	//�`��J�n�B
	g_graphicsEngine->BegineRender();
	//�t���[���o�b�t�@�̂̃����_�����O�^�[�Q�b�g���o�b�N�A�b�v���Ă����B
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	d3dDeviceContext->OMGetRenderTargets(
		1,
		&m_frameBufferRenderTargetView,
		&m_frameBufferDepthStencilView
	);
	//�r���[�|�[�g���o�b�N�A�b�v������Ă����B
	unsigned int numViewport = 1;
	d3dDeviceContext->RSGetViewports(&numViewport, &m_frameBufferViewports);

	//�v�������_�����O
	PreRender();
	
	//�t�H���[�h�����_�����O�B
	ForwordRender();
	
	//�|�X�g�����_�����O
	PostRender();	

	//�`��I���B
	g_graphicsEngine->EndRender();
}
