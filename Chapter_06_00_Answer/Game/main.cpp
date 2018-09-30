#include "stdafx.h"
#include "system/system.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"

using namespace std;


/*!
*@brief	モデルエフェクト。
*@details
* DirectX::Modelの描画処理で使用されるシェーダーを差し替えるためのクラス
*/
class C3DModelEffect : public DirectX::IEffect {
private:
	Shader m_vsShader;
	Shader m_psShader;
public:
	//コンストラクタ。
	C3DModelEffect()
	{
		//頂点シェーダーをロード。
		m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
		m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	}
	//この関数はDirectX::Model::Draw内部のドローコールの直前に呼ばれる。
	//なので、この関数のなかで、シェーダーの設定や、テクスチャの設定などを行うとよい。
	void __cdecl Apply(ID3D11DeviceContext* deviceContext) override
	{
		//シェーダーを適用する。
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
	}
	//この関数はDirectX::Modelの初期化処理から呼ばれる。
	//頂点シェーダーのバイトコードとコードの長さを設定する必要がある。
	void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
	{
		*pShaderByteCode = m_vsShader.GetByteCode();
		*pByteCodeLength = m_vsShader.GetByteCodeSize();
	}
};
/*!
*@brief
*  エフェクトファクトリ。
*@detail
* エフェクトのファクトリ。マテリアルの情報に応じて作成するエフェクトを変更するクラス。
* 
*/
class C3DModelEffectFactory : public DirectX::EffectFactory {
public:
	C3DModelEffectFactory(ID3D11Device* device) :
		EffectFactory(device) {}
	//エフェクトを作成。
	std::shared_ptr<DirectX::IEffect> __cdecl CreateEffect(const EffectInfo& info, ID3D11DeviceContext* deviceContext)override
	{
		shared_ptr<C3DModelEffect> effect = make_shared< C3DModelEffect>();

		return effect;
	}
};
/*!
*@brief
*  3Dモデル描画クラス。
*/
class C3DModelDraw {
	//定数バッファ。
	struct SVSConstantBuffer {
		CMatrix mWorld;
		CMatrix mView;
		CMatrix mProj;
	};

