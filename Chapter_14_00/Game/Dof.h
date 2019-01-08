#pragma once

#include "RenderTarget.h"
#include "GaussianBlur.h"

class PostEffect;

/// <summary>
/// 被写界深度
/// </summary>
/// <remarks>
/// DofクラスのインスタンスはPostEffectクラスが保持しています。
/// </remarks>
class Dof
{
public:

	/// <summary>
	/// デストラクタ。
	/// </summary>
	/// <remarks>
	/// 終了処理を記述してください。
	/// </remarks>
	~Dof();
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <remarks>
	/// PostEffect::Init関数から呼び出されています。
	/// 初期化処理を記述してください。
	/// </remarks>
	void Init();
	/// <summary>
	/// 描画。
	/// </summary>
	/// <param name="postEffect"></param>
	void Draw(PostEffect& postEffect);
private:
	//必要なメンバ変数は追加していってね。
};

