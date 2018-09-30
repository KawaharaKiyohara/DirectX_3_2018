/*!
 * @brief	モデルシェーダー。
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//アルベドテクスチャ。
Texture2D<float4> g_grayTexture : register(t1);		//グレーテクスチャ。

/////////////////////////////////////////////////////////////
// SamplerState
/////////////////////////////////////////////////////////////
sampler g_sampler : register(s0);

/*!
 * @brief	頂点シェーダー用の定数バッファ。
 */
cbuffer VSCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

/*!
 * @brief	ピクセルシェーダー用の定数バッファ。
 */
cbuffer PSCb : register(b0) {
	float grayRate;		//グレースケールのレート。
};
/*!
 * @brief	スキンなしモデルの頂点構造体。
 */
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//頂点座標。
   	float2 TexCoord	: TEXCOORD0;		//UV座標。
};

/*!
 * @brief	ピクセルシェーダーの入力。
 */
struct PSInput{
	float4 Position 	: SV_POSITION;	//座標。
	float2 TexCoord		: TEXCOORD0;	//UV座標。
};

/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos = mul(mWorld, In.Position);
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	//UV座標はそのままピクセルシェーダーに渡す。
	psInput.TexCoord = In.TexCoord;
	return psInput;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	//albedoテクスチャからカラーをフェッチする。
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	float4 grayColor = g_grayTexture.Sample(g_sampler, In.TexCoord);
	float4 finalColor = albedoColor * ( 1.0f - grayRate ) + grayColor * grayRate;
	return finalColor;
}