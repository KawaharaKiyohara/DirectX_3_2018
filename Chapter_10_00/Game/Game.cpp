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
	//�V���h�E�}�b�v�����p�̃����_�����O�^�[�Q�b�g���쐬�B
	//�𑜓x��2048�~2048�B
	//�e�N�X�`���̃t�H�[�}�b�g��R�����݂̂�32bit���������_�^�B
	m_shadowMapRT.Create(
		2048, 
		2048, 
		DXGI_FORMAT_R32_FLOAT
	);
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
	//todo �V���h�E�}�b�v�֌W�̍X�V�����B
	m_lightCameraPosition = { 0.0f, 1000.0f, 0.0f };
	m_lightCameraTarget = { 0.0f, 0.0f, 0.0f };
	UpdateShadowMap();
}
void Game::UpdateShadowMap()
{
	//���C�g�̕������v�Z����B
	auto lightDir = m_lightCameraTarget - m_lightCameraPosition;
	if (lightDir.Length() < 0.0001f) {
		//���C�g�J�����̒����_�Ǝ��_���߂�����B
		//���炭�o�O�Ȃ̂ŁA�N���b�V��������B
		std::abort();
	}
	//���K�����āA�����x�N�g���ɕϊ�����B
	lightDir.Normalize();
	//���C�g�̕����ɂ���āA���C�g�J�����̏���������߂�B
	CVector3 lightCameraUpAxis;
	if (fabsf(lightDir.y) > 0.99998f) {
		//�قڐ^��or�^���������Ă���̂ŁA1,0,0��������Ƃ���B
		lightCameraUpAxis = CVector3::AxisX();
	}
	else {
		//�^��������Ă��Ȃ��Ƃ��́A0,1,0��������Ƃ���B
		lightCameraUpAxis = CVector3::AxisY();
	}
	//�J�����̏���������܂����̂ŁA���C�g�r���[�s����v�Z����B
	m_lightViewMatrix.MakeLookAt(
		m_lightCameraPosition, 
		m_lightCameraTarget, 
		lightCameraUpAxis
	);

	//���C�g�v���W�F�N�V�����s����쐬����B
	//���z������̉e�𗎂Ƃ������Ȃ�A���s���e�s����쐬����B
	m_lightProjMatrix.MakeOrthoProjectionMatrix(
		3000,
		3000,
		0.1f,
		5000.0f
	);
}
void Game::RenderToShadowMap()
{
	auto d3dDeviceContext = g_graphicsEngine->GetD3DDeviceContext();
	//�����_�����O�^�[�Q�b�g��؂�ւ���B
	ID3D11RenderTargetView* rts[] = {
		m_shadowMapRT.GetRenderTargetView()
	};
	d3dDeviceContext->OMSetRenderTargets(1, rts, m_shadowMapRT.GetDepthStensilView());
	//�r���[�|�[�g��ݒ�B
	d3dDeviceContext->RSSetViewports(1, m_shadowMapRT.GetViewport());
	//�V���h�E�}�b�v���N���A�B
	//��ԉ���Z��1.0�Ȃ̂ŁA1.0�œh��Ԃ��B
	float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //red,green,blue,alpha
	m_shadowMapRT.ClearRenderTarget(clearColor);

	//�V���h�E�L���X�^�[�ƂȂ郂�f����`�悷��B
	//����͑S���V���h�E�L���X�^�[�B
	//�w�i��`��B
	m_bgModelDraw.Draw(
		enRenderMode_CreateShadowMap, 
		m_lightViewMatrix, 
		m_lightProjMatrix
	);
	//���j�e�B������ʏ탌���_�����O�B
	m_unityChanModelDraw.Draw(
		enRenderMode_CreateShadowMap, 
		m_lightViewMatrix, 
		m_lightProjMatrix
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
	RenderToShadowMap();

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
