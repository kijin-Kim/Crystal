#pragma once
#include <map>
#include <wrl/client.h>
#include <set>
#include <d3d12shader.h>

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

		const D3D12_INPUT_ELEMENT_DESC* GetData() const { return m_InputLayout.data(); }
		UINT GetCount() { return m_InputLayout.size(); }
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
		RootSignature(const RootParameter& PerFrame, const RootParameter& PerObject, const RootParameter& PerDraw);
		~RootSignature() = default;

		ID3D12RootSignature* GetData() const { return m_D3d12RootSignature.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_D3d12RootSignature = nullptr;
	};


	class Shader final
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

		void SetInputLayout(InputLayout inputLayout) { m_InputLayout = inputLayout; }
		const InputLayout& GetInputLayout() const { return m_InputLayout; }

	private:
		Microsoft::WRL::ComPtr<ID3DBlob> loadSourceFromFile(const std::string& filePath);
		void compileShader(Microsoft::WRL::ComPtr<ID3DBlob>& srcblob, ShaderType type);
		bool hasShader(ShaderType type, const std::string& src);

	private:
		mutable std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> m_ShaderDataBlobs;
		std::string m_Name;
		ShaderType m_Type = ShaderType::None;

		InputLayout m_InputLayout = {};
		RootSignature m_RootSignature = {};

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> m_ShaderReflection = nullptr;
	};
}
