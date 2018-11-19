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
