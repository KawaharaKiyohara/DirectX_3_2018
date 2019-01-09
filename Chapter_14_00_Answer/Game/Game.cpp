#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

Game::Game()
{
	g_game = this;
	//�J�������������B
	InitCamera();
	//Unity�����̃��f�����������B
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);

	m_unityChanModelDraw2.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw2.SetDirectionLightColor(0, m_ligPower);

	m_unityChanModelDraw3.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw3.SetDirectionLightColor(0, m_ligPower);

/*	m_unityChanModelDraw.SetDirectionLightColor(2, { 2.0f,2.0f, 2.0f, 1.0f });
	m_unityChanModelDraw.SetDirectionLightColor(3, { 2.0f,2.0f, 2.0f, 1.0f });*/

	//�w�i���f���̏����������B
	m_bgModelDraw.Init(L"Assets/modelData/bg.cmo");
	//�n�ʂ��V���h�E���V�[�o�[�ɂ���B
	m_bgModelDraw.SetShadowReciever(true);
	
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
	//�J�������񂷂�
	CQuaternion qRot;
	qRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.01f);
	auto toPos = g_camera3D.GetPosition() - g_camera3D.GetTarget() ;
	qRot.Multiply(toPos);
	g_camera3D.SetPosition(g_camera3D.GetTarget() + toPos);
	g_camera3D.Update();

	m_ligPower.x += g_pad[0].IsPress(enButtonB) * 0.1f;
	m_ligPower.y += g_pad[0].IsPress(enButtonA) * 0.1f;
	m_ligPower.z += g_pad[0].IsPress(enButtonX) * 0.1f;
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetDirectionLightColor(1, m_ligPower);
	m_unityChanModelDraw2.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw2.SetDirectionLightColor(1, m_ligPower);
	m_unityChanModelDraw3.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw3.SetDirectionLightColor(1, m_ligPower);

	m_unityChanModelDraw2.Update({ 400.0f, 0.0f, -1000.0f });
	m_unityChanModelDraw3.Update({ -400.0f, 0.0f, 500.0f });

	//�V���h�E�L���X�^�[��o�^�B
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw2);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw3);
	m_shadowMap.RegistShadowCaster(&m_bgModelDraw);
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
	//�����_�����O�^�[�Q�b�g�𕡐��ݒ肷��B
	//     MRT(Multi rendering target)
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
	m_unityChanModelDraw2.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
	m_unityChanModelDraw3.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);
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
