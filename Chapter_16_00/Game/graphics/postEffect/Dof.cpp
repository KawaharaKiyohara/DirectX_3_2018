#include "stdafx.h"
#include "Dof.h"
#include "Game.h"

/// <summary>
/// デストラクタ。
/// </summary>
/// <remarks>
/// 終了処理を記述してください。
/// </remarks>
Dof::~Dof()
{
}
/// <summary>
/// 初期化。
/// </summary>
/// <remarks>
/// PostEffect::Init関数から呼び出されています。
/// 初期化処理を記述してください。
/// </remarks>
void Dof::Init()
{
	
}
/// <summary>
/// この関数はPostEffect::Drawから呼ばれているよ。
/// </summary>
/// <param name="postEffect"></param>
void Dof::Draw(PostEffect& postEffect)
{
	//描画ステップ_1
	//メインレンダリングターゲットに描かれた絵にブラーをかけていく。
	{
	}
	//描画ステップ_2
	//ステップ１でぼかした画像を、メインレンダリングターゲットに
	//半透明合成で書き込んでいく。
	{
	}
}