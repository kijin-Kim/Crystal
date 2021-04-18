#include "cspch.h"
#include "Shader.h"

#include "Crystal/Core/Device.h"
#include "Crystal/Renderer/CommandQueue.h"


namespace Crystal {


	RootSignature::RootSignature(const RootParameter& perFrame, const RootParameter& perObject, const RootParameter& perExecute,
		std::initializer_list<CD3DX12_STATIC_SAMPLER_DESC> samplers) 
		: m_PerFrame(perFrame), m_PerObject(perObject), m_PerExecute(perExecute)
	{
		int cbvRegister = 0;
		int srvRegister = 0;
		int uavRegister = 0;


		std::vector<CD3DX12_ROOT_PARAMETER1> rootParameters;

		//=========== Per Frame ======================================================
		std::vector<CD3DX12_DESCRIPTOR_RANGE1> perFrameRanges;
		if (perFrame.CbvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, perFrame.CbvCount, cbvRegister);
			perFrameRanges.push_back(range);

			cbvRegister += perFrame.CbvCount;
		}

		if (perFrame.SrvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, perFrame.SrvCount, srvRegister,
				0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
			perFrameRanges.push_back(range);

			srvRegister += perFrame.SrvCount;
		}

		if (perFrame.UavCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, perFrame.UavCount, uavRegister);
			perFrameRanges.push_back(range);

