#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;

	//�T�E���h�G���W�����������B
	m_soundEngine.Init();

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

	//Effekseer���������B
	InitEffekseer();

	//�T���v���̃G�t�F�N�g�����[�h����B
	m_sampleEffect = Effekseer::Effect::Create(m_effekseerManager, (const EFK_CHAR*)L"Assets/effect/test.efk");
	//�G�t�F�N�g���Đ�����B
	m_playEffectHandle = m_effekseerManager->Play(m_sampleEffect, 0.0f, 0.0f, 0.0f);
}

Game::~Game()
{
	if (m_frameBufferRenderTargetView != nullptr) {
		m_frameBufferRenderTargetView->Release();
	}
	if (m_frameBufferDepthStencilView != nullptr) {
		m_frameBufferDepthStencilView->Release();
	}
	if (m_effekseerManager != nullptr) {
		m_effekseerManager->Destroy();

	}
	if (m_effekseerRenderer != nullptr) {
		m_effekseerRenderer->Destroy();
	}
	//�G�t�F�N�g��j���B
	if (m_sampleEffect != nullptr) {
		m_sampleEffect->Release();
	}
}

void Game::InitEffekseer()
{
	//�����_���[���������B
	m_effekseerRenderer = EffekseerRendererDX11::Renderer::Create(
		g_graphicsEngine->GetD3DDevice(),			//D3D�f�o�C�X�B
		g_graphicsEngine->GetD3DDeviceContext(),	//D3D�f�o�C�X�R���e�L�X�g�B
		20000										//�|���̍ő吔�B
	);
	//�G�t�F�N�g�}�l�[�W�����������B
	m_effekseerManager = Effekseer::Manager::Create(10000);

	// �`��p�C���X�^���X����`��@�\��ݒ�
	m_effekseerManager->SetSpriteRenderer(m_effekseerRenderer->CreateSpriteRenderer());
	m_effekseerManager->SetRibbonRenderer(m_effekseerRenderer->CreateRibbonRenderer());
	m_effekseerManager->SetRingRenderer(m_effekseerRenderer->CreateRingRenderer());
	m_effekseerManager->SetTrackRenderer(m_effekseerRenderer->CreateTrackRenderer());
	m_effekseerManager->SetModelRenderer(m_effekseerRenderer->CreateModelRenderer());

	// �`��p�C���X�^���X����e�N�X�`���̓Ǎ��@�\��ݒ�
	// �Ǝ��g���\�A���݂̓t�@�C������ǂݍ���ł���B
	m_effekseerManager->SetTextureLoader(m_effekseerRenderer->CreateTextureLoader());
	m_effekseerManager->SetModelLoader(m_effekseerRenderer->CreateModelLoader());
	
}
void Game::InitCamera()
{
	g_camera3D.SetPosition({ 50.0f, 0.0f, 0.0f });
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

	//�V���h�E�}�b�v���X�V�B
	m_shadowMap.UpdateFromLightTarget(
		{ 1000.0f, 1000.0f, 1000.0f },
		{ 0.0f, 0.0f, 0.0f }
	);
	//�|�X�g�G�t�F�N�g�̍X�V�B
	m_postEffect.Update();

	//Effekseer�J�����s���ݒ�B
	//�܂���Effeseer�̍s��^�̕ϐ��ɁA�J�����s��ƃv���W�F�N�V�����s����R�s�[�B
	Effekseer::Matrix44 efCameraMat;
	g_camera3D.GetViewMatrix().CopyTo(efCameraMat);
	Effekseer::Matrix44 efProjMat;
	g_camera3D.GetProjectionMatrix().CopyTo(efProjMat);
	//�J�����s��ƃv���W�F�N�V�����s���ݒ�B
	m_effekseerRenderer->SetCameraMatrix(efCameraMat);
	m_effekseerRenderer->SetProjectionMatrix(efProjMat);
	//Effekseer���X�V�B
	m_effekseerManager->Update();

	if (g_pad[0].IsTrigger(enButtonA)) {
		//�Đ����̃G�t�F�N�g���~�߂�B
		m_effekseerManager->StopEffect(m_playEffectHandle);
		//�Đ��B
		m_playEffectHandle = m_effekseerManager->Play(m_sampleEffect, 0.0f, 0.0f, 0.0f);
	}
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


	//�G�t�F�N�g�͕s�����I�u�W�F�N�g��`�悵����ŕ`�悷��B
	m_effekseerRenderer->BeginRendering();
	m_effekseerManager->Draw();
	m_effekseerRenderer->EndRendering();
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
