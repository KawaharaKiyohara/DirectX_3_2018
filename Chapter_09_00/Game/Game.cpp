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
	CreateRenderTarget();
}

Game::~Game()
{
	//�����_�����O�^�[�Q�b�g�̉���B
	ReleaseRenderTarget();
}
void Game::CreateRenderTarget()
{
	//D3D�f�o�C�X���擾�B
	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	//1.�����_�����O�^�[�Q�b�g�ƂȂ�e�N�X�`�����쐬�B
	D3D11_TEXTURE2D_DESC texDesc = {0};
	{
		//////////////////////////////////////////////////////////////
		//��������e�N�X�`���쐬�B
		//////////////////////////////////////////////////////////////
		//�e�N�X�`���̉��̉𑜓x�B
		texDesc.Width = 1280;
		//�e�N�X�`���̏c�̉𑜓x�B
		texDesc.Height = 720;
		//�~�b�v�}�b�v���x���B1�ł����B
		texDesc.MipLevels = 1;
		//�e�N�X�`���z��̃T�C�Y�B1�ł����B
		texDesc.ArraySize = 1;
		//�e�N�X�`���̃t�H�[�}�b�g�B
		//RGBA�̊e�v�f8bit��32bit�t�H�[�}�b�g�ɂ���B
		//���̃p�����[�^�͍���ύX����̂ŁA���ځB
		texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//�e�N�X�`���̎g�p�p�r�����߂�B
		//�����_�����O�^�[�Q�b�g�̃e�N�X�`���́A
		//�����_�����O�^�[�Q�b�g�AShaderResourceView�AUnorderedAccessView�Ƃ��ăo�C���h����B
		texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		//�}���`�T���v�����O�̐��B1�ł����B
		texDesc.SampleDesc.Count = 1;
		//�}���`�T���v�����O�̕i���B0�ł����B
		texDesc.SampleDesc.Quality = 0;
		//�e�N�X�`���̓ǂݏ����̂�����BD3D11_USAGE_DEFAULT�ł����B
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		//CPU����̃A�N�Z�X�̎d���B�A�N�Z�X���Ȃ��̂�0�ł悢�B
		texDesc.CPUAccessFlags = 0;
		//0�ł悢�B
		texDesc.MiscFlags = 0;
		//�e�N�X�`�����쐬�B
		d3dDevice->CreateTexture2D(&texDesc, nullptr, &m_renderTargetTex);
	}
	//2.�����_�����O�^�[�Q�b�g�r���[�̍쐬
	{
		//////////////////////////////////////////////////////////////
		//�������烌���_�����O�^�[�Q�b�g�r���[�̍쐬�B
		//////////////////////////////////////////////////////////////
		//�����_�����O�^�[�Q�b�g�r���[���쐬���邽�߂̃f�[�^��ݒ肷��B�B
		D3D11_RENDER_TARGET_VIEW_DESC viewDesc;
		//�t�H�[�}�b�g�̓e�N�X�`���Ɠ����ɂ���B
		viewDesc.Format = texDesc.Format;
		//2D�e�N�X�`�����������Ƃ��w�肷��B
		viewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		//0�ł����B
		viewDesc.Texture2D.MipSlice = 0;
		//�����_�����O�^�[�Q�b�g�r���[�̍쐬�B
		d3dDevice->CreateRenderTargetView(m_renderTargetTex, &viewDesc, &m_renderTargetView);
	}
	//3.�V�F�[�_�[���\�[�X�r���[�̍쐬
	{
		
		//////////////////////////////////////////////////////////////
		//��������V�F�[�_�[���\�[�X�r���[�̍쐬�B
		//////////////////////////////////////////////////////////////
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		//�t�H�[�}�b�g�̓e�N�X�`���Ɠ����ł悢�B
		srvDesc.Format = texDesc.Format;
		//2�����e�N�X�`���Ƃ��Ĉ����B
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		//�~�b�v�}�b�v���x�����e�N�X�`���Ɠ����ł悢�B
		srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
		//0�ł����B
		srvDesc.Texture2D.MostDetailedMip = 0;
		//SRV���쐬����B
		d3dDevice->CreateShaderResourceView(m_renderTargetTex, &srvDesc, &m_renderTargetSRV);
	}
	//4.�f�v�X�X�e���V���e�N�X�`���̍쐬
	D3D11_TEXTURE2D_DESC depthTexDesc = texDesc;
	{
		//////////////////////////////////////////////////////////////
		//��������f�v�X�X�e���V���e�N�X�`���̍쐬�B
		//////////////////////////////////////////////////////////////
		//�f�v�X�X�e���V���r���[�Ƀo�C���h����B
		depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		//32bit���������_�̃f�v�X�X�e���V���o�b�t�@���쐬����B
		depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
		//�f�v�X�X�e���V���e�N�X�`�����쐬����B
		d3dDevice->CreateTexture2D(&depthTexDesc, nullptr, &m_depthStencilTex);
	}
	//5.�f�v�X�X�e���V���r���[�̍쐬
	{
		//////////////////////////////////////////////////////////////
		//��������f�v�X�X�e���V���r���[�̍쐬�B
		//////////////////////////////////////////////////////////////
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		//�t�H�[�}�b�g�͐[�x�X�e���V���Ɠ����ɂ���B
		depthStencilViewDesc.Format = depthTexDesc.Format;
		//2D�e�N�X�`�����������Ƃ��w�肷��B
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//0�ł����B
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		//0�ł����B
		depthStencilViewDesc.Flags = 0;
		//�f�v�X�X�e���V���r���[���쐬�B
		d3dDevice->CreateDepthStencilView(m_depthStencilTex, &depthStencilViewDesc, &m_depthStencilView);
	}
}
void Game::ReleaseRenderTarget()
{
	if (m_depthStencilView != nullptr) {
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	if (m_depthStencilTex != nullptr) {
		m_depthStencilTex->Release();
		m_depthStencilTex = nullptr;
	}
	
	if (m_renderTargetView != nullptr) {
		m_renderTargetView->Release();
		m_renderTargetView = nullptr;
	}
	if (m_renderTargetTex != nullptr) {
		m_renderTargetTex->Release();
		m_renderTargetTex = nullptr;
	}
	if (m_renderTargetSRV != nullptr) {
		m_renderTargetSRV->Release();
	}
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
		m_renderTargetView
	};
	d3dDeviceContext->OMSetRenderTargets(1, rts, m_depthStencilView);
	//�����_�����O�^�[�Q�b�g���N���A�B
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f }; //red,green,blue,alpha
	d3dDeviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
	d3dDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

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
		material->SetAlbedoTexture(m_renderTargetSRV);
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
