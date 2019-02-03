#include "stdafx.h"
#include "ConstantBufferGPU.h"


ConstantBufferGPU::~ConstantBufferGPU()
{
	if (m_constantBuffer != nullptr) {
		m_constantBuffer->Release();
	}
}

void ConstantBufferGPU::Create(const void* pInitData, int bufferSize)
{
	//作成する定数バッファの情報を設定する。
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = (((bufferSize - 1) / 16) + 1) * 16;	//16バイトアライメントに切りあげる。
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;

	auto d3dDevice = g_graphicsEngine->GetD3DDevice();
	if (pInitData != nullptr) {
		//初期化データが指定されている。
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		d3dDevice->CreateBuffer(&bufferDesc, &InitData, &m_constantBuffer);
	}
	else {
		d3dDevice->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
	}
}