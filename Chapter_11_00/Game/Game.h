#pragma once

#include "C3DModelDraw.h"
#include "RenderTarget.h"
#include "ShadowMap.h"
#include "Sprite.h"

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
	/// <summary>
	/// シャドウマップを取得。
	/// </summary>
	/// <returns></returns>
	ShadowMap* GetShadowMap()
	{
		return &m_shadowMap;
	}
private:
	/// <summary>
	/// 半透明合成のブレンドステートを初期化。
	/// </summary>
	void InitTranslucentBlendState();
private:
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };			//ユニティちゃんの座標。
	C3DModelDraw m_unityChanModelDraw;						//ユニティちゃんのモデルの描画処理。。
	C3DModelDraw m_bgModelDraw;								//背景のモデルの描画処理。
	ID3D11BlendState* m_translucentBlendState = nullptr;	//半透明合成用のブレンドステート。
	Sprite m_sprite;										//スプライト。
	ShadowMap m_shadowMap;									//シャドウマップ。
};


