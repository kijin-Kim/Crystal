#pragma once
#include <map>
#include <wrl/client.h>

#include "RootSignature.h"

namespace Crystal {

	class RenderComponent;

	enum class ShaderType
	{
		None, Vertex, Pixel
	};

	class Shader
	{
		friend class ShaderLibrary;
	public:
		Shader(const std::string& fileName);
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		std::map< ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>>& GetRaw() const { return m_ShaderDataBlobs; }
		const std::vector<RenderComponent*>& GetRenderComponents() const { return m_RenderComponents; }
	private:
		void loadFromFile(const std::string& filePath, ShaderType type);


	private:
		mutable std::map<ShaderType, Microsoft::WRL::ComPtr<ID3DBlob>> m_ShaderDataBlobs;
		std::string m_Name;
		ShaderType m_Type = ShaderType::None;

		std::vector<RenderComponent*> m_RenderComponents;
	};

}


