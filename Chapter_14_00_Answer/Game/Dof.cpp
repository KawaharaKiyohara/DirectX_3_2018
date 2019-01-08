#include "stdafx.h"
#include "Dof.h"
#include "Game.h"


Dof::~Dof()
{
	if (m_finalBlendState != nullptr) {
		m_finalBlendState->Release();
	}
}
void Dof::Init()
{
	//ガウシアンブラーの初期化。
	m_bokeBlur.Init(g_game->GetMainRenderTarget()->GetRenderTargetSRV(), 5.0);

	m_vs.Load("Assets/shader/dof.fx", "VSMain", Shader::EnType::VS);
	m_psFinal.Load("Assets/shader/dof.fx", "PSFinal", Shader::EnType::PS);

	//半透明合成のブレンドステートを作成する。
	CD3D11_DEFAULT defaultSettings;
	//デフォルトセッティングで初期化する。
	CD3D11_BLEND_DESC blendDesc(defaultSettings);
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	auto device = g_graphicsEngine->GetD3DDevice();

	device->CreateBlendState(&blendDesc, &m_finalBlendState);
}
/// <summary>
/// この関数はPostEffect::Drawから呼ばれているよ。
/// </summary>
/// <param name="postEffect"></param>
void Dof::Draw(PostEffect& postEffect)
{
	//メインレンダリングターゲットを取得。
	auto mainRT = g_game->GetMainRenderTarget();
	auto deviceContext = g_graphicsEngine->GetD3DDeviceContext();

	//描画ステップ_1
	//シーンにブラーをかけていく。
	{
		m_bokeBlur.Execute(postEffect);
	}
	//描画ステップ_2
	//ステップ２で抽出した画像を、メインレンダリングターゲットに
	//書き込んでいく。ボケない箇所はピクセルキルする。
	{
		auto mainRT = g_game->GetMainRenderTarget();
		g_graphicsEngine->ChangeRenderTarget(mainRT, mainRT->GetViewport());

		//ボケ画像のアドレスをt0レジスタに設定する。
		auto srv = m_bokeBlur.GetResultTextureSRV();
		deviceContext->PSSetShaderResources(0, 1, &srv);
		//深度テクスチャをt1レジスタに設定する。
		auto depthTexSrv = g_game->GetDepthInViewRenderTarget()->GetRenderTargetSRV();
		deviceContext->PSSetShaderResources(1, 1, &depthTexSrv);

		ID3D11BlendState* oldBlendState;
		float oldBlendFactor[4];
		UINT oldMask;
		deviceContext->OMGetBlendState(&oldBlendState, oldBlendFactor, &oldMask);
		//半透明合成用のブレンディングステートを設定する。
		float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		deviceContext->OMSetBlendState(m_finalBlendState, blendFactor, 0xffffffff);
		//フルスクリーン描画。
		postEffect.DrawFullScreenQuadPrimitive(deviceContext, m_vs, m_psFinal);

		//ブレンドステートを戻す。
		deviceContext->OMSetBlendState(oldBlendState, oldBlendFactor, oldMask);
		oldBlendState->Release();

	}
}