			uavRegister += perFrame.UavCount;
		}

		CD3DX12_ROOT_PARAMETER1 rootParameter = {};
		if (!perFrameRanges.empty())
		{
			rootParameter.InitAsDescriptorTable(perFrameRanges.size(), perFrameRanges.data());
			rootParameters.push_back(rootParameter);
		}
		
		//=========== per Object ======================================================

		std::vector<CD3DX12_DESCRIPTOR_RANGE1> perObjectRanges;
		if (perObject.CbvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, perObject.CbvCount, cbvRegister);
			perObjectRanges.push_back(range);

			cbvRegister += perObject.CbvCount;
		}

		if (perObject.SrvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, perObject.SrvCount, srvRegister,
				0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
			perObjectRanges.push_back(range);

			srvRegister += perObject.SrvCount;
		}

		if (perObject.UavCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, perObject.UavCount, uavRegister);
			perObjectRanges.push_back(range);

			uavRegister += perObject.UavCount;
		}

		rootParameter = {};
		if (!perObjectRanges.empty())
		{
			rootParameter.InitAsDescriptorTable(perObjectRanges.size(), perObjectRanges.data());
			rootParameters.push_back(rootParameter);
		}

		//=========== per Execute ======================================================

		std::vector<CD3DX12_DESCRIPTOR_RANGE1> perExecuteRanges;
		if (perExecute.CbvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, perExecute.CbvCount, cbvRegister);
			perExecuteRanges.push_back(range);

			cbvRegister += perExecute.CbvCount;
		}

		if (perExecute.SrvCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, perExecute.SrvCount, srvRegister,
				0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
			perExecuteRanges.push_back(range);

			srvRegister += perExecute.SrvCount;
		}

		if (perExecute.UavCount > 0)
		{
			CD3DX12_DESCRIPTOR_RANGE1 range = {};
			range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, perExecute.UavCount, uavRegister);
			perExecuteRanges.push_back(range);

			uavRegister += perExecute.UavCount;
		}

		rootParameter = {};
		if (!perExecuteRanges.empty())
		{
			rootParameter.InitAsDescriptorTable(perExecuteRanges.size(), perExecuteRanges.data());
			rootParameters.push_back(rootParameter);
		}

		//===========================================================================

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc = {};
		rootSigDesc.Init_1_1(rootParameters.size(), rootParameters.data(), samplers.size(), samplers.begin(),
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureDataBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> rootSignatureErrorBlob = nullptr;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSigDesc, &rootSignatureDataBlob, &rootSignatureErrorBlob);
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 시리얼화하는데 실패하였습니다");
		auto device = Device::Instance().GetD3DDevice();
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
			rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_D3d12RootSignature));
		CS_FATAL(SUCCEEDED(hr), "Root Signature를 생성하는데 실패하였습니다");
	}






	Shader::Shader(const std::string& fileName)
	{
		CS_INFO("%s 셰이더 불러오는 중...", fileName.c_str());

		Microsoft::WRL::ComPtr<ID3DBlob> srcBlob = loadSourceFromFile(fileName);
		const std::string srcString = (char*)srcBlob->GetBufferPointer();

		bool bIsValidShader = false;
		if (canCompile(ShaderType::Vertex, srcString))
		{
			compileShader(srcBlob, ShaderType::Vertex);
			bIsValidShader = true;
		}
		if (canCompile(ShaderType::Hull, srcString))
		{
			compileShader(srcBlob, ShaderType::Hull);
			bIsValidShader = true;
		}
		if (canCompile(ShaderType::Domain, srcString))
		{
			compileShader(srcBlob, ShaderType::Domain);
			bIsValidShader = true;
		}
		if (canCompile(ShaderType::Geometry, srcString))
		{
			compileShader(srcBlob, ShaderType::Geometry);
			bIsValidShader = true;
		}
		if (canCompile(ShaderType::Pixel, srcString))
		{
			compileShader(srcBlob, ShaderType::Pixel);
			bIsValidShader = true;
		}
		if (canCompile(ShaderType::Compute, srcString))
		{
			compileShader(srcBlob, ShaderType::Compute);
			bIsValidShader = true;
		}
		CS_FATAL(bIsValidShader, "%s 셰이더의 진입점을 찾을 수 없습니다", fileName.c_str());
		/* Crystal에서는 각각의 셰이더를 파싱할 때, 다음과 같은 prefix를 사용한 진입점을 요구합니다.
		* 버텍스 셰이더 : vs
		* 헐 셰이더 : hs
		* 도메인 셰이더 : ds
		* 지오메트리 셰이더 : gs
		* 픽셀 셰이더 : ps
		* 컴퓨트 셰이더 : cs
		*/

		CS_INFO("%s 셰이더 불러오기 완료", fileName.c_str());
	}

	bool Shader::CheckInputValidation(const std::string& inputName, D3D_SHADER_INPUT_TYPE shaderInputType)
	{
		/*D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc = {};
		HRESULT hr = m_ShaderReflection->GetResourceBindingDescByName(inputName.c_str(), &shaderInputBindDesc);
		return SUCCEEDED(hr) && shaderInputBindDesc.Type == shaderInputType;*/

		return true;
	}

	bool Shader::CheckInputValidation(const std::string& inputName, const std::string& typeName)
	{
		/*ID3D12ShaderReflectionVariable* shaderReflectionVariable = m_ShaderReflection->GetVariableByName(inputName.c_str());
		ID3D12ShaderReflectionType* shaderReflectionType = shaderReflectionVariable->GetType();
		D3D12_SHADER_TYPE_DESC shaderTypeDesc = {};
		shaderReflectionType->GetDesc(&shaderTypeDesc);

		return shaderTypeDesc.Name && shaderTypeDesc.Name == typeName;*/

		return true;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> Shader::loadSourceFromFile(const std::string& filePath)
	{
		std::wstring shaderPath(filePath.begin(), filePath.end());
		Microsoft::WRL::ComPtr<ID3DBlob> shaderCode = nullptr;
		HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), &shaderCode);
		CS_FATAL(SUCCEEDED(hr), "%s 셰이더가 존재하지 않습니다", filePath.c_str());
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
			CS_FATAL(false, "유효하지 않은 Shader Type 입니다.");
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
		CS_FATAL(SUCCEEDED(hr), "셰이더 컴파일에 실패하였습니다. %s", (char*)errorBlob->GetBufferPointer());

		hr = D3DReflect(m_ShaderDataBlobs[type]->GetBufferPointer(), m_ShaderDataBlobs[type]->GetBufferSize(), IID_PPV_ARGS(&m_ShaderReflection));
		CS_FATAL(SUCCEEDED(hr), "셰이더 리플렉션을 실패하였습니다.");
	}

	bool Shader::canCompile(ShaderType type, const std::string& src)
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
			CS_FATAL(false, "유효하지 않은 Shader Type입니다.");
			return false;
		}
	}
}