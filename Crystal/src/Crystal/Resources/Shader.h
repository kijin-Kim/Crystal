#pragma once
#include <map>
#include <wrl/client.h>
#include <set>
#include <d3d12shader.h>
#include "Crystal/GamePlay/Objects/Object.h"
#include "Crystal/Math/Math.h"

struct CD3DX12_STATIC_SAMPLER_DESC;

namespace Crystal {
	enum class ShaderType
	{
		None, Vertex, Hull, Domain, Geometry, Pixel, Compute
	};

	class InputLayout
	{
	public:
		InputLayout() = default;
		InputLayout(std::initializer_list<D3D12_INPUT_ELEMENT_DESC> inputLayout) : m_InputLayout(inputLayout) {}

		const D3D12_INPUT_ELEMENT_DESC* GetData() const noexcept { return m_InputLayout.data(); }
		UINT GetCount() noexcept { return m_InputLayout.size(); }
	private:
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;
	};

	struct RootParameter
	{
		int CbvCount = 0;
		int SrvCount = 0;
		int UavCount = 0;

		bool IsNull() const
		{
			return ((CbvCount == 0) && (SrvCount == 0) && (UavCount == 0));
		}
	};

	class RootSignature
	{
	public:
		RootSignature() = default;
		RootSignature(const RootParameter& perFrame, const RootParameter& perObject, const RootParameter& perExecute,
			std::initializer_list<CD3DX12_STATIC_SAMPLER_DESC> samplers = {});
		~RootSignature() = default;

		ID3D12RootSignature* GetData() const { return m_D3d12RootSignature.Get(); }

		bool HasPerFrameParameter() const { return !m_PerFrame.IsNull(); }
		bool HasPerObjectParameter() const { return !m_PerObject.IsNull(); }
		bool HasPerExecuteParameter() const { return !m_PerExecute.IsNull(); }

		int GetPerFrameDescriptorCount() const { return m_PerFrame.CbvCount + m_PerFrame.SrvCount + m_PerFrame.UavCount; }
		int GetPerObjectDescriptorCount() const { return m_PerObject.CbvCount + m_PerObject.SrvCount + m_PerObject.UavCount; }
		int GetPerExecuteDescriptorCount() const { return m_PerExecute.CbvCount + m_PerExecute.SrvCount + m_PerExecute.UavCount; }

		int GetPerFrameParameterIndex() const { return !m_PerFrame.IsNull() - 1; }
		int GetPerObjectParameterIndex() const { return !m_PerFrame.IsNull() + !m_PerObject.IsNull() - 1; }
		int GetPerExecuteParameterIndex() const { return !m_PerFrame.IsNull() + !m_PerObject.IsNull() + !m_PerExecute.IsNull() - 1; }



	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3d12RootSignature = nullptr;

		RootParameter m_PerFrame = {};
		RootParameter m_PerObject = {};
		RootParameter m_PerExecute = {};
	};


	class Shader : public Object
	{
		friend class ShaderManager;
	public:
		Shader(const std::string& fileName);
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>>& GetRaw() const { return m_ShaderDataBlobs; }

		bool CheckInputValidation(const std::string& inputName, D3D_SHADER_INPUT_TYPE shaderInputType);
		bool CheckInputValidation(const std::string& inputName, const std::string& typeName);

		void SetRootSignature(RootSignature rootsig) { m_RootSignature = rootsig; }
		const RootSignature& GetRootSignature() const { return m_RootSignature; }

		//== For Render Shader =======
		void SetInputLayout(InputLayout inputLayout) { m_InputLayout = inputLayout; }
		const InputLayout& GetInputLayout() const { return m_InputLayout; }

		void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) { m_PrimitiveTopologyType = type; }
		D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType() const { return m_PrimitiveTopologyType; }

		//== For Compute Shader =======
		void SetDispatchThreadGroupCounts(const DirectX::XMFLOAT3& count) { m_DispatchThreadGroupCount = count; }
		const DirectX::XMFLOAT3& GetDispatchThreadGroupCounts() const  { return m_DispatchThreadGroupCount; }
		
		

		bool HasShader(ShaderType type) { return m_ShaderDataBlobs.count(type) != 0; }

		STATIC_TYPE_IMPLE(Shader)

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> loadSourceFromFile(const std::string& filePath);
		void compileShader(Microsoft::WRL::ComPtr<ID3DBlob>& srcblob, ShaderType type);
		bool canCompile(ShaderType type, const std::string& src);

	private:
		mutable std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> m_ShaderDataBlobs;
		std::string m_Name;
		ShaderType m_Type = ShaderType::None;

		InputLayout m_InputLayout = {};
		RootSignature m_RootSignature = {};

		D3D12_PRIMITIVE_TOPOLOGY_TYPE m_PrimitiveTopologyType;

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> m_ShaderReflection = nullptr;

		DirectX::XMFLOAT3 m_DispatchThreadGroupCount = Vector3::Zero;
	};
}
