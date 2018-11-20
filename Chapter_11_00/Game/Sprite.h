#pragma once
#include "graphics/Shader.h"

/// <summary>
/// スプライトクラス。
/// </summary>
class Sprite
{
public:
	/// <summary>
	/// コンストラクタ。
	/// </summary>
	Sprite();
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~Sprite();
	
	/// <summary>
	/// 初期化。
	/// </summary>
	/// <param name="texFilePath">テクスチャのファイルパス</param>
	/// <param name="w">画像の幅</param>
	/// <param name="h">画像の高さ</param>
	void Init(const wchar_t* texFilePath, float w, float h);

	/// <summary>
	/// 更新。
	/// </summary>
	/// <param name="trans">平行移動量</param>
	/// <param name="rot">回転</param>
	/// <param name="scale">拡大率</param>
	/// <param name="pivot">
	/// ピボット
	/// 0.5, 0.5で画像の中心が基点。
	///	0.0, 0.0で画像の左下。
	///	1.0, 1.0で画像の右上。
	///	UnityのuGUIに準拠。
	/// </param>
	void Update(const CVector3& trans, const CQuaternion& rot, const CVector3& scale, CVector2 pivot = {0.5f, 0.5f});

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
private:

	/// <summary>
	/// 定数バッファの初期化。
	/// </summary>
	void InitConstantBuffer();

	/// <summary>
	/// 頂点バッファの初期化。
	/// </summary>
	/// <param name="w">画像の幅</param>
	/// <param name="h">画像の高さ</param>
	void InitVertexBuffer(float w, float h);

	/// <summary>
	/// インデックスバッファの初期化。
	/// </summary>
	void InitIndexBuffer();

	/// <summary>
	/// サンプラステートの初期化。
	/// </summary>
	void InitSamplerState();
private:
	struct ConstantBuffer {
		CMatrix WVP;		//ワールドビュープロジェクション行列。
	};
	ID3D11Buffer*				m_vertexBuffer = NULL;					//頂点バッファ。
	ID3D11Buffer*				m_indexBuffer = NULL;					//インデックスバッファ。
	Shader						m_vs;									//頂点シェーダー。
	Shader						m_ps;									//ピクセルシェーダー。
	ID3D11ShaderResourceView*	m_texture = NULL;						//テクスチャ。
	ID3D11SamplerState*			m_samplerState = NULL;					//サンプラステート。
	CVector3					m_position = CVector3::Zero();			//座標。
	CQuaternion					m_rotation = CQuaternion::Identity();	//回転
	CVector3					m_scale = CVector3::One();
	CMatrix						m_world = CMatrix::Identity();			//ワールド行列。
	CVector2					m_size = CVector2::Zero();				//画像のサイズ。
	ID3D11Buffer*				m_cb = nullptr;							//定数バッファ。

};

