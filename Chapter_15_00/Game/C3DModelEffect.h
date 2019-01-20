#pragma once
#include "graphics/Shader.h"
#include <array>

/*!
*@brief	モデルエフェクト。
*@details
* DirectX::Modelの描画処理で使用されるシェーダーを差し替えるためのクラス
*/
class C3DModelEffect : public DirectX::IEffect {
private:
	Shader m_vsShader;
	Shader m_psShader;
	Shader m_vsShadowMap;			//シャドウマップ生成用の頂点シェーダー。
	Shader m_psShadowMap;		//シャドウマップ生成用のピクセルシェーダー。
	ID3D11ShaderResourceView* m_albedoTexture = nullptr;
	//C++11で追加された配列。従来のCの配列のパフォーマンスを維持しつつ
	//配列の要素数の取得や、境界チェックまでできる優れもの。
	//普通の配列なんて使わなくてよくね？ってなります。
	std::array<ID3D11ShaderResourceView*, 4> m_albedoTextureStack = { nullptr };
	int m_albedoTextureStackPos = 0;
	EnRenderMode m_renderMode = enRenderMode_Invalid;	//レンダリングモード。
public:
	//コンストラクタ。
	C3DModelEffect();

	/// <summary>
	/// デストラクタ。
	/// </summary>
	~C3DModelEffect();

	//この関数はDirectX::Model::Draw内部のドローコールの直前に呼ばれる。
	//なので、この関数のなかで、シェーダーの設定や、テクスチャの設定などを行うとよい。
	void __cdecl Apply(ID3D11DeviceContext* deviceContext) override;

	//この関数はDirectX::Modelの初期化処理から呼ばれる。
	//頂点シェーダーのバイトコードとコードの長さを設定する必要がある。
	void __cdecl GetVertexShaderBytecode(void const** pShaderByteCode, size_t* pByteCodeLength) override
	{
		*pShaderByteCode = m_vsShader.GetByteCode();
		*pByteCodeLength = m_vsShader.GetByteCodeSize();
	}
	void SetAlbedoTexture(ID3D11ShaderResourceView* texSRV)
	{
		if (m_albedoTexture != nullptr) {
			//参照カウンタを下げる。
			m_albedoTexture->Release();
		}
		m_albedoTexture = texSRV;
		//参照カウンタを上げる。
		m_albedoTexture->AddRef();
	}
	/// <summary>
	/// 現在のアルベドテクスチャをスタックに退避させる。
	/// </summary>
	void PushAlbedoTexture()
	{
		m_albedoTextureStack[m_albedoTextureStackPos] = m_albedoTexture;
		m_albedoTexture = nullptr;
		m_albedoTextureStackPos++;
	}	
	/// <summary>
	/// スタックからアルベドテクスチャをポップする。
	/// </summary>
	void PopAlbedoTexture()
	{
		m_albedoTextureStackPos--;
		SetAlbedoTexture(m_albedoTextureStack[m_albedoTextureStackPos]);
	}
	
	void SetRenderMode(EnRenderMode renderMode)
	{
		m_renderMode = renderMode;
	}
private:
	
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
