#pragma once

#include "C3DModelDraw.h"
#include "RenderTarget.h"

/// <summary>
/// ゲームクラス。
/// </summary>
class Game
{
	
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	Game();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~Game();

	/// <summary>
	/// 更新処理。
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理。
	/// </summary>
	void Render();
private:
	/// <summary>
	/// シャドウマップにレンダリング。
	/// </summary>
	void RenderToShadowMap();
	/// <summary>
	/// シャドウマップ関係の更新処理。
	/// </summary>
	/// <remarks>
	/// ライトビュー・プロジェクション行列などが計算されます。
	/// </remarks>
	void UpdateShadowMap();
private:
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//ユニティちゃんの座標。
	C3DModelDraw m_unityChanModelDraw;		//ユニティちゃんのモデルの描画処理。。
	C3DModelDraw m_bgModelDraw;				//背景のモデルの描画処理。

	CVector3 m_lightCameraPosition;				//ライトカメラの視点。
	CVector3 m_lightCameraTarget;				//ライトカメラの注視点。
	CMatrix m_lightViewMatrix;					//ライトビュー行列。
	CMatrix m_lightProjMatrix;					//ライトプロジェクション行列。
	RenderTarget m_shadowMapRT;					//シャドウマップを描画するレンダリングターゲット。
};


