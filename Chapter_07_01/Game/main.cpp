#include "stdafx.h"
#include "system/system.h"
#include "graphics/Shader.h"
#include "graphics/Camera.h"

using namespace std;


/*!
*@brief	ディレクションライト。
*/
struct SDirectionLight {
	CVector4 direction;		//ライトの方向。
	CVector4 color;			//ライトのカラー。
};

/*!解説①　ライト構造体を用意した。
*@brief	ライト構造体。
*/
struct SLight {
	SDirectionLight		directionLight;		//ディレクションライト
	CVector3			eyePos;				//視点の座標。
	float				specPow;			//鏡面反射の絞り。
};
/*!解説②　16の倍数に切り上げる処理を関数化。
*@brief	引数で渡された整数値を16の倍数に切り上げます。
*@param[in]		n		整数の値。
*@return		nを16の倍数に切り上げた値。
*/
int Raundup16(int n)
{
	return (((n - 1) / 16) + 1) * 16;
}
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
	ID3D11Buffer*		m_lightCb = nullptr;				//!<ライト用の定数バッファ。
	SLight				m_light;							//!<ライト構造体。解説③　ライト用の構造体を使うように変更。
	ID3D11SamplerState* m_samplerState = nullptr;			//!<サンプラーステート。
	ID3D11ShaderResourceView* m_albedoTextureSRV = nullptr;	//!<アルベドテクスチャのSRV
public:
	//デストラクタ。
	~C3DModelDraw()
	{
		//定数バッファを解放。
		if (m_cb != nullptr) {
			m_cb->Release();
		}
		//ライト用の定数バッファの解放。
		if (m_lightCb != nullptr) {
			m_lightCb->Release();
		}
		//サンプラステートを解放。
		if (m_samplerState != nullptr) {
			m_samplerState->Release();
		}
		//アルベドテクスチャを解放。
		if (m_albedoTextureSRV != nullptr) {
			m_albedoTextureSRV->Release();
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
		//ディレクションライトの初期化。
		InitDirectionLight();

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
	//ディレクションライトの初期化。
	void InitDirectionLight()
	{
		m_light.directionLight.direction = { 1.0f, 0.0f,0.0f, 0.0f };
		m_light.directionLight.color = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_light.specPow = 10.0f;
	}
	//アルベドテクスチャを初期化。
	void InitAlbedoTexture()
	{
		//ファイル名を使って、テクスチャをロードして、ShaderResrouceViewを作成する。
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			g_graphicsEngine->GetD3DDevice(), L"Assets/modelData/utc_all2.dds", 0,
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
		bufferDesc.ByteWidth = Raundup16(bufferSize);	//バッファは16バイトアライメントになっている必要がある。
																	//アライメントって→バッファのサイズが16の倍数ということです。
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;			//バッファをどのようなパイプラインにバインドするかを指定する。
																	//定数バッファにバインドするので、D3D11_BIND_CONSTANT_BUFFERを指定する。
		bufferDesc.CPUAccessFlags = 0;								//CPU アクセスのフラグです。
																	//CPUアクセスが不要な場合は0。
		//作成。
		g_graphicsEngine->GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_cb);

		//続いて、ライト用の定数バッファを作成。
		//作成するバッファのサイズを変更するだけ。
		bufferDesc.ByteWidth = Raundup16(sizeof(SLight));	
		g_graphicsEngine->GetD3DDevice()->CreateBuffer(&bufferDesc, NULL, &m_lightCb);
	}
	void Update()
	{
		//ライトを回す。
		CQuaternion qRot;
		qRot.SetRotationDeg(CVector3::AxisY(), g_pad[0].GetLStickXF());
		qRot.Multiply(m_light.directionLight.direction);
		if (g_pad[0].IsPress(enButtonLeft)) {
			m_light.specPow = max(0.0f, m_light.specPow - 0.5f);
		}
		if (g_pad[0].IsPress(enButtonRight)) {
			m_light.specPow = min(100.0f, m_light.specPow + 0.5f);
		}
	}
	void Draw()
	{
		auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
		DirectX::CommonStates state(g_graphicsEngine->GetD3DDevice());

		//単位マトリクス。
		CMatrix mWorld = CMatrix::Identity();
		//定数バッファを更新。
		SVSConstantBuffer vsCb;
		vsCb.mWorld = mWorld;
		vsCb.mProj = g_camera3D.GetProjectionMatrix();
		vsCb.mView = g_camera3D.GetViewMatrix();
		deviceContext->UpdateSubresource(m_cb, 0, nullptr, &vsCb, 0, 0);
		//解説④　視点を更新
		//視点を設定。
		m_light.eyePos = g_camera3D.GetPosition();
		//ライト用の定数バッファを更新。
		deviceContext->UpdateSubresource(m_lightCb, 0, nullptr, &m_light, 0, 0);

		//定数バッファをシェーダースロットに設定。
		deviceContext->VSSetConstantBuffers(0, 1, &m_cb);
		deviceContext->PSSetConstantBuffers(0, 1, &m_lightCb);
		//サンプラステートを設定する。
		deviceContext->PSSetSamplers(0, 1, &m_samplerState);
		//アルベドテクスチャを設定する。
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTextureSRV);
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
		modelDraw.Update();
		modelDraw.Draw();

		//描画終了。
		g_graphicsEngine->EndRender();
	}
}