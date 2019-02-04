#include "stdafx.h"
#include "Game.h"
#include "util/Stopwatch.h"

ID3D11ShaderResourceView* g_normalMapSRV = nullptr;
ID3D11ShaderResourceView* g_specMapSRV = nullptr;

Game::Game()
{
	//�J�������������B
	InitCamera();

	CVector3 ambientColor = { 0.7f, 0.7f, 0.7f };
	//Unity�����̃��f�����������B
	m_unityChanModelDraw.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw.SetAmbientLight(ambientColor);
	
	m_unityChanModelDraw_NoAO.Init(L"Assets/modelData/unityChan.cmo");
	m_unityChanModelDraw_NoAO.SetDirectionLightColor(0, m_ligPower);
	m_unityChanModelDraw_NoAO.SetAmbientLight(ambientColor);

	//Unity�����̖@���}�b�v�����[�h�B
	//�t�@�C�������g���āA�e�N�X�`�������[�h���āAShaderResrouceView���쐬����B
	DirectX::CreateDDSTextureFromFileEx(
		g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_nomal.dds", 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		false, nullptr, &g_normalMapSRV);

	//Unity�����̃X�y�L�����}�b�v�����[�h�B
	//�t�@�C�������g���āA�e�N�X�`�������[�h���āAShaderResourceView���쐬����B
	DirectX::CreateDDSTextureFromFileEx(
		g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_spec.dds", 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		false, nullptr, &g_specMapSRV);

	
	//���f���ɖ@���}�b�v��ݒ肷��B
	m_unityChanModelDraw.SetNormalMap(g_normalMapSRV);
	m_unityChanModelDraw_NoAO.SetNormalMap(g_normalMapSRV);
	//���f���ɃX�y�L�����}�b�v��ݒ肷��B
	m_unityChanModelDraw.SetSpecularMap(g_specMapSRV);
	m_unityChanModelDraw_NoAO.SetSpecularMap(g_specMapSRV);

	//���C���ƂȂ郌���_�����O�^�[�Q�b�g���쐬����B
	m_mainRenderTarget.Create(
		FRAME_BUFFER_W,
		FRAME_BUFFER_H,
		DXGI_FORMAT_R16G16B16A16_FLOAT
	);

	//���C�������_�����O�^�[�Q�b�g�ɕ`���ꂽ�G��
	//�t���[���o�b�t�@�ɃR�s�[���邽�߂̃X�v���C�g������������B
	m_copyMainRtToFrameBufferSprite.Init(
		m_mainRenderTarget.GetRenderTargetSRV(),
		FRAME_BUFFER_W,
		FRAME_BUFFER_H
	);
}

Game::~Game()
{
	if (m_frameBufferRenderTargetView != nullptr) {
		m_frameBufferRenderTargetView->Release();
	}
	if (m_frameBufferDepthStencilView != nullptr) {
		m_frameBufferDepthStencilView->Release();
	}
	if (g_normalMapSRV != nullptr) {
		g_normalMapSRV->Release();
	}
	if (g_specMapSRV != nullptr) {
		g_specMapSRV->Release();
	}
}

void Game::InitCamera()
{
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
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
	CQuaternion qRot;
	qRot.SetRotation(CVector3::AxisY(), g_pad[0].GetLStickXF() * 0.1f);
	qRot.Multiply(m_ligDirection);
	
	m_unityChanModelDraw.SetDirectionLightDirection(0, m_ligDirection);
	m_unityChanModelDraw_NoAO.SetDirectionLightDirection(0, m_ligDirection);
	
	//�V���h�E�L���X�^�[��o�^�B
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw);
	m_shadowMap.RegistShadowCaster(&m_unityChanModelDraw_NoAO);

	m_unityChanModelDraw.Update({ 130.0f, 0.0f, 0.0f });
	m_unityChanModelDraw_NoAO.Update({ -130.0f, 0.0f, 0.0f });

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
	//�����_�����O�^�[�Q�b�g�����C���ɕύX����B
	g_graphicsEngine->ChangeRenderTarget(&m_mainRenderTarget, &m_frameBufferViewports);
	//���C�������_�����O�^�[�Q�b�g���N���A����B
	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_mainRenderTarget.ClearRenderTarget(clearColor);


	m_unityChanModelDraw.Draw(
		enRenderMode_Normal,
		g_camera3D.GetViewMatrix(),
		g_camera3D.GetProjectionMatrix()
	);

	m_unityChanModelDraw_NoAO.Draw(
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
	Stopwatch sw;
	sw.Start();
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
	sw.Stop();
	char text[256];
	sprintf(text, "frame delta time(�P�ʁF�~���b) = %f\n", sw.GetElapsedMillisecond());
	OutputDebugString(text);
}
