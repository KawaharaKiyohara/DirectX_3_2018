/*!
 * @brief	シェーダー。
 */
#include "stdafx.h"
#include "graphics/Shader.h"
#include "graphics/ShaderResources.h"


Shader::Shader()
{
}
Shader::~Shader()
{
	Release();
}

void Shader::Release()
{
}

bool Shader::Load(const char* filePath, const char* entryFuncName, EnType shaderType)
{
	Release();
	bool result = ShaderResources::GetInstance().Load(
		m_shader,
		m_inputLayout,
		m_blobOut,
		filePath,
		entryFuncName,
		shaderType
	);
    return result;
}
