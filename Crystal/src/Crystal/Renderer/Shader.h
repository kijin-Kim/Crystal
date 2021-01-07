#pragma once
#include <map>
#include <wrl/client.h>
#include <set>


namespace Crystal {

	enum class ShaderType
	{
		None, Vertex, Pixel
	};

	struct ShaderInputInfo
	{
		ShaderInputInfo(const std::string& name, D3D_SHADER_INPUT_TYPE type) : Name(std::move(name)), Type(type) {}
		std::string Name;
		D3D_SHADER_INPUT_TYPE Type;
	};

	struct ShaderInputInfoCompare
	{
		bool operator() (const ShaderInputInfo& lhs, const ShaderInputInfo& rhs) const
		{
			if (lhs.Name == rhs.Name)
			{
				return lhs.Type < rhs.Type;
			}
			return lhs.Name < rhs.Name;
		}
	};

	class Shader final
	{
		friend class ShaderManager;
	public:
		Shader(const std::string& fileName);
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>>& GetRaw() const { return m_ShaderDataBlobs; }

		bool CheckInputValidation(const ShaderInputInfo& Info) { return m_ShaderInputInfos.find(Info) != m_ShaderInputInfos.end(); }
	private:
		void loadFromFile(const std::string& filePath, ShaderType type);

	private:
		mutable std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> m_ShaderDataBlobs;
		std::set<ShaderInputInfo, ShaderInputInfoCompare> m_ShaderInputInfos;
		std::string m_Name;
		ShaderType m_Type = ShaderType::None;
	};
}
