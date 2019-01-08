#pragma once
/// <summary>
/// GPU上の定数バッファ。
/// </summary>
class ConstantBufferGPU
{
public:
	/// <summary>
	/// デストラクタ。
	/// </summary>
	~ConstantBufferGPU();
	/// <summary>
	/// VRAM上に定数バッファを作成する。
	/// </summary>
	/// <param name="pInitData">
	/// 初期化データ。
	/// nullptrを指定した場合は、未初期化の定数バッファが作成されます。
	/// </param>
	/// <param name="bufferSize">
	/// 作成するバッファのサイズ。
	/// この関数内で16バイトアライメントに切り上げされます。
	/// </param>
	void Create(const void* pInitData, int bufferSize);
	/// <summary>
	/// 生定数バッファを取得。
	/// </summary>
	/// <returns></returns>
	ID3D11Buffer* GetD3D11Buffer() const
	{
		return m_constantBuffer;
	}

private:
	ID3D11Buffer*		m_constantBuffer = nullptr;	//!<GPUBuffe上の定数バッファ。

};

