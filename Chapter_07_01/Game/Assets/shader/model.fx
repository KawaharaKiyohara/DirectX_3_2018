/*!
 * @brief	モデルシェーダー。
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//アルベドテクスチャ。

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
 *@brief	ディレクションライト。
 */
struct SDirectionLight{
	float3 direction;
	float4 color;
};
/*!
 *@brief	ライト用の定数バッファ。
 */
cbuffer LightCb : register(b0){
	SDirectionLight		directionLight;		//ディレクションライト。
	float3				eyePos;				//カメラの視点。
	float				specPow;			//スペキュラライトの絞り。
};

/*!
 * @brief	スキンなしモデルの頂点構造体。
 */
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//頂点座標。
    float3 Normal   : NORMAL;			//法線。
   	float2 TexCoord	: TEXCOORD0;		//UV座標。
};

/*!
 * @brief	ピクセルシェーダーの入力。
 */
struct PSInput{
	float4 Position 	: SV_POSITION;	//座標。
	float3 Normal		: NORMAL;		//法線。
	float2 TexCoord		: TEXCOORD0;	//UV座標。
	float3 worldPos		: TEXCOORD1;	//ワールド座標。
};

/*!--------------------------------------------------------------------------------------
 * @brief	スキンなしモデル用の頂点シェーダー。
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos = mul(mWorld, In.Position);
	//鏡面反射の計算のために、ワールド座標をピクセルシェーダーに渡す。
	psInput.worldPos = pos;
	
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	//UV座標はそのままピクセルシェーダーに渡す。
	psInput.TexCoord = In.TexCoord;
	//法線はそのままピクセルシェーダーに渡す。
	psInput.Normal = In.Normal;
	return psInput;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	//albedoテクスチャからカラーをフェッチする。
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	//ディレクションライトの拡散反射光を計算する。
	float3 lig = max( 0.0f, dot( In.Normal * -1.0f, directionLight.direction) ) * directionLight.color.xyz;
	//ディレクションライトの鏡面反射光を計算する。
	{
		//ライトを当てる面から視点に伸びるベクトルtoEyeDirを求める。
		float3 toEyeDir = normalize( eyePos - In.worldPos );
		//反射ベクトルを求める。
		float3 reflectEyeDir = -toEyeDir + 2 * dot( In.Normal, toEyeDir) * In.Normal;
		float t = max( 0.0f, dot( -directionLight.direction, reflectEyeDir));
		float3 specLig = pow( t, specPow ) * directionLight.color.xyz;
		//鏡面反射を反射光に加算する。
		lig += specLig;
	}
	float4 finalColor = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalColor.xyz = albedoColor.xyz * lig;
	return finalColor;
}