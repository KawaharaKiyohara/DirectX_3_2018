#pragma once

#include "C3DModelEffect.h"


const int NUM_DIRECTION_LIG = 4;
/*!
*@brief	ディレクションライト。
*/
struct SDirectionLight {
	CVector4 direction[NUM_DIRECTION_LIG];		//ライトの方向。
	CVector4 color[NUM_DIRECTION_LIG];			//ライトのカラー。
};


/*!
*@brief
*  3Dモデル描画クラス。
*/
class C3DModelDraw {
	/// <summary>
	/// モデルエフェクト用の構造体。
	/// </summary>
	/// <remarks>
	/// これを変更したら、Asserts/shader/model.fxの
	/// VSCbも変更する必要があります。
	/// この構造体は16byteアライメントになっている必要があります。
	/// </remarks>
	struct SModelFxConstantBuffer {
		CMatrix mWorld;			//ワールド行列。
		CMatrix mView;			//ビュー行列。
		CMatrix mProj;			//プロジェクション行列。
		CMatrix mLightView;		//todo ライトビュー行列。
		CMatrix mLightProj;		//todo ライトプロジェクション行列。
		int isShadowReciever;	//todo シャドウレシーバーのフラグ。
	};
	std::unique_ptr<DirectX::Model>		m_modelDx;				//DirectXTKが提供するモデル。
	ID3D11Buffer*						m_cb = nullptr;			//!<定数バッファ。
	ID3D11Buffer*						m_lightCb = nullptr;	//!<ライト用の定数バッファ。
	SDirectionLight						m_dirLight;				//!<ディレクションライト。
	ID3D11SamplerState* m_samplerState = nullptr;			//!<サンプラーステート。
	CMatrix m_worldMatrix = CMatrix::Identity();			//!<ワールド行列。
	bool m_isShadowReciever = false;						//シャドウレシーバーのフラグ。
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
	/// <param name="renderStep">
	///	描画モード。GraphicsEngine.hのEnRenderModeを参照。
	/// </param>
	/// <param name="viewMatrix">ビュー行列</param>
	/// <param name="projMatrix">プロジェクション行列行列</param>
	void Draw(EnRenderMode renderMode, CMatrix viewMatrix, CMatrix projMatrix);
	/// <summary>
	/// マテリアルに対してクエリを行う。
	/// </summary>
	/// <param name="func">問い合わせ関数</param>
	void QueryMaterials(std::function<void(C3DModelEffect*)> func)
	{
		m_modelDx->UpdateEffects([&](DirectX::IEffect* material) {
			func(reinterpret_cast<C3DModelEffect*>(material));
		});
	}
	/// <summary>
	/// シャドウレシーバーのフラグを設定する。
	/// </summary>
	/// <param name="flag">trueを渡すとシャドウレシーバーになる</param>
	/// <remarks>
	/// シャドウレシーバーとは影を落とされるオブジェクトのことです。
	/// シャドウキャスターによって生成された、シャドウマップを利用して
	/// 自身に影を落とします。
	/// オブジェクトがシャドウレシーバーかつシャドウキャスターになっている場合は
	/// セルフシャドウ(自分の影が自分に落ちる)を行うことができます。
	/// </remarks>
	void SetShadowReciever(bool flag)
	{
		m_isShadowReciever = flag;
	}
	/// <summary>
	/// ディレクションライトのカラーを設定
	/// </summary>
	/// <param name="ligNo">ライトの番号</param>
	/// <param name="color">ライトのカラー</param>
	void SetDirectionLightColor(int ligNo, CVector4 color)
	{
		if (ligNo < NUM_DIRECTION_LIG) {
			m_dirLight.color[ligNo] = color;
		}
		else {
			std::abort();
		}
	}
	/// <summary>
	/// ディレクションライトの方向を設定。
	/// </summary>
	/// <param name="ligNo">ライトの番号</param>
	/// <param name="dir">ライトの方向</param>
	void SetDirectionLightDirection(int ligNo, CVector3 dir)
	{
		if (ligNo < NUM_DIRECTION_LIG) {
			m_dirLight.direction[ligNo] = dir;
		}
		else {
			std::abort();
		}
	}
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
