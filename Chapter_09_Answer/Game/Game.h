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
	/// レンダリングモード。
	/// </summary>
	enum EnRenderMode {
		enRenderMode_Normal,		//通常レンダリング。
		enRenderMode_Silhouette,	//シルエットをレンダリング。
		enRenderMode_Num,			//レンダリングモードの数。
	};
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
	/// レンダリングターゲットの解放。
	/// </summary>
	void ReleaseRenderTarget();
	/// <summary>
	/// レンダリングターゲットの作成。
	/// </summary>
	void CreateRenderTarget();
	
private:
	CVector3 m_unityChanPos = { 0.0f, 0.0f, 0.0f };	//ユニティちゃんの座標。
	C3DModelDraw m_unityChanModelDraw;		//ユニティちゃんのモデルの描画処理。。
	C3DModelDraw m_bgModelDraw;				//背景のモデルの描画処理。
	//ここからレンダリングターゲット関係のメンバ変数です。
	RenderTarget m_renderTarget;			//レンダリングターゲット。
};


