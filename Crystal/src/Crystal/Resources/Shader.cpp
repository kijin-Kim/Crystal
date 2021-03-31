#include "cspch.h"
#include "Shader.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {
	Shader::Shader(const std::string& fileName)
	{
		CS_INFO("%s ���̴� �ҷ����� ��...", fileName.c_str());

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
		CS_FATAL(bIsValidShader, "%s ���̴��� �������� ã�� �� �����ϴ�", fileName.c_str());
		/* Crystal������ ������ ���̴��� �Ľ��� ��, ������ ���� prefix�� ����� �������� �䱸�մϴ�.
		* ���ؽ� ���̴� : vs
		* �� ���̴� : hs
		* ������ ���̴� : ds
		* ������Ʈ�� ���̴� : gs
		* �ȼ� ���̴� : ps
		* ��ǻƮ ���̴� : cs
		*/

		CS_INFO("%s ���̴� �ҷ����� �Ϸ�", fileName.c_str());
	}

	bool Shader::CheckInputValidation(const std::string& inputName, D3D_SHADER_INPUT_TYPE shaderInputType)
	{
		D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc = {};
		HRESULT hr = m_ShaderReflection->GetResourceBindingDescByName(inputName.c_str(), &shaderInputBindDesc);
		return SUCCEEDED(hr) && shaderInputBindDesc.Type == shaderInputType;
	}

	bool Shader::CheckInputValidation(const std::string& inputName, const std::string& typeName)
	{
		ID3D12ShaderReflectionVariable* shaderReflectionVariable = m_ShaderReflection->GetVariableByName(inputName.c_str());
		ID3D12ShaderReflectionType* shaderReflectionType = shaderReflectionVariable->GetType();
		D3D12_SHADER_TYPE_DESC shaderTypeDesc = {};
		shaderReflectionType->GetDesc(&shaderTypeDesc);

		return shaderTypeDesc.Name && shaderTypeDesc.Name == typeName;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::loadSourceFromFile(const std::string& filePath)
	{
		std::wstring shaderPath(filePath.begin(), filePath.end());
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode = nullptr;
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &shaderCode);
		CS_FATAL(SUCCEEDED(hr), "%s ���̴��� �������� �ʽ��ϴ�", filePath.c_str());
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
			CS_FATAL(false, "��ȿ���� ���� Shader Type �Դϴ�.");
			break;
		}

		entryPoint = staticShaderType + entryPoint;
		compilationTarget = staticShaderType + compilationTarget;

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DCompile(srcBlob->GetBufferPointer(), srcBlob->GetBufferSize(), nullptr, nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), compilationTarget.c_str(), compileFlag, 0, &m_ShaderDataBlobs[type], &errorBlob);

		if (errorBlob)
		{
			CS_WARN("%s", (char*)errorBlob->GetBufferPointer());
		}
		CS_FATAL(SUCCEEDED(hr), "���̴� �����Ͽ� �����Ͽ����ϴ�. %s", (char*)errorBlob->GetBufferPointer());

		hr = D3DReflect(m_ShaderDataBlobs[type]->GetBufferPointer(), m_ShaderDataBlobs[type]->GetBufferSize(), IID_PPV_ARGS(&m_ShaderReflection));
		CS_FATAL(SUCCEEDED(hr), "���̴� ���÷����� �����Ͽ����ϴ�.");
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
			CS_FATAL(false, "��ȿ���� ���� Shader Type�Դϴ�.");
			return false;
		}
	}
}