/*!
 * @brief	���f���V�F�[�_�[�B
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//�A���x�h�e�N�X�`���B
Texture2D<float4> g_grayTexture : register(t1);		//�O���[�e�N�X�`���B

/////////////////////////////////////////////////////////////
// SamplerState
/////////////////////////////////////////////////////////////
sampler g_sampler : register(s0);

/*!
 * @brief	���_�V�F�[�_�[�p�̒萔�o�b�t�@�B
 */
cbuffer VSCb : register(b0){
	float4x4 mWorld;
	float4x4 mView;
	float4x4 mProj;
};

/*!
 * @brief	�s�N�Z���V�F�[�_�[�p�̒萔�o�b�t�@�B
 */
cbuffer PSCb : register(b0) {
	float grayRate;		//�O���[�X�P�[���̃��[�g�B
};
/*!
 * @brief	�X�L���Ȃ����f���̒��_�\���́B
 */
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//���_���W�B
   	float2 TexCoord	: TEXCOORD0;		//UV���W�B
};

/*!
 * @brief	�s�N�Z���V�F�[�_�[�̓��́B
 */
struct PSInput{
	float4 Position 	: SV_POSITION;	//���W�B
	float2 TexCoord		: TEXCOORD0;	//UV���W�B
};

/*!--------------------------------------------------------------------------------------
 * @brief	�X�L���Ȃ����f���p�̒��_�V�F�[�_�[�B
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos = mul(mWorld, In.Position);
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	//UV���W�͂��̂܂܃s�N�Z���V�F�[�_�[�ɓn���B
	psInput.TexCoord = In.TexCoord;
	return psInput;
}
//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	//albedo�e�N�X�`������J���[���t�F�b�`����B
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	float4 grayColor = g_grayTexture.Sample(g_sampler, In.TexCoord);
	float4 finalColor = albedoColor * ( 1.0f - grayRate ) + grayColor * grayRate;
	return finalColor;
}