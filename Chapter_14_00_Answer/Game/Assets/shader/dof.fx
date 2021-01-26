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
	//カメラ空間でのZ値をサンプリング。
	float depth = depthInViewTexture.Sample(Sampler, psIn.uv);
	
	//カメラ空間でのZ値に応じて、α値を計算。
	//Z値が500以下の場合はαの値は0。500以上になると
	//αの値が大きくなっていきます。
	float alpha = saturate((depth - 500.0f) / 1000.0f);
	
	//47行目でもとめたαの値を、ボケ画像のα値として出力。
	//500以下の場合はαの値が0になっているため、
	//ボケ画像はメインシーンには合成されません。
	return float4( bokeTexture.Sample(Sampler, psIn.uv).xyz, alpha);
}