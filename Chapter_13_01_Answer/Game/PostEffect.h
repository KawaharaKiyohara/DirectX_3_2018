#pragma once
#include "Bloom.h"

/// <summary>
/// ポストエフェクトクラス。
/// </summary>
class PostEffect
{
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	PostEffect();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~PostEffect();
	/// <summary>
	/// 更新。
	/// </summary>
	void Update();
	/// <summary>
	/// ドロー。
	/// </summary>
	void Draw();
	/// <summary>
	/// フルスクリーン描画。
	/// </summary>
	void DrawFullScreenQuadPrimitive( ID3D11DeviceContext* deviceContext, Shader& vsShader, Shader& psShader );
private:
	/// <summary>
	/// フルスクリーン描画用の四角形プリミティブの初期化。
	/// </summary>
	void InitFullScreenQuadPrimitive();
private:
	Bloom	m_bloom;	//ブルーム。
	//フルスクリーン描画用のメンバ変数。
	ID3D11Buffer*	m_vertexBuffer = nullptr;		//頂点バッファ。
	ID3D11InputLayout* m_inputLayout = nullptr;		//入力レイアウト。
};

