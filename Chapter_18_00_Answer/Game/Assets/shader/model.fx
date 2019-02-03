/*!
 * @brief	���f���V�F�[�_�[�B
 */

/////////////////////////////////////////////////////////////
// Shader Resource View
/////////////////////////////////////////////////////////////
Texture2D<float4> g_albedoTexture : register(t0);	//�A���x�h�e�N�X�`���B
Texture2D<float4> g_shadowMap : register(t1);		//todo �V���h�E�}�b�v�B
Texture2D<float4> g_normalMap : register(t2);		//	�@���}�b�v�B
Texture2D<float4> g_specularMap : register(t3);
/////////////////////////////////////////////////////////////
// SamplerState
/////////////////////////////////////////////////////////////
sampler g_sampler : register(s0);


/// <summary>
/// ���f���V�F�[�_�[�p�̒萔�o�b�t�@�B
/// </summary>
cbuffer ModelFxConstantBuffer : register(b0){
	float4x4 mWorld			: packoffset(c0);
	float4x4 mView			: packoffset(c4);
	float4x4 mProj			: packoffset(c8);
	float4x4 mLightView		: packoffset(c12);	//���C�g�r���[�s��B
	float4x4 mLightProj		: packoffset(c16);	//���C�g�v���W�F�N�V�����s��B
	int isShadowReciever	: packoffset(c20.x);	//�V���h�E���V�[�o�[�t���O�B
	int isHasNormalMap		: packoffset(c20.y);	//�@���}�b�v����H
	int isHasSpecularMap	: packoffset(c20.z);	//�X�y�L�����}�b�v����H
};
static const int NUM_DIRECTION_LIG = 4;

/// <summary>
/// ���C�g�p�̒萔�o�b�t�@�B
/// </summary>
cbuffer LightCb : register(b1){
	float3 dligDirection[NUM_DIRECTION_LIG];
	float4 dligColor[NUM_DIRECTION_LIG];
};

/// <summary>
/// �V���h�E�}�b�v�p�̒萔�o�b�t�@�B
/// </summary>
cbuffer ShadowMapCb : register( b1 ){
	float4x4 lightViewProjMatrix;	//���C�g�r���[�v���W�F�N�V�����s��B
}

/// <summary>
/// �X�L���Ȃ����f���̒��_�\���́B
/// </summary>
struct VSInputNmTxVcTangent
{
    float4 Position : SV_Position;		//���_���W�B
    float3 Normal   : NORMAL;			//�@���B
   	float2 TexCoord	: TEXCOORD0;		//UV���W�B
   	float3 tangent	: TANGENT;			//�ڃx�N�g���B
};


/// <summary>
/// �s�N�Z���V�F�[�_�[�̓��́B
/// </summary>
struct PSInput{
	float4 Position 	: SV_POSITION;	//���W�B
	float3 Normal		: NORMAL;		//�@���B
	float2 TexCoord		: TEXCOORD0;	//UV���W�B
	float4 posInLVP		: TEXCOORD1;	//���C�g�r���[�v���W�F�N�V������Ԃł̍��W�B
	float3 tangent		: TEXCOORD2;	//�ڃx�N�g���B
};
/// <summary>
/// �V���h�E�}�b�v�p�̃s�N�Z���V�F�[�_�ւ̓��͍\���́B
/// </summary>
struct PSInput_ShadowMap {
	float4 Position 			: SV_POSITION;	//���W�B
};

