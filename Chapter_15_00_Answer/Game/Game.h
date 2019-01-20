#pragma once

#include "C3DModelDraw.h"
#include "graphics/RenderTarget.h"
#include "graphics/ShadowMap.h"
#include "graphics/Sprite.h"
#include "graphics/postEffect/PostEffect.h"
#include "sound/SoundEngine.h"
#include "sound/SoundSource.h"

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
	/// <summary>
	/// メインレンダリングターゲットを取得。
	/// </summary>
	/// <returns></returns>
	RenderTarget* GetMainRenderTarget()
	{
		return &m_mainRenderTarget;
	}
private:
	/// <summary>
	/// プリレンダリング。
	/// </summary>
	void PreRender();
	/// <summary>
	/// フォワードレンダリング(通常の描画だと考えてOK)
	/// </summary>
	void ForwordRender();
	/// <summary>
	/// ポストレンダリング
	/// </summary>
	void PostRender();
		/// <summary>
	/// カメラを初期化。
	/// </summary>
	void InitCamera();
private:
	CSoundEngine m_soundEngine;				//サウンドエンジン。
	RenderTarget m_mainRenderTarget;		//メインレンダリングターゲット。
	RenderTarget m_depthRenderTarget;		//深度値の書き込み先となるレンダリングターゲット。
	ShadowMap m_shadowMap;					//シャドウマップ。
	PostEffect m_postEffect;				//ポストエフェクト。
	int m_playBgmNo = 0;					//再生中のBGMの番号。
	CSoundSource m_bgm[2];						//BGM。
	CSoundSource m_se;						//SE。
	CSoundSource m_se2;
	Sprite m_copyMainRtToFrameBufferSprite;			//メインレンダリングターゲットに描かれた絵をフレームバッファにコピーするためのスプライト。
	D3D11_VIEWPORT m_frameBufferViewports;			//フレームバッファのビューポート。
	ID3D11RenderTargetView* m_frameBufferRenderTargetView = nullptr;	//フレームバッファのレンダリングターゲットビュー。
	ID3D11DepthStencilView* m_frameBufferDepthStencilView = nullptr;	//フレームバッファのデプスステンシルビュー。
};


