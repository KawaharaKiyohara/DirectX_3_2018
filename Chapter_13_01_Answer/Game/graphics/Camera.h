#pragma once
class Camera
{
public:
	/// <summary>
	/// �ˉe�s��̍X�V���@�B
	/// </summary>
	enum EnUpdateProjMatrixFunc {
		enUpdateProjMatrixFunc_Perspective,		//!<�����ˉe�s��B���ߖ@���������G����肽���Ȃ炱�����B
		enUpdateProjMatrixFunc_Ortho,			//!<���s���e�B�Q�c�I�ȕ\�����������Ȃ炱�����B
	};
	
	/// <summary>
	/// �J�����s��A�v���W�F�N�V�����s��̍X�V�B
	/// </summary>
	/// <remarks>
	/// ���ݐݒ肳��Ă���A�����_�A���_�A��p�Ȃǂ̏������Ƃ�
	/// �J�����s��ƃv���W�F�N�V�����s����X�V���܂��B
	/// ���̊֐����Ă΂Ȃ��ƁA�J�����͓����Ȃ��̂Œ��ӂ��K�v�ł��B
	/// </remarks>
	void Update();

	/// <summary>
	/// �J�����s����擾�B
	/// </summary>
	/// <returns>�J�����s��</returns>
	CMatrix GetViewMatrix()
	{
		return m_viewMatrix;
	}

	/// <summary>
	/// �v���W�F�N�V�����s����擾�B
	/// </summary>
	/// <returns>�v���W�F�N�V�����s��</returns>
	CMatrix GetProjectionMatrix() 
	{
		return m_projMatrix;
	}
	
	/// <summary>
	/// ���_���擾�B
	/// </summary>
	/// <returns>���_</returns>
	CVector3 GetPosition()
	{
		return m_position;
	}
	
	/// <summary>
	/// �����_���擾�B
	/// </summary>
	/// <returns>�����_</returns>
	CVector3 GetTarget()
	{
		return m_target;
	}

	/// <summary>
	/// �J�����̏�������擾�B
	/// </summary>
	/// <returns></returns>
	CVector3 GetUp()
	{
		return m_up;
	}

	/// <summary>
	/// ���_�̍��W��ݒ�B
	/// </summary>
	/// <param name="pos">���_�̍��W�B</param>
	void SetPosition( CVector3 pos )
	{
		m_position = pos;
	}

	/// <summary>
	/// �����_��ݒ�B
	/// </summary>
	/// <param name="target">�����_</param>
	void SetTarget( CVector3 target )
	{
		m_target = target;
	}

	/// <summary>
	/// �J�����̏������ݒ�B
	/// </summary>
	/// <param name="up">�J�����̏����</param>
	void SetUp( CVector3 up )
	{
		m_up = up;
	}

	/// <summary>
	/// �����ʂ�ݒ�B
	/// </summary>
	/// <param name="f">�����ʂ܂ł̋���</param>
	void SetFar( float f )
	{
		m_far = f;
	}

	/// <summary>
	/// �ߕ��ʂ�ݒ�B
	/// </summary>
	/// <param name="n">�ߕ��ʂ܂ł̋���</param>
	void SetNear( float n )
	{
		m_near = n;
	}
	/// <summary>
	/// ��p��ݒ�B
	/// </summary>
	/// <param name="angle">��p�B�P�ʂ̓��W�A���B</param>
	void SetViewAngle( float angle )
	{
		m_viewAngle = angle;
	}
	/// <summary>
	/// ���s���e�̕���ݒ�B
	/// </summary>
	/// <remarks>
	/// m_updateProjMatrixFunc��enUpdateProjMatrixFunc_Ortho�̎��Ɏg�p�����B
	/// </remarks>
	/// <param name="w">��</param>
	void SetWidth(float w)
	{
		m_width = w;
	}
	/// <summary>
	/// ���s���e�̍�����ݒ�B
	/// <remarks>
	/// </remarks>
	/// m_updateProjMatrixFunc��enUpdateProjMatrixFunc_Ortho�̎��Ɏg�p�����B
	/// </summary>
	/// <param name="h">����</param>
	void SetHeight(float h)
	{
		m_height = h;
	}
	/// <summary>
	/// �ˉe�s��̌v�Z�̎d����ݒ�B
	/// </summary>
	/// <param name="func">EnUpdateProjMatrixFunc���Q�ƁB</param>
	void SetUpdateProjMatrixFunc(EnUpdateProjMatrixFunc func)
	{
		m_updateProjMatrixFunc = func;
	}
private:
	CMatrix	m_viewMatrix = CMatrix::Identity();		//�r���[�s��B
	CMatrix m_projMatrix = CMatrix::Identity();		//�v���W�F�N�V�����s��B
	CVector3 m_target = CVector3::Zero();			//�����_�B
	CVector3 m_position = CVector3::Zero();			//���_�B
	CVector3 m_up = CVector3::Up();					//������B
	float m_viewAngle = CMath::DegToRad(60.0f);		//��p�B
	float m_far = 10000.0f;							//�������ʂ܂ł̋����B
	float m_near = 1.0f;							//�ߕ��ʂ܂ł̋����B
	float m_width = 1280.0f;						//���s���e�̕��B
	float m_height = 720.0f;						//���s���e�̍����B
	EnUpdateProjMatrixFunc m_updateProjMatrixFunc = enUpdateProjMatrixFunc_Perspective;	//�ˉe�s��̍쐬���@�B
};

extern Camera g_camera3D;		//!<3D�J�����B
extern Camera g_camera2D;		//!<2D�J�����B