/// <summary>
/// �X�L���Ȃ����f���p�̒��_�V�F�[�_�[�B
/// </summary>
PSInput VSMain( VSInputNmTxVcTangent In ) 
{
	PSInput psInput = (PSInput)0;
	//���[�J�����W�n���烏�[���h���W�n�ɕϊ�����B
	float4 worldPos = mul(mWorld, In.Position);
	//���[���h���W�n����J�������W�n�ɕϊ�����B
	psInput.Position  = mul(mView, worldPos);
	//�J�������W�n����X�N���[�����W�n�ɕϊ�����B
	psInput.Position = mul(mProj, psInput.Position);
	
	if(isShadowReciever == 1 ){
		//�����āA���C�g�r���[�v���W�F�N�V������Ԃɕϊ��B
		psInput.posInLVP = mul( mLightView, worldPos );
		psInput.posInLVP = mul( mLightProj, psInput.posInLVP );
	}
	
	//UV���W�͂��̂܂܃s�N�Z���V�F�[�_�[�ɓn���B
	psInput.TexCoord = In.TexCoord;
	//�@�������[���h�s��ŕϊ�����B
	//�@����float3��3�v�f�̃x�N�g���Ȃ̂ŁA��]�Ɗg��̕ϊ��������s����B
	psInput.Normal = mul( mWorld, In.Normal);
	//�g�听���������Ă��邩������Ȃ��̂ŁA���K�����Ƃ��B
	psInput.Normal = normalize(psInput.Normal);
	//�ڃx�N�g�����񂷁B
	psInput.tangent = mul( mWorld, In.tangent);
	psInput.tangent = normalize(psInput.tangent);
		
		
	return psInput;
}
/// <summary>
/// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
/// </summary>
float4 PSMain(PSInput In) : SV_Target0
{
	//albedo�e�N�X�`������J���[���t�F�b�`����B
	float4 albedoColor = g_albedoTexture.Sample(g_sampler, In.TexCoord);
	
	//�@�����v�Z����B
	float3 normal = 0;
	if(isHasNormalMap == 1){
		//�@���}�b�v������B
		//�@���Ɛڃx�N�g���̊O�ς��v�Z���āA�]�x�N�g�����v�Z����B
		float3 biNormal = cross(In.Normal, In.tangent);
		normal = g_normalMap.Sample(g_sampler, In.TexCoord);
		//0.0�`1.0�͈̔͂ɂȂ��Ă���^���W�F���g�X�y�[�X�@����
		//-1.0�`1.0�͈̔͂ɕϊ�����B
		normal =(normal * 2.0f)- 1.0f;
		//�@�����^���W�F���g�X�y�[�X����A���[���h�X�y�[�X�ɕϊ�����B
		normal = In.tangent * normal.x + biNormal * normal.y + In.Normal * normal.z;
	}else{
		//�Ȃ��B
		normal = In.Normal;
	}
	float3 lig = 0.0f;
	//�f�B���N�V�������C�g�̊g�U���ˌ����v�Z����B
	for (int i = 0; i < NUM_DIRECTION_LIG; i++) {
		lig += max(0.0f, dot(normal * -1.0f, dligDirection[i])) * dligColor[i];
	}
	
	if(isShadowReciever == 1 ){	//�V���h�E���V�[�o�[�B
		//LVP��Ԃ��猩�����̍ł���O�̐[�x�l���V���h�E�}�b�v����擾����B
		float2 shadowMapUV = In.posInLVP.xy / In.posInLVP.w;
		shadowMapUV *= float2(0.5f, -0.5f);
		shadowMapUV += 0.5f;
		//�V���h�E�}�b�v�͈͓̔����ǂ����𔻒肷��B
		if ( shadowMapUV.x < 1.0f 
			&& shadowMapUV.x > 0.0f 
			&& shadowMapUV.y < 1.0f 
			&& shadowMapUV.y > 0.0f
		) {
			
			///LVP��Ԃł̐[�x�l���v�Z�B
			float zInLVP = In.posInLVP.z / In.posInLVP.w;
			//�V���h�E�}�b�v�ɏ������܂�Ă���[�x�l���擾�B
			float zInShadowMap = g_shadowMap.Sample(g_sampler, shadowMapUV);

			if (zInLVP > zInShadowMap + 0.01f) {
				//�e�������Ă���̂ŁA�����キ����
				lig *= 0.5f;
			}
		}
	}
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	finalColor.xyz = albedoColor.xyz * lig;
	return finalColor;
}
/// <summary>
/// �V���h�E�}�b�v�����p�̒��_�V�F�[�_�[�B
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
/// �s�N�Z���V�F�[�_�[�̃G���g���֐��B
/// </summary>
float4 PSMain_ShadowMap(PSInput_ShadowMap In) : SV_Target0
{
	//�ˉe��Ԃł�Z�l��Ԃ��B
	return In.Position.z  / In.Position.w;
}

