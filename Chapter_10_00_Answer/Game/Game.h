#pragma once

#include "C3DModelDraw.h"
#include "RenderTarget.h"
#include "ShadowMap.h"

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
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//ユニティちゃんの座標。
	C3DModelDraw m_unityChanModelDraw;		//ユニティちゃんのモデルの描画処理。。
	C3DModelDraw m_bgModelDraw;				//背景のモデルの描画処理。
	ShadowMap m_shadowMap;					//シャドウマップ。
};


