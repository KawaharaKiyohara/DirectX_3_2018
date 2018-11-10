#pragma once
#include "graphics/Shader.h"



const int NUM_DIRECTION_LIG = 4;
/*!
*@brief	ディレクションライト。
*/
struct SDirectionLight {
	CVector4 direction[NUM_DIRECTION_LIG];		//ライトの方向。
	CVector4 color[NUM_DIRECTION_LIG];			//ライトのカラー。
};

/*!
*@brief	モデルエフェクト。
*@details
* DirectX::Modelの描画処理で使用されるシェーダーを差し替えるためのクラス
*/
class C3DModelEffect : public DirectX::IEffect {
private:
	Shader m_vsShader;
	Shader m_psShader;
	ID3D11ShaderResourceView* m_albedoTexture = nullptr;
public:
	//コンストラクタ。
	C3DModelEffect()
	{
		//頂点シェーダーをロード。
		m_vsShader.Load("Assets/shader/model.fx", "VSMain", Shader::EnType::VS);
		m_psShader.Load("Assets/shader/model.fx", "PSMain", Shader::EnType::PS);
	}
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~C3DModelEffect()
	{
		if (m_albedoTexture != nullptr) {
			m_albedoTexture->Release();
		}
	}
	//この関数はDirectX::Model::Draw内部のドローコールの直前に呼ばれる。
	//なので、この関数のなかで、シェーダーの設定や、テクスチャの設定などを行うとよい。
	void __cdecl Apply(ID3D11DeviceContext* deviceContext) override
	{
		//シェーダーを適用する。
		deviceContext->VSSetShader((ID3D11VertexShader*)m_vsShader.GetBody(), NULL, 0);
		deviceContext->PSSetShader((ID3D11PixelShader*)m_psShader.GetBody(), NULL, 0);
		deviceContext->PSSetShaderResources(0, 1, &m_albedoTexture);
	}
	//この関数はDirectX::Modelの初期化処理から呼ばれる。
	//頂点シェーダーのバイトコードとコードの長さを設定する必要がある。
	void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
	{
		*pShaderByteCode = m_vsShader.GetByteCode();
		*pByteCodeLength = m_vsShader.GetByteCodeSize();
	}
	void SetAlbedoTexture(ID3D11ShaderResourceView* texSRV)
	{
		m_albedoTexture = texSRV;
		//参照カウンタを上げる。
		m_albedoTexture->AddRef();
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
		std::shared_ptr<C3DModelEffect> effect = std::make_shared< C3DModelEffect>();

		
		if (info.diffuseTexture && *info.diffuseTexture)
		{
			//アルベドテクスチャが貼られている。
			ID3D11ShaderResourceView* texSRV;
			DirectX::EffectFactory::CreateTexture(info.diffuseTexture, deviceContext, &texSRV);
			effect->SetAlbedoTexture(texSRV);
			texSRV->Release();
		}

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

	std::unique_ptr<DirectX::Model>		m_modelDx;				//DirectXTKが提供するモデル。
	ID3D11Buffer*						m_cb = nullptr;			//!<定数バッファ。
	ID3D11Buffer*						m_lightCb = nullptr;	//!<ライト用の定数バッファ。
	SDirectionLight						m_dirLight;				//!<ディレクションライト。
	ID3D11SamplerState* m_samplerState = nullptr;			//!<サンプラーステート。
	CMatrix m_worldMatrix = CMatrix::Identity();			//!<ワールド行列。
public:
	//デストラクタ。
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~C3DModelDraw();

	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="filePath">ロードするcmoファイルのファイルパス。</param>
	void Init(const wchar_t* filePath);

	/// <summary>
	/// 更新。
	/// </summary>
	/// <param name="trans">平行移動</param>
	/// <param name="qRot">回転</param>
	/// <param name="scale">拡大率</param>
	void Update(CVector3 trans);
	/// <summary>
	/// 描画。
	/// </summary>
	void Draw();
	
private:
	//ディレクションライトの初期化。
	/// <summary>
	/// ディレクションライトの初期化。
	/// </summary>
	void InitDirectionLight();

	/// <summary>
	/// サンプラステートの初期化。
	/// </summary>
	void InitSamplerState();

	/// <summary>
	/// 定数バッファの初期化。
	/// </summary>
	void InitConstantBuffer();
	
};
