#include "stdafx.h"
#include "system/system.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"

using namespace std;


/*!
*@brief	���f���G�t�F�N�g�B
*@details
* DirectX::Model�̕`�揈���Ŏg�p�����V�F�[�_�[�������ւ��邽�߂̃N���X
*/
class C3DModelEffect : public DirectX::IEffect {
private:
	Shader m_vsShader;
	Shader m_psShader;
public:
	//�R���X�g���N�^�B
	C3DModelEffect()
	{
		//���_�V�F�[�_�[�����[�h�B
		m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
		m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	}
	//���̊֐���DirectX::Model::Draw�����̃h���[�R�[���̒��O�ɌĂ΂��B
	//�Ȃ̂ŁA���̊֐��̂Ȃ��ŁA�V�F�[�_�[�̐ݒ��A�e�N�X�`���̐ݒ�Ȃǂ��s���Ƃ悢�B
	void __cdecl Apply(ID3D11DeviceContext* deviceContext) override
	{
		//�V�F�[�_�[��K�p����B
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
	}
	//���̊֐���DirectX::Model�̏�������������Ă΂��B
	//���_�V�F�[�_�[�̃o�C�g�R�[�h�ƃR�[�h�̒�����ݒ肷��K�v������B
	void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
	{
		*pShaderByteCode = m_vsShader.GetByteCode();
		*pByteCodeLength = m_vsShader.GetByteCodeSize();
	}
};
/*!
*@brief
*  �G�t�F�N�g�t�@�N�g���B
*@detail
* �G�t�F�N�g�̃t�@�N�g���B�}�e���A���̏��ɉ����č쐬����G�t�F�N�g��ύX����N���X�B
* 
*/
class C3DModelEffectFactory : public DirectX::EffectFactory {
public:
	C3DModelEffectFactory(ID3D11Device* device) :
		EffectFactory(device) {}
	//�G�t�F�N�g���쐬�B
	std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(const EffectInfo& info, ID3D11DeviceContext* deviceContext)override
	{
		shared_ptr<C3DModelEffect> effect = make_shared< C3DModelEffect>();

		return effect;
	}
};
/*!
*@brief
*  3D���f���`��N���X�B
*/
class C3DModelDraw {
	//�萔�o�b�t�@�B
	struct SVSConstantBuffer {
		CMatrix mWorld;
		CMatrix mView;
		CMatrix mProj;
	};

