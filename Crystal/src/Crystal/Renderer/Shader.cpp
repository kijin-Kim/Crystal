#include "cspch.h"
#include "Shader.h"
#include "Renderer.h"

namespace Crystal {

	Shader::Shader(const std::string& fileName)
	{
		// assumed we have only vertex and pixel shader
		std::string filePathVS = fileName + "_VS.hlsl";
		std::string filePathPS = fileName + "_PS.hlsl";
		loadFromFile(filePathVS, ShaderType::Vertex);
		loadFromFile(filePathPS, ShaderType::Pixel);
	}

	void Shader::loadFromFile(const std::string& filePath, ShaderType type)
	{
		std::wstring shaderPath(filePath.begin(), filePath.end());
		std::string entryPoint = "Main";
		std::string compilationTarget = "_5_1";
		std::string staticShaderType = "";

		UINT compileFlag = 0;
#ifdef CS_DEBUG
		compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		switch (type)
		{
		case Crystal::ShaderType::Vertex:
			staticShaderType = "vs";
			break;
		case Crystal::ShaderType::Pixel:
			staticShaderType = "ps";
			break;
		default:
			CS_ASSERT(false, "���� Ÿ���� ���̴��� �������� �ʽ��ϴ�");
			break;
		}

		entryPoint = staticShaderType + entryPoint;
		compilationTarget = staticShaderType + compilationTarget;

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entryPoint.c_str(), compilationTarget.c_str(), compileFlag, 0, &m_ShaderDataBlobs[type], &errorBlob);
		if (errorBlob)
		{
			CS_ERROR("���̴� ������ ���� : %s ", (char*)errorBlob->GetBufferPointer());
		}
		CS_ASSERT(SUCCEEDED(hr), "���̴� �����Ͽ� �����Ͽ����ϴ�");
	}

}