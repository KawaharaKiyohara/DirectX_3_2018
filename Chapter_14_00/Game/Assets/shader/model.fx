/*!
 * @brief	モデルシェーダー。
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//アルベドテクスチャ。
Texture2D<float4> g_shadowMap : register(t1);		//シャドウマップ。
/////////////////////////////////////////////////////////////
// SamplerState
/////////////////////////////////////////////////////////////
sampler g_sampler : register(s0);


/// <summary>
/// モデルシェーダー用の定数バッファ。
/// </summary>
cbuffer ModelFxConstantBuffer : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
	float4x4 mLightView;	//ライトビュー行列。
	float4x4 mLightProj;	//ライトプロジェクション行列。
	int isShadowReciever;	//シャドウレシーバーフラグ。
};
static const int NUM_DIRECTION_LIG = 4;

/// <summary>
/// ライト用の定数バッファ。
/// </summary>
cbuffer LightCb : register(b1){
	float3 dligDirection[NUM_DIRECTION_LIG];
	float4 dligColor[NUM_DIRECTION_LIG];
};

/// <summary>
/// シャドウマップ用の定数バッファ。
/// </summary>
cbuffer ShadowMapCb : register( b1 ){
	float4x4 lightViewProjMatrix;	//ライトビュープロジェクション行列。
}

/// <summary>
/// スキンなしモデルの頂点構造体。
/// </summary>
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//頂点座標。
    float3 Normal   : NORMAL;			//法線。
   	float2 TexCoord	: TEXCOORD0;		//UV座標。
};


/// <summary>
/// ピクセルシェーダーの入力。
/// </summary>
struct PSInput{
	float4 Position 	: SV_POSITION;	//座標。
	float3 Normal		: NORMAL;		//法線。
	float2 TexCoord		: TEXCOORD0;	//UV座標。
	float4 posInLVP		: TEXCOORD1;	//ライトビュープロジェクション空間での座標。
	float4 posInView	: TEXCOORD2;	//カメラ空間での座標。
};
/// <summary>
/// シャドウマップ用のピクセルシェーダへの入力構造体。
/// </summary>
struct PSInput_ShadowMap {
	float4 Position 			: SV_POSITION;	//座標。
};

/// <summary>
/// スキンなしモデル用の頂点シェーダー。
/// </summary>
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	//ローカル座標系からワールド座標系に変換する。
	float4 worldPos = mul(mWorld, In.Position);
	//ワールド座標系からカメラ座標系に変換する。
	psInput.Position  = mul(mView, worldPos);
	//todo カメラ空間での座標をピクセルシェーダーで使うので、保存しておく。
	psInput.posInView = psInput.Position;
	//カメラ座標系からスクリーン座標系に変換する。
	psInput.Position = mul(mProj, psInput.Position);
	
	if(isShadowReciever == 1 ){
		//続いて、ライトビュープロジェクション空間に変換。
		psInput.posInLVP = mul( mLightView, worldPos );
		psInput.posInLVP = mul( mLightProj, psInput.posInLVP );
	}
	
	//UV座標はそのままピクセルシェーダーに渡す。
	psInput.TexCoord = In.TexCoord;
	//法線はそのままピクセルシェーダーに渡す。
	psInput.Normal = In.Normal;
	
	return psInput;
}

///複数のレンダリングターゲットに出力するために、構造体を用意。
/// <summary>
/// ピクセルシェーダーからの出力構造体。
/// </summary>
struct PSOutput{
	float4 color		: SV_Target0;	//0番目のレンダリングターゲットに出力される。
	float depthInView	: SV_Target1;	//1番目のレンダリングターゲットに出力される。
};
/// <summary>
/// ピクセルシェーダーのエントリ関数。
/// </summary>
PSOutput PSMain(PSInput In) : SV_Target0
{
	PSOutput psOut = (PSOutput)0;

	//albedoテクスチャからカラーをフェッチする。
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	
	float3 lig = 0.0f;
	//ディレクションライトの拡散反射光を計算する。
		
	for (int i = 0; i < NUM_DIRECTION_LIG; i++) {
		lig += max(0.0f, dot(In.Normal * -1.0f, dligDirection[i])) * dligColor[i];
	}
	
	if(isShadowReciever == 1 ){	//シャドウレシーバー。
		//LVP空間から見た時の最も手前の深度値をシャドウマップから取得する。
		float2 shadowMapUV = In.posInLVP.xy / In.posInLVP.w;
		shadowMapUV *= float2(0.5f, -0.5f);
		shadowMapUV += 0.5f;
		//シャドウマップの範囲内かどうかを判定する。
		if ( shadowMapUV.x < 1.0f 
			&& shadowMapUV.x > 0.0f 
			&& shadowMapUV.y < 1.0f 
			&& shadowMapUV.y > 0.0f
		) {
			
			///LVP空間での深度値を計算。
			float zInLVP = In.posInLVP.z / In.posInLVP.w;
			//シャドウマップに書き込まれている深度値を取得。
			float zInShadowMap = g_shadowMap.Sample(g_sampler, shadowMapUV);

			if (zInLVP > zInShadowMap + 0.01f) {
				//影が落ちているので、光を弱くする
				lig *= 0.5f;
			}
		}
	}
	//カラーを出力。
	psOut.color = float4( albedoColor.xyz * lig, 1.0f );
	//カメラ座標系でのZ値を出力。
	psOut.depthInView = In.posInView.z;
	return psOut;
}
/// <summary>
/// シャドウマップ生成用の頂点シェーダー。
/// </summary>
PSInput_ShadowMap VSMain_ShadowMap(VSInputNmTxVcTangent In) 
{
	PSInput_ShadowMap psInput = (PSInput_ShadowMap)0;
	float4 pos = mul(mWorld, In.Position);
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	return psInput;
}
/// <summary>
/// ピクセルシェーダーのエントリ関数。
/// </summary>
float4 PSMain_ShadowMap(PSInput_ShadowMap In) : SV_Target0
{
	//射影空間でのZ値を返す。
	return In.Position.z  / In.Position.w;
}

