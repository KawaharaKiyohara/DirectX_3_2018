/*!
 * @brief	�V���v���V�F�[�_�[�B
 */
cbuffer PSCb : register( b0 ){
	float4 mulColor;
	float4 addColor;
};

//--------------------------------------------------------------------------------------
// ���_�V�F�[�_�[�̃G���g���[�֐��B
//--------------------------------------------------------------------------------------
float4 VS( float4 Pos : POSITION ) : SV_POSITION
{
	float4 pos = Pos;
    return pos;
}


//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PS( float4 pos : SV_POSITION ) : SV_Target
{
	float4 color = float4( 1.0f, 1.0f, 1.0f, 1.0f);
	color *= mulColor;
	color += addColor;
    return color; 
}
