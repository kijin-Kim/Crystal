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
		// CS_FATAL(SUCCEEDED(hr), "Root Signature?? ?©ª???????? ????????????");
		auto device = Device::Instance().GetD3DDevice();
		hr = device->CreateRootSignature(0, rootSignatureDataBlob->GetBufferPointer(),
			rootSignatureDataBlob->GetBufferSize(), IID_PPV_ARGS(&m_D3d12RootSignature));
		// CS_FATAL(SUCCEEDED(hr), "Root Signature?? ???????? ????????????");
	}






	Shader::Shader(const std::string& fileName)
	{
		// CS_INFO("%s ????? ??????? ??...", fileName.c_str());

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
		// CS_FATAL(bIsValidShader, "%s ??????? ???????? ??? ?? ???????", fileName.c_str());
		/* Crystal?????? ?????? ??????? ????? ??, ?????? ???? prefix?? ????? ???????? ??????.
		* ????? ????? : vs
		* ?? ????? : hs
		* ?????? ????? : ds
		* ????????? ????? : gs
		* ??? ????? : ps
		* ???? ????? : cs
		*/

		// CS_INFO("%s ????? ??????? ???", fileName.c_str());
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
		// CS_FATAL(SUCCEEDED(hr), "%s ??????? ???????? ??????", filePath.c_str());
		return shaderCode;
	}

	void Shader::compileShader(Microsoft::WRL::ComPtr<ID3DBlob>& srcBlob, ShaderType type)
	{
		std::string entryPoint = "Main";
		std::string compilationTarget = "_5_1";
		std::string staticShaderType;

		UINT compileFlag = 0;
#ifdef  CS_DEBUG
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
			// CS_FATAL(false, "??????? ???? Shader Type ????.");
			break;
		}

		entryPoint = staticShaderType + entryPoint;
		compilationTarget = staticShaderType + compilationTarget;

		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3DCompile(srcBlob->GetBufferPointer(), srcBlob->GetBufferSize(), nullptr, nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), compilationTarget.c_str(), compileFlag, 0, &m_ShaderDataBlobs[type], &errorBlob);

		if (errorBlob)
		{
			// CS_WARN("%s", (char*)errorBlob->GetBufferPointer());
		}
		// CS_FATAL(SUCCEEDED(hr), "????? ??????? ????????????. %s", (char*)errorBlob->GetBufferPointer());

		hr = D3DReflect(m_ShaderDataBlobs[type]->GetBufferPointer(), m_ShaderDataBlobs[type]->GetBufferSize(), IID_PPV_ARGS(&m_ShaderReflection));
		// CS_FATAL(SUCCEEDED(hr), "????? ???¡À????? ????????????.");
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
			// CS_FATAL(false, "??????? ???? Shader Type????.");
			return false;
		}
	}
}