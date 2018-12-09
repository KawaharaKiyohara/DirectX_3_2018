#pragma once
#include "RenderTarget.h"
#include "graphics/Shader.h"

class PostEffect;

/// <summary>
/// ブルーム
/// </summary>
class Bloom
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Bloom();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Bloom();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(PostEffect& postEffect);
private:
	/// <summary>
	/// アルファブレンディングのステートを初期化する。
	/// </summary>
	void InitAlphaBlendState();
	/// <summary>
	/// 定数バッファの初期化。
	/// </summary>
	void InitConstantBuffer();
	/// <summary>
	/// シェーダーを初期化。
	/// </summary>
	void InitShader();
	/// <summary>
	/// レンダリングターゲットの初期化。
	/// </summary>
	void InitRenderTarget();
	/// <summary>
	/// サンプラステートの初期化。
	/// </summary>
	void InitSamplerState();
private:
	static const int NUM_WEIGHTS = 8;	//カウスフィルタの重みの数。
	/*!
	* @brief	ブラー用のパラメータバッファ。
	*@details
	* これを変更したら、Assets/shader/bloom.fxのCBBlurの中身も変更するように。
	*/
	struct SBlurParam {
		CVector4 offset;
		float weights[NUM_WEIGHTS];
	};
	ID3D11SamplerState* m_samplerState = nullptr;		//サンプラステート。
	ID3D11BlendState* m_disableBlendState = nullptr;	//アルファブレンディングを無効にするブレンディングステート。
	ID3D11BlendState* m_finalBlendState = nullptr;		//最終合成用のブレンディングステート。
	RenderTarget m_luminanceRT;			//輝度を抽出するレンダリングターゲット
	RenderTarget m_downSamplingRT[2];	//ブラーをかけるためのダウンサンプリング用のレンダリングターゲット。
	Shader m_vs;						//何もしない頂点シェーダー。
	Shader m_psLuminance;				//輝度抽出用のピクセルシェーダー。
	Shader m_vsXBlur;					//Xブラー用の頂点シェーダー。
	Shader m_vsYBlur;					//Yブラー用の頂点シェーダー。
	Shader m_psBlur;					//ブラー用のピクセルシェーダー。
	Shader m_psFinal;					//最終合成用のピクセルシェーダー。

	SBlurParam m_blurParam;				//ブラー用のパラメータ。
	float m_blurDispersion = 50.0f;		//ブラーの分散具合。大きくするほど強く分散する。
	ID3D11Buffer* m_blurParamCB = nullptr;	//ブラーパラメータ用の定数バッファ。
};

