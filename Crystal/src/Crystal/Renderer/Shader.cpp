#include "cspch.h"
#include "Shader.h"
#include "Renderer.h"

namespace Crystal {
	Shader::Shader(const std::string& fileName)
	{
		// assumed we have only vertex and pixel shader
		loadFromFile(fileName, ShaderType::Vertex);
		loadFromFile(fileName, ShaderType::Pixel);
	}

	void Shader::loadFromFile(const std::string& filePath, ShaderType type)
	{
		std::wstring shaderPath(filePath.begin(), filePath.end());
		std::string entryPoint = "Main";
		std::string compilationTarget = "_5_1";
		std::string staticShaderType;

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
			CS_ASSERT(false, "현재 타입의 셰이더는 지원하지 않습니다");
			break;
		}

		entryPoint = staticShaderType + entryPoint;
		compilationTarget = staticShaderType + compilationTarget;

		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode = nullptr;
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &shaderCode);
		
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		hr = D3DCompile(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), nullptr, nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), compilationTarget.c_str(), compileFlag, 0, &m_ShaderDataBlobs[type], &errorBlob);

		if (errorBlob)
		{
			CS_ERROR("셰이더 컴파일 에러 : %s ", (char*)errorBlob->GetBufferPointer());
		}
		CS_ASSERT(SUCCEEDED(hr), "셰이더 컴파일에 실패하였습니다");


		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> shaderReflection = nullptr;
		hr = D3DReflect(m_ShaderDataBlobs[type]->GetBufferPointer(), m_ShaderDataBlobs[type]->GetBufferSize(), IID_PPV_ARGS(&shaderReflection));
		CS_ASSERT(SUCCEEDED(hr), "셰이더 리플렉션을 실패하였습니다.");

	
	
		D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc = {};

		hr = S_OK;
		for (int i = 0; ; i++)
		{
			hr = shaderReflection->GetResourceBindingDesc(i, &shaderInputBindDesc);
			if (FAILED(hr))
				break;
			auto it = m_ShaderInputInfos.emplace(shaderInputBindDesc.Name, shaderInputBindDesc.Type);
			CS_ASSERT(it.second, "셰이더 리플렉션 실패!");
		}
		
		

	}
}