	unique_ptr<DirectX::Model>		m_modelDx;				//DirectXTKが提供するモデル。
	ID3D11Buffer*		m_cb = nullptr;						//!<定数バッファ。
	ID3D11Buffer*		m_psCb = nullptr;					//!<ピクセルシェーダー用の定数バッファ。
	ID3D11SamplerState* m_samplerState = nullptr;			//!<サンプラーステート。
	ID3D11ShaderResourceView* m_albedoTextureSRV = nullptr;	//!<アルベドテクスチャのSRV
	ID3D11ShaderResourceView* m_grayTextureSRV = nullptr;	//!<グレーテクスチャのSRV
public:
	//デストラクタ。
	~C3DModelDraw()
	{
		//定数バッファを解放。
		if (m_cb != nullptr) {
			m_cb->Release();
		}
		//サンプラステートを解放。
		if (m_samplerState != nullptr) {
			m_samplerState->Release();
		}
		//アルベドテクスチャを解放。
		if (m_albedoTextureSRV != nullptr) {
			m_albedoTextureSRV->Release();
		}
		//グレーテクスチャを解放。
		if (m_grayTextureSRV != nullptr) {
			m_grayTextureSRV->Release();
		}
	}
	void Init()
	{
		//定数バッファを初期化。
		InitConstantBuffer();
		//サンプラステートを初期化。
		InitSamplerState();
		//アルベドテクスチャを初期化。
		InitAlbedoTexture();
		//グレイテクスチャを初期化。
		InitGrayTexture();

		//エフェクトファクトリを作成
		C3DModelEffectFactory effectFactory(g_graphicsEngine->GetD3DDevice());
		//テクスチャがあるフォルダを設定する。
		effectFactory.SetDirectory(L"Assets/modelData");
		//cmoファイルからDirectX::Modelを作成する。
		m_modelDx = DirectX::Model::CreateFromCMO(	//CMOファイルからモデルを作成する関数の、CreateFromCMOを実行する。
			g_graphicsEngine->GetD3DDevice(),			//第一引数はD3Dデバイス。
			L"Assets/modelData/unityChan.cmo",			//第二引数は読み込むCMOファイルのファイルパス。
			effectFactory,								//第三引数はエフェクトファクトリ。
			false,										//第四引数はCullモード。今は気にしなくてよい。
			false
		);
	}
	//アルベドテクスチャを初期化。
	void InitAlbedoTexture()
	{
		//ファイル名を使って、テクスチャをロードして、ShaderResrouceViewを作成する。
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_all2.dds", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			false, nullptr, &m_grayTextureSRV);
	}
	//グレイテクスチャを初期化。
	void InitGrayTexture()
	{
		//ファイル名を使って、テクスチャをロードして、ShaderResrouceViewを作成する。
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_all2_gray.dds", 0,
			D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
			false, nullptr, &m_albedoTextureSRV);
	}
	//サンプラステートの初期化。
	void InitSamplerState()
	{
		//サンプラステートはテクスチャをサンプリングする方法を指定する。
		CD3D11_DEFAULT defDesc;
		CD3D11_SAMPLER_DESC desc(defDesc);
		//
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;		//U方向はラップ
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;		//V方向はラップ
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;		//W方向はラップ(W方向は3次元テクスチャの時に使用される。)
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;	//テクスチャフィルタはバイリニアフィルタ
		//サンプラステートを作成。
		g_graphicsEngine->GetD3DDevice()->CreateSamplerState(&desc, &m_samplerState);
	}
	//定数バッファの初期化。
	void InitConstantBuffer()
	{
		//作成するバッファのサイズをsizeof演算子で求める。
		int bufferSize = sizeof(SVSConstantBuffer);
		//どんなバッファを作成するのかをせてbufferDescに設定する。
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));				//０でクリア。
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;						//バッファで想定されている、読み込みおよび書き込み方法。
		bufferDesc.ByteWidth = (((bufferSize - 1) / 16) + 1) * 16;	//バッファは16バイトアライメントになっている必要がある。
																	//アライメントって→バッファのサイズが16の倍数ということです。
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//バッファをどのようなパイプラインにバインドするかを指定する。
																	//定数バッファにバインドするので、D3D11_BIND_CONSTANT_BUFFERを指定する。
		bufferDesc.CPUAccessFlags = 0;								//CPU アクセスのフラグです。
																	//CPUアクセスが不要な場合は0。
		//作成。
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		d3dDevice->CreateBuffer(&bufferDesc, NULL, &m_cb);

		//ピクセルシェーダー用の定数バッファを作成する。
		bufferDesc.ByteWidth = 16;		//4バイトしか転送しないので、最低限のバッファサイズの16でええやろ。
		d3dDevice->CreateBuffer(&bufferDesc, NULL, &m_psCb);
	}
	void Update()
	{
	}
	void Draw()
	{
		auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
		DirectX::CommonStates state(g_graphicsEngine->GetD3DDevice());

		//グレースケールレートを決めるための種。
		static float grayScaleRateSeed = 0.0f;
		grayScaleRateSeed += 0.05f;
		float grayScaleRate = sin(grayScaleRateSeed);	//sin値は-1～1の範囲を返してくる。
		grayScaleRate = grayScaleRate * 0.5f + 0.5f;	//grayScaleRateの値を0.0～1.0の範囲に変更する。
		//定数バッファを更新。
		deviceContext->UpdateSubresource(m_psCb, 0, nullptr, &grayScaleRate, 0, 0);
		deviceContext->PSSetConstantBuffers(0, 1, &m_psCb);
		//単位マトリクス。
		CMatrix mWorld = CMatrix::Identity();
		//定数バッファを更新。
		SVSConstantBuffer vsCb;
		vsCb.mWorld = mWorld;
		vsCb.mProj = g_camera3D.GetProjectionMatrix();
		vsCb.mView = g_camera3D.GetViewMatrix();
		deviceContext->UpdateSubresource(m_cb, 0, nullptr, &vsCb, 0, 0);
		//定数バッファをGPUに転送。
		deviceContext->VSSetConstantBuffers(0, 1, &m_cb);
		//サンプラステートを設定する。
		deviceContext->PSSetSamplers(0, 1, &m_samplerState);
		//アルベドテクスチャを設定する。
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
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");

	//カメラを設定。
	g_camera3D.SetPosition({ 0.0f, 200.0f, 500.0f });
	g_camera3D.SetTarget({ 0.0f, 200.0f, 0.0f });
	g_camera3D.Update();

	C3DModelDraw modelDraw;
	modelDraw.Init();
	//ゲームループ。
	while (DispatchWindowMessage() == true)
	{
		//描画開始。
		g_graphicsEngine->BegineRender();
		
		for (auto& pad : g_pad) {
			pad.Update();
		}
		modelDraw.Draw();

		//描画終了。
		g_graphicsEngine->EndRender();
	}
}