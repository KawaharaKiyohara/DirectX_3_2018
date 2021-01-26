/*!
 *@brief	�ǂ��ӂ�
 */

/*!
 *@brief	���_�V�F�[�_�[�̓��́B
 */
struct VSInput{
	float4 pos : SV_Position;
	float2 uv  : TEXCOORD0;
};

/*!
 *@brief	�s�N�Z���V�F�[�_�[�̓��́B
 */
struct PSInput{
	float4 pos : SV_POSITION;
	float2 uv  : TEXCOORD0;
};

Texture2D<float4> bokeTexture : register(t0);			//�{�P�e�N�X�`���B
Texture2D<float4> depthInViewTexture : register(t1);	//�J������Ԃ̐[�x�e�N�X�`���B
sampler Sampler : register(s0);


/*!
 * @brief	���_�V�F�[�_�[�B
 */
PSInput VSMain(VSInput In) 
{
	PSInput psIn;
	psIn.pos = In.pos;
	psIn.uv = In.uv;
	return psIn;
}

/////////////////////////////////////////////////////////
// �ŏI�����p�̃s�N�Z���V�F�[�_�[�B
/////////////////////////////////////////////////////////
float4 PSFinal(PSInput psIn) : SV_Target0
{
	//�J������Ԃł�Z�l���T���v�����O�B
	float depth = depthInViewTexture.Sample(Sampler, psIn.uv);
	
	//�J������Ԃł�Z�l�ɉ����āA���l���v�Z�B
	//Z�l��500�ȉ��̏ꍇ�̓��̒l��0�B500�ȏ�ɂȂ��
	//���̒l���傫���Ȃ��Ă����܂��B
	float alpha = saturate((depth - 500.0f) / 1000.0f);
	
	//47�s�ڂł��Ƃ߂����̒l���A�{�P�摜�̃��l�Ƃ��ďo�́B
	//500�ȉ��̏ꍇ�̓��̒l��0�ɂȂ��Ă��邽�߁A
	//�{�P�摜�̓��C���V�[���ɂ͍�������܂���B
	return float4( bokeTexture.Sample(Sampler, psIn.uv).xyz, alpha);
}