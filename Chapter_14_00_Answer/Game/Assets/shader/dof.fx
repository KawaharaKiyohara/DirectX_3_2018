/*!
 *@brief	どっふぉ
 */

/*!
 *@brief	頂点シェーダーの入力。
 */
struct VSInput{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief	ピクセルシェーダーの入力。
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> bokeTexture : register(t0);			//ボケテクスチャ。
Texture2D<float4> depthInViewTexture : register(t1);	//カメラ空間の深度テクスチャ。
sampler Sampler : register(s0);


/*!
 * @brief	頂点シェーダー。
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = In.pos;
	psIn.uv = In.uv;
	return psIn;
}

/////////////////////////////////////////////////////////
// 最終合成用のピクセルシェーダー。
/////////////////////////////////////////////////////////
float4 PSFinal(PSInput psIn) : SV_Target0
{
	float depth = depthInViewTexture.Sample(Sampler, psIn.uv);
	float alpha = saturate((depth - 500.0f) / 1000.0f);
	return float4( bokeTexture.Sample(Sampler, psIn.uv).xyz, alpha);
}