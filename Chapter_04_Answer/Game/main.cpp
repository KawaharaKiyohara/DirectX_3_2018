#include "stdafx.h"
#include "system/system.h"
#include "level/Level.h"
#include "graphics/Shader.h"


//三角形描画クラス。
class TriangleDraw {
	
	Shader m_vsShader;							//!<VSシェーダー。
	Shader m_psShader;							//!<PSシェーダー。
	ID3D11Buffer*	m_vertexBuffer = nullptr;	//!<頂点バッファ。
	ID3D11Buffer*	m_constantBuffer = nullptr;	//!<定数バッファ。
	//頂点シェーダー用の定数バッファ。
	struct SVSConstantBuffer {
		CMatrix mWVP;		//WVP行列。
	};

	struct SSimpleVertex {
		CVector4 pos;
	};
public:
	//コンストラクタ。
	TriangleDraw()
	{
		//頂点バッファを初期化する。
		InitVertexBuffer();
		//定数バッファを初期化する。
		InitConstantBuffer();
		//シェーダーをロードする。
		LoadShader();
	}
	//デストラクタ。
	~TriangleDraw()
	{
		//頂点バッファを解放。
		if (m_vertexBuffer != nullptr) {
			m_vertexBuffer->Release();
		}
		//定数バッファを解放。
		if (m_constantBuffer != nullptr) {
			m_constantBuffer->Release();
		}
	}
	//シェーダーをロードする。
	void LoadShader()
	{
		//頂点シェーダーをロード。
		//第一引数はロードするシェーダーソースファイルのファイルパス。
		//第二引数はエントリーポイントとなる関数の名前。 
		//第三引数はロードしたシェーダーのタイプ。ここは頂点シェーダー。
		m_vsShader.Load(
			"Assets/shader/Tutorial.fx",
			"VS",						
			Shader::EnType::VS			
		);
		//ピクセルシェーダーをロード。
		m_psShader.Load(
			"Assets/shader/Tutorial.fx",
			"PS",
			Shader::EnType::PS
		);
	}
	//頂点バッファの初期化。
	void InitVertexBuffer()
	{
		//頂点バッファのソースデータ。
		SSimpleVertex vertices[] =
		{
			CVector4(0.0f, 100.0f, 0.0f, 1.0f),
			CVector4(100.0f, -100.0f, 0.0f, 1.0f),
			CVector4(-100.0f, -100.0f, 0.0f, 1.0f),
		};

		//頂点バッファの定義データを作成する。
		D3D11_BUFFER_DESC desc = {0};	//0クリア。
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(vertices);			//バッファの大きさ。
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	//どのバッファにバインドするかを指定する。
		desc.CPUAccessFlags = 0;					//CPUからアクセスするか決める。0ならしない。

		//頂点バッファのソースデータを指定する。
		D3D11_SUBRESOURCE_DATA InitData = {0};	//0で初期化。
		InitData.pSysMem = vertices;

		//頂点バッファをVRAM上に作成する。
		g_graphicsEngine->GetD3DDevice()->CreateBuffer(&desc, &InitData, &m_vertexBuffer);
	}
	void InitConstantBuffer()
	{
		//定数バッファの定義データを作成する。
		D3D11_BUFFER_DESC desc = { 0 };	//0クリア。
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(SVSConstantBuffer);		//バッファの大きさ。
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;	//どのバッファにバインドするかを指定する。
														//定数バッファの場合、ここが頂点バッファと変わる・。
		desc.CPUAccessFlags = 0;
		//定数バッファをVRAM上に作成する。
		g_graphicsEngine->GetD3DDevice()->CreateBuffer(&desc, nullptr, &m_constantBuffer);
	}
	//描画だってばよ！
	void Draw()
	{
		//デバイスコンテキストを取得。
		auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();
		//デバイスコンテキストに描画コマンドを積む。
		//ワールド・ビュー・プロジェクション行列を更新する。
		
		//まずはワールド行列。今回は単位行列にしとく。
		CMatrix mWorld = CMatrix::Identity();
		//続いてビュー行列。
		CMatrix mView;
		mView.MakeLookAt(
			{ 0.0f, 0.0f, 300.0f },		//視点
			{ 0.0f, 0.0f, 0.0f }	,	//注視点
			{ 0.0f, 1.0f, 0.0f }		//上方向
		);
		//最後にプロジェクション行列。
		CMatrix mProj;
		mProj.MakeProjectionMatrix(
			CMath::PI * 0.5f,					//画角
			FRAME_BUFFER_W / FRAME_BUFFER_H,	//アスペクト比
			0.1f,								//近平面までの距離
			1000.0f								//遠平面までの距離。
		);
		//全部の行列を合成する。
		SVSConstantBuffer cb;
		cb.mWVP.Mul(mWorld, mView);
		cb.mWVP.Mul(cb.mWVP, mProj);

		//VRAM上の定数バッファの内容を更新。
		deviceContext->UpdateSubresource(m_constantBuffer, 0, NULL, &cb, 0, 0);
		//頂点シェーダーで使用する定数バッファを設定。
		deviceContext->VSSetConstantBuffers(
			0,					//定数バッファをバインドするスロット番号(後述)
			1,					//設定するバッファの数。
			&m_constantBuffer	//設定する定数バッファ配列。
		);
		//頂点バッファを設定。
		unsigned int stride = sizeof(SSimpleVertex);	//頂点バッファのストライド
		unsigned int offset = 0;						//頂点バッファのオフセット。
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset );
		//プリミティブのトポロジーを設定。
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//頂点シェーダーを設定。
		deviceContext->VSSetShader(
			(ID3D11VertexShader*)m_vsShader.GetBody(),	//頂点シェーダー。
			NULL,										//NULLでいい。
			0											//0でいい。
		);
		//ピクセルシェーダーを設定。
		deviceContext->PSSetShader(
			(ID3D11PixelShader*)m_psShader.GetBody(),	//ピクセルシェーダー。
			NULL,										//NULLでいい。
			0											//0でいい。
		);
		//頂点レイアウトを設定。
		deviceContext->IASetInputLayout(m_vsShader.GetInputLayout());

		//ここまで設定した内容でドロー
		deviceContext->Draw(
			3,	//頂点数。
			0	//描画開始の頂点番号。大抵０。
		);
	}
};

///////////////////////////////////////////////////////////////////
// ウィンドウプログラムのメイン関数。
///////////////////////////////////////////////////////////////////
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//ゲームの初期化。
	InitGame(hInstance, hPrevInstance, lpCmdLine, nCmdShow, "Game");

	//TriangleDrawクラスの変数を用意する。
	TriangleDraw triangleDraw;

	//ゲームループ。
	while (DispatchWindowMessage() == true)
	{
		//描画開始。
		g_graphicsEngine->BegineRender();
		
		for (auto& pad : g_pad) {
			pad.Update();
		}
		//三角形を描画。
		triangleDraw.Draw();

		//描画終了。
		g_graphicsEngine->EndRender();
	}
}