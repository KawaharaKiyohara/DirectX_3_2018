/*!
 * @brief	���f���V�F�[�_�[�B
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//�A���x�h�e�N�X�`���B

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
 *@brief	�f�B���N�V�������C�g�B
 */
struct SDirectionLight{
	float3 direction;
	float4 color;
};
/*!
 *@brief	���C�g�p�̒萔�o�b�t�@�B
 */
cbuffer LightCb : register(b0){
	SDirectionLight		directionLight;		//�f�B���N�V�������C�g�B
	float3				eyePos;				//�J�����̎��_�B
	float				specPow;			//�X�y�L�������C�g�̍i��B
};

/*!
 * @brief	�X�L���Ȃ����f���̒��_�\���́B
 */
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//���_���W�B
    float3 Normal   : NORMAL;			//�@���B
   	float2 TexCoord	: TEXCOORD0;		//UV���W�B
};

/*!
 * @brief	�s�N�Z���V�F�[�_�[�̓��́B
 */
struct PSInput{
	float4 Position 	: SV_POSITION;	//���W�B
	float3 Normal		: NORMAL;		//�@���B
	float2 TexCoord		: TEXCOORD0;	//UV���W�B
	float3 worldPos		: TEXCOORD1;	//���[���h���W�B
};

/*!--------------------------------------------------------------------------------------
 * @brief	�X�L���Ȃ����f���p�̒��_�V�F�[�_�[�B
-------------------------------------------------------------------------------------- */
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	float4 pos = mul(mWorld, In.Position);
	//���ʔ��˂̌v�Z�̂��߂ɁA���[���h���W���s�N�Z���V�F�[�_�[�ɓn���B
	psInput.worldPos = pos;
	
	pos = mul(mView, pos);
	pos = mul(mProj, pos);
	psInput.Position = pos;
	//UV���W�͂��̂܂܃s�N�Z���V�F�[�_�[�ɓn���B
	psInput.TexCoord = In.TexCoord;
	//�@���͂��̂܂܃s�N�Z���V�F�[�_�[�ɓn���B
	psInput.Normal = In.Normal;
	return psInput;
}
//--------------------------------------------------------------------------------------
// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
//--------------------------------------------------------------------------------------
float4 PSMain( PSInput In ) : SV_Target0
{
	//albedo�e�N�X�`������J���[���t�F�b�`����B
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	//�f�B���N�V�������C�g�̊g�U���ˌ����v�Z����B
	float3 lig = max( 0.0f, dot( In.Normal * -1.0f, directionLight.direction) ) * directionLight.color.xyz;
	//�f�B���N�V�������C�g�̋��ʔ��ˌ����v�Z����B
	{
		//���C�g�𓖂Ă�ʂ��王�_�ɐL�т�x�N�g��toEyeDir�����߂�B
		float3 toEyeDir = normalize( eyePos - In.worldPos );
		//���˃x�N�g�������߂�B
		float3 reflectEyeDir = -toEyeDir + 2 * dot( In.Normal, toEyeDir) * In.Normal;
		float t = max( 0.0f, dot( -directionLight.direction, reflectEyeDir));
		float3 specLig = pow( t, specPow ) * directionLight.color.xyz;
		//���ʔ��˂𔽎ˌ��ɉ��Z����B
		lig += specLig;
	}
	float4 finalColor = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	finalColor.xyz = albedoColor.xyz * lig;
	return finalColor;
}