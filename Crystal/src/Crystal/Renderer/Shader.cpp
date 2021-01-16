#include "cspch.h"
#include "Shader.h"
#include "Renderer.h"

namespace Crystal {
	Shader::Shader(const std::string& fileName)
	{
		Microsoft::WRL::ComPtr<ID3DBlob> srcBlob = loadSourceFromFile(fileName);
		const std::string srcString = (char*)srcBlob->GetBufferPointer();

		bool bIsValidShader = false;
		if (hasShader(ShaderType::Vertex, srcString))
		{
			compileShader(srcBlob, ShaderType::Vertex);
			bIsValidShader = true;
		}
		if (hasShader(ShaderType::Hull, srcString))
		{
			compileShader(srcBlob, ShaderType::Hull);
			bIsValidShader = true;
		}
		if (hasShader(ShaderType::Domain, srcString))
		{
			compileShader(srcBlob, ShaderType::Domain);
			bIsValidShader = true;
		}
		if (hasShader(ShaderType::Geometry, srcString))
		{
			compileShader(srcBlob, ShaderType::Geometry);
			bIsValidShader = true;
		}
		if (hasShader(ShaderType::Pixel, srcString))
		{
			compileShader(srcBlob, ShaderType::Pixel);
			bIsValidShader = true;
		}
		if (hasShader(ShaderType::Compute, srcString))
		{
			compileShader(srcBlob, ShaderType::Compute);
			bIsValidShader = true;
		}

		CS_ASSERT(bIsValidShader, "%s 셰이더가 존재하지 않거나 진입점이 존재하지않습니다.", fileName.c_str());
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::loadSourceFromFile(const std::string& filePath)
	{
		std::wstring shaderPath(filePath.begin(), filePath.end());
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode = nullptr;
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &shaderCode);

		return shaderCode;
	}

	void Shader::compileShader(Microsoft::WRL::ComPtr<ID3DBlob>& srcBlob, ShaderType type)
	{
		std::string entryPoint = "Main";
		std::string compilationTarget = "_5_1";
		std::string staticShaderType;

		UINT compileFlag = 0;
#ifdef CS_DEBUG
		compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_STRICTNESS;
#endif

		switch (type)
		{
		case ShaderType::Vertex:
			staticShaderType = "vs";
			break;
		case ShaderType::Hull:
			staticShaderType = "hs";
			break;
		case ShaderType::Domain:
			staticShaderType = "ds";
			break;
		case ShaderType::Geometry:
			staticShaderType = "gs";
			break;
		case ShaderType::Pixel:
			staticShaderType = "ps";
			break;
		case ShaderType::Compute:
			staticShaderType = "cs";
			break;
		default:
			CS_ASSERT(false, "유효하지 않은 Shader Type 입니다.");
			break;
		}

		entryPoint = staticShaderType + entryPoint;
		compilationTarget = staticShaderType + compilationTarget;

		
		
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DCompile(srcBlob->GetBufferPointer(), srcBlob->GetBufferSize(), nullptr, nullptr,
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
			m_ShaderInputInfos.emplace(shaderInputBindDesc.Name, shaderInputBindDesc.Type);
		}
		
		

	}

	bool Shader::hasShader(ShaderType type, const std::string& src)
	{
		switch (type)
		{
		case ShaderType::Vertex:
			return src.find("vsMain") != std::string::npos;
		case ShaderType::Hull:
			return src.find("hsMain") != std::string::npos;
		case ShaderType::Domain:
			return src.find("dsMain") != std::string::npos;
		case ShaderType::Geometry:
			return src.find("gsMain") != std::string::npos;
		case ShaderType::Pixel:
			return src.find("psMain") != std::string::npos;
		case ShaderType::Compute:
			return src.find("csMain") != std::string::npos;
		default:
			CS_ASSERT(false, "유효하지 않은 Shader Type입니다.");
			return false;
		}
	}

}