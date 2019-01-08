#pragma once
class Camera
{
public:
	/// <summary>
	/// 射影行列の更新方法。
	/// </summary>
	enum EnUpdateProjMatrixFunc {
		enUpdateProjMatrixFunc_Perspective,		//!<透視射影行列。遠近法が効いた絵を作りたいならこっち。
		enUpdateProjMatrixFunc_Ortho,			//!<平行投影。２Ｄ的な表現がしたいならこっち。
	};
	
	/// <summary>
	/// カメラ行列、プロジェクション行列の更新。
	/// </summary>
	/// <remarks>
	/// 現在設定されている、注視点、視点、画角などの情報をもとに
	/// カメラ行列とプロジェクション行列を更新します。
	/// この関数を呼ばないと、カメラは動かないので注意が必要です。
	/// </remarks>
	void Update();

	/// <summary>
	/// カメラ行列を取得。
	/// </summary>
	/// <returns>カメラ行列</returns>
	CMatrix GetViewMatrix()
	{
		return m_viewMatrix;
	}

	/// <summary>
	/// プロジェクション行列を取得。
	/// </summary>
	/// <returns>プロジェクション行列</returns>
	CMatrix GetProjectionMatrix() 
	{
		return m_projMatrix;
	}
	
	/// <summary>
	/// 視点を取得。
	/// </summary>
	/// <returns>視点</returns>
	CVector3 GetPosition()
	{
		return m_position;
	}
	
	/// <summary>
	/// 注視点を取得。
	/// </summary>
	/// <returns>注視点</returns>
	CVector3 GetTarget()
	{
		return m_target;
	}

	/// <summary>
	/// カメラの上方向を取得。
	/// </summary>
	/// <returns></returns>
	CVector3 GetUp()
	{
		return m_up;
	}

	/// <summary>
	/// 視点の座標を設定。
	/// </summary>
	/// <param name="pos">視点の座標。</param>
	void SetPosition( CVector3 pos )
	{
		m_position = pos;
	}

	/// <summary>
	/// 注視点を設定。
	/// </summary>
	/// <param name="target">注視点</param>
	void SetTarget( CVector3 target )
	{
		m_target = target;
	}

	/// <summary>
	/// カメラの上方向を設定。
	/// </summary>
	/// <param name="up">カメラの上方向</param>
	void SetUp( CVector3 up )
	{
		m_up = up;
	}

	/// <summary>
	/// 遠平面を設定。
	/// </summary>
	/// <param name="f">遠平面までの距離</param>
	void SetFar( float f )
	{
		m_far = f;
	}

	/// <summary>
	/// 近平面を設定。
	/// </summary>
	/// <param name="n">近平面までの距離</param>
	void SetNear( float n )
	{
		m_near = n;
	}
	/// <summary>
	/// 画角を設定。
	/// </summary>
	/// <param name="angle">画角。単位はラジアン。</param>
	void SetViewAngle( float angle )
	{
		m_viewAngle = angle;
	}
	/// <summary>
	/// 平行投影の幅を設定。
	/// </summary>
	/// <remarks>
	/// m_updateProjMatrixFuncがenUpdateProjMatrixFunc_Orthoの時に使用される。
	/// </remarks>
	/// <param name="w">幅</param>
	void SetWidth(float w)
	{
		m_width = w;
	}
	/// <summary>
	/// 平行投影の高さを設定。
	/// <remarks>
	/// </remarks>
	/// m_updateProjMatrixFuncがenUpdateProjMatrixFunc_Orthoの時に使用される。
	/// </summary>
	/// <param name="h">高さ</param>
	void SetHeight(float h)
	{
		m_height = h;
	}
	/// <summary>
	/// 射影行列の計算の仕方を設定。
	/// </summary>
	/// <param name="func">EnUpdateProjMatrixFuncを参照。</param>
	void SetUpdateProjMatrixFunc(EnUpdateProjMatrixFunc func)
	{
		m_updateProjMatrixFunc = func;
	}
private:
	CMatrix	m_viewMatrix = CMatrix::Identity();		//ビュー行列。
	CMatrix m_projMatrix = CMatrix::Identity();		//プロジェクション行列。
	CVector3 m_target = CVector3::Zero();			//注視点。
	CVector3 m_position = CVector3::Zero();			//視点。
	CVector3 m_up = CVector3::Up();					//上方向。
	float m_viewAngle = CMath::DegToRad(60.0f);		//画角。
	float m_far = 10000.0f;							//遠い平面までの距離。
	float m_near = 1.0f;							//近平面までの距離。
	float m_width = 1280.0f;						//平行投影の幅。
	float m_height = 720.0f;						//平行投影の高さ。
	EnUpdateProjMatrixFunc m_updateProjMatrixFunc = enUpdateProjMatrixFunc_Perspective;	//射影行列の作成方法。
};

extern Camera g_camera3D;		//!<3Dカメラ。
extern Camera g_camera2D;		//!<2Dカメラ。