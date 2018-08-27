#include "stdafx.h"
#include "system/system.h"
#include "level/Level.h"
#include "graphics/Shader.h"


//�O�p�`�`��N���X�B
class TriangleDraw {
	
	Shader m_vsShader;							//!<VS�V�F�[�_�[�B
	Shader m_psShader;							//!<PS�V�F�[�_�[�B
	ID3D11Buffer*	m_vertexBuffer = nullptr;	//!<���_�o�b�t�@�B

	struct SSimpleVertex {
		CVector4 pos;

	};
public:
	//�R���X�g���N�^�B
	TriangleDraw()
	{
		//���_�o�b�t�@������������B
		InitVertexBuffer();

		//�V�F�[�_�[�����[�h����B
		LoadShader();
	}
	//�f�X�g���N�^�B
	~TriangleDraw()
	{
		//���_�o�b�t�@������B
		if (m_vertexBuffer != nullptr) {
			m_vertexBuffer->Release();
		}
	}
	//�V�F�[�_�[�����[�h����B
	void LoadShader()
	{
		//���_�V�F�[�_�[�����[�h�B
		//�������̓��[�h����V�F�[�_�[�\�[�X�t�@�C���̃t�@�C���p�X�B
		//�������̓G���g���[�|�C���g�ƂȂ�֐��̖��O�B 
		//��O�����̓��[�h�����V�F�[�_�[�̃^�C�v�B�����͒��_�V�F�[�_�[�B
		m_vsShader.Load(
			"Assets/shader/Tutorial.fx",
			"VS",						
			Shader::EnType::VS			
		);
		//�s�N�Z���V�F�[�_�[�����[�h�B
		m_psShader.Load(
			"Assets/shader/Tutorial.fx",
			"PS",
			Shader::EnType::PS
		);
	}
	//���_�o�b�t�@�̏������B
	void InitVertexBuffer()
	{
		//���_�o�b�t�@�̃\�[�X�f�[�^�B
		SSimpleVertex vertices[] =
		{
			CVector4(0.0f, 0.5f, 0.0f, 1.0f),
			CVector4(0.5f, -0.5f, 0.0f, 1.0f),
			CVector4(-0.5f, -0.5f, 0.0f, 1.0f),
		};

		//���_�o�b�t�@�̒�`�f�[�^���쐬����B
		D3D11_BUFFER_DESC desc = {0};	//0�N���A�B
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(vertices);			//�o�b�t�@�̑傫���B
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//�ǂ̃o�b�t�@�Ƀo�C���h���邩���w�肷��B
		desc.CPUAccessFlags = 0;					//CPU����A�N�Z�X���邩���߂�B0�Ȃ炵�Ȃ��B

		//���_�o�b�t�@�̃\�[�X�f�[�^���w�肷��B
		D3D11_SUBRESOURCE_DATA InitData = {0};	//0�ŏ������B
		InitData.pSysMem = vertices;

		//���_�o�b�t�@��VRAM��ɍ쐬����B
		g_graphicsEngine->GetD3DDevice()->CreateBuffer(&desc, &InitData, &m_vertexBuffer);
	}
	//�`�悾���Ă΂�I
	void Draw()
	{
		//�f�o�C�X�R���e�L�X�g���擾�B
		auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
		//�f�o�C�X�R���e�L�X�g�ɕ`��R�}���h��ςށB
		//���_�o�b�t�@��ݒ�B
		unsigned int stride = sizeof(SSimpleVertex);	//���_�o�b�t�@�̃X�g���C�h
		unsigned int offset = 0;						//���_�o�b�t�@�̃I�t�Z�b�g�B
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset );
		//�v���~�e�B�u�̃g�|���W�[��ݒ�B
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//���_�V�F�[�_�[��ݒ�B
		deviceContext->VSSetShader(
			(ID3D11VertexShader*)m_vsShader.GetBody(),	//���_�V�F�[�_�[�B
			NULL,										//NULL�ł����B
			0											//0�ł����B
		);
		//�s�N�Z���V�F�[�_�[��ݒ�B
		deviceContext->PSSetShader(
			(ID3D11PixelShader*)m_psShader.GetBody(),	//�s�N�Z���V�F�[�_�[�B
			NULL,										//NULL�ł����B
			0											//0�ł����B
		);
		//���_���C�A�E�g��ݒ�B
		deviceContext->IASetInputLayout(m_vsShader.GetInputLayout());

		//�����܂Őݒ肵�����e�Ńh���[
		deviceContext->Draw(
			3,	//���_���B
			0	//�`��J�n�̒��_�ԍ��B���O�B
		);
	}
};

///////////////////////////////////////////////////////////////////
// �E�B���h�E�v���O�����̃��C���֐��B
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//�Q�[���̏������B
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");

	//TriangleDraw�N���X�̕ϐ���p�ӂ���B
	TriangleDraw triangleDraw;

	//�Q�[�����[�v�B
	while (DispatchWindowMessage() == true)
	{
		//�`��J�n�B
		g_graphicsEngine->BegineRender();
		
		//�O�p�`��`��B
		triangleDraw.Draw();

		//�`��I���B
		g_graphicsEngine->EndRender();
	}
}