	unique_ptr<DirectX::Model>		m_modelDx;				//DirectXTK���񋟂��郂�f���B
	ID3D11Buffer*		m_cb = nullptr;						//!<�萔�o�b�t�@�B
	ID3D11Buffer*		m_psCb = nullptr;					//!<�s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�B
	ID3D11SamplerState* m_samplerState = nullptr;			//!<�T���v���[�X�e�[�g�B
	ID3D11ShaderResourceView* m_albedoTextureSRV = nullptr;	//!<�A���x�h�e�N�X�`����SRV
	ID3D11ShaderResourceView* m_grayTextureSRV = nullptr;	//!<�O���[�e�N�X�`����SRV
public:
	//�f�X�g���N�^�B
	~C3DModelDraw()
	{
		//�萔�o�b�t�@������B
		if (m_cb != nullptr) {
			m_cb->Release();
		}
		//�T���v���X�e�[�g������B
		if (m_samplerState != nullptr) {
			m_samplerState->Release();
		}
		//�A���x�h�e�N�X�`��������B
		if (m_albedoTextureSRV != nullptr) {
			m_albedoTextureSRV->Release();
		}
		//�O���[�e�N�X�`��������B
		if (m_grayTextureSRV != nullptr) {
			m_grayTextureSRV->Release();
		}
	}
	void Init()
	{
		//�萔�o�b�t�@���������B
		InitConstantBuffer();
		//�T���v���X�e�[�g���������B
		InitSamplerState();
		//�A���x�h�e�N�X�`�����������B
		InitAlbedoTexture();
		//�O���C�e�N�X�`�����������B
		InitGrayTexture();

		//�G�t�F�N�g�t�@�N�g�����쐬
		C3DModelEffectFactory effectFactory(g_graphicsEngine->GetD3DDevice());
		//�e�N�X�`��������t�H���_��ݒ肷��B
		effectFactory.SetDirectory(L"Assets/modelData");
		//cmo�t�@�C������DirectX::Model���쐬����B
		m_modelDx = DirectX::Model::CreateFromCMO(	//CMO�t�@�C�����烂�f�����쐬����֐��́ACreateFromCMO�����s����B
			g_graphicsEngine->GetD3DDevice(),			//��������D3D�f�o�C�X�B
			L"Assets/modelData/unityChan.cmo",			//�������͓ǂݍ���CMO�t�@�C���̃t�@�C���p�X�B
			effectFactory,								//��O�����̓G�t�F�N�g�t�@�N�g���B
			false,										//��l������Cull���[�h�B���͋C�ɂ��Ȃ��Ă悢�B
			false
		);
	}
	//�A���x�h�e�N�X�`�����������B
	void InitAlbedoTexture()
	{
		//�t�@�C�������g���āA�e�N�X�`�������[�h���āAShaderResrouceView���쐬����B
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_all2.dds", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			false, nullptr, &m_grayTextureSRV);
	}
	//�O���C�e�N�X�`�����������B
	void InitGrayTexture()
	{
		//�t�@�C�������g���āA�e�N�X�`�������[�h���āAShaderResrouceView���쐬����B
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_all2_gray.dds", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			false, nullptr, &m_albedoTextureSRV);
	}
	//�T���v���X�e�[�g�̏������B
	void InitSamplerState()
	{
		//�T���v���X�e�[�g�̓e�N�X�`�����T���v�����O������@���w�肷��B
		CD3D11_DEFAULT defDesc;
		CD3D11_SAMPLER_DESC desc(defDesc);
		//
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//U�����̓��b�v
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//V�����̓��b�v
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//W�����̓��b�v(W������3�����e�N�X�`���̎��Ɏg�p�����B)
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//�e�N�X�`���t�B���^�̓o�C���j�A�t�B���^
		//�T���v���X�e�[�g���쐬�B
		g_graphicsEngine->GetD3DDevice()->CreateSamplerState(&desc, &m_samplerState);
	}
	//�萔�o�b�t�@�̏������B
	void InitConstantBuffer()
	{
		//�쐬����o�b�t�@�̃T�C�Y��sizeof���Z�q�ŋ��߂�B
		int bufferSize = sizeof(SVSConstantBuffer);
		//�ǂ�ȃo�b�t�@���쐬����̂�������bufferDesc�ɐݒ肷��B
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));				//�O�ŃN���A�B
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;						//�o�b�t�@�őz�肳��Ă���A�ǂݍ��݂���я������ݕ��@�B
		bufferDesc.ByteWidth = (((bufferSize - 1) / 16) + 1) * 16;	//�o�b�t�@��16�o�C�g�A���C�����g�ɂȂ��Ă���K�v������B
																	//�A���C�����g���ā��o�b�t�@�̃T�C�Y��16�̔{���Ƃ������Ƃł��B
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//�o�b�t�@���ǂ̂悤�ȃp�C�v���C���Ƀo�C���h���邩���w�肷��B
																	//�萔�o�b�t�@�Ƀo�C���h����̂ŁAD3D11_BIND_CONSTANT_BUFFER���w�肷��B
		bufferDesc.CPUAccessFlags = 0;								//CPU �A�N�Z�X�̃t���O�ł��B
																	//CPU�A�N�Z�X���s�v�ȏꍇ��0�B
		//�쐬�B
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		d3dDevice->CreateBuffer(&bufferDesc, NULL, &m_cb);

		//�s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@���쐬����B
		bufferDesc.ByteWidth = 16;		//4�o�C�g�����]�����Ȃ��̂ŁA�Œ���̃o�b�t�@�T�C�Y��16�ł������B
		d3dDevice->CreateBuffer(&bufferDesc, NULL, &m_psCb);
	}
	void Update()
	{
	}
	void Draw()
	{
		auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
		DirectX::CommonStates state(g_graphicsEngine->GetD3DDevice());

		//�O���[�X�P�[�����[�g�����߂邽�߂̎�B
		static float grayScaleRateSeed = 0.0f;
		grayScaleRateSeed += 0.05f;
		float grayScaleRate = sin(grayScaleRateSeed);	//sin�l��-1�`1�͈̔͂�Ԃ��Ă���B
		grayScaleRate = grayScaleRate * 0.5f + 0.5f;	//grayScaleRate�̒l��0.0�`1.0�͈̔͂ɕύX����B
		//�萔�o�b�t�@���X�V�B
		deviceContext->UpdateSubresource(m_psCb, 0, nullptr, &grayScaleRate, 0, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &m_psCb);
		//�P�ʃ}�g���N�X�B
		CMatrix mWorld = CMatrix::Identity();
		//�萔�o�b�t�@���X�V�B
		SVSConstantBuffer vsCb;
		vsCb.mWorld = mWorld;
		vsCb.mProj = g_camera3D.GetProjectionMatrix();
		vsCb.mView = g_camera3D.GetViewMatrix();
		deviceContext->UpdateSubresource(m_cb, 0, nullptr, &vsCb, 0, 0);
		//�萔�o�b�t�@��GPU�ɓ]���B
		deviceContext->VSSetConstantBuffers(0, 1, &m_cb);
		//�T���v���X�e�[�g��ݒ肷��B
		deviceContext->PSSetSamplers(0, 1, &m_samplerState);
		//�A���x�h�e�N�X�`����ݒ肷��B
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTextureSRV);
		deviceContext->PSSetShaderResources(1, 1, &m_grayTextureSRV);
		m_modelDx->Draw(
			deviceContext,
			state,
			mWorld,
			g_camera3D.GetViewMatrix(),
			g_camera3D.GetProjectionMatrix()
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

	//�J������ݒ�B
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();

	C3DModelDraw modelDraw;
	modelDraw.Init();
	//�Q�[�����[�v�B
	while (DispatchWindowMessage() == true)
	{
		//�`��J�n�B
		g_graphicsEngine->BegineRender();
		
		for (auto& pad : g_pad) {
			pad.Update();
		}
		modelDraw.Draw();

		//�`��I���B
		g_graphicsEngine->EndRender();
	}
}