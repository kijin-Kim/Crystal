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
	private:
		Microsoft::WRL::ComPtr<ID3DBlob> loadSourceFromFile(const std::string& filePath);
		void compileShader(Microsoft::WRL::ComPtr<ID3DBlob>& srcblob, ShaderType type);
		bool hasShader(ShaderType type, const std::string& src);

	private:
		mutable std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> m_ShaderDataBlobs;
		std::string m_Name;
		ShaderType m_Type = ShaderType::None;

		Microsoft::WRL::ComPtr<ID3D12ShaderReflection> m_ShaderReflection = nullptr;
	};
}
