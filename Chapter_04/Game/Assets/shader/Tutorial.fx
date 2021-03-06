/*!
 * @brief	シンプルシェーダー。
 */

//定数バッファ
cbuffer VSCb : register( b0 ){
	float4x4 mWorld;	//ワールド行列。
	float4x4 mView;		//ビュー行列。
	float4x4 mProj;		//プロジェクション行列。
};
//--------------------------------------------------------------------------------------
// 頂点シェーダーのエントリー関数。
//--------------------------------------------------------------------------------------
float4 VS( float4 Pos : POSITION ) : SV_POSITION
{
	//ワールド行列をモデルの頂点座標に乗算する。
	float4 pos = mul( mWorld, Pos );
	//ビュー行列を乗算。
	pos = mul( mView, pos );
	//プロジェクション行列を乗算して、
	//スクリーンの正規化座標系(-1.0〜1.0の座標系)に変換する。
	pos = mul( mProj, pos );
	//座標変換が終わった。
    return pos;
}
//--------------------------------------------------------------------------------------
// ピクセルシェーダーのエントリ関数。
//--------------------------------------------------------------------------------------
float4 PS( float4 pos : SV_POSITION ) : SV_Target
{
	float4 color = float4( 1.0f, 1.0f, 1.0f, 1.0f);
    return color; 
}
