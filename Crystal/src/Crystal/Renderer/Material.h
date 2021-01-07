#pragma once
#include <memory>
#include "Crystal/AssetManager/Texture.h"
#include "Shader.h"

namespace Crystal {

	// TODO : Material 단위로 텍스쳐를 로딩

	class Material final
	{
	public:
		Material(std::shared_ptr<Shader> shader) : m_Shader(std::move(shader)) {}
		~Material() = default;

		void Set(const std::string& inputBindName, std::shared_ptr<Texture> texture);
		/*void Set(const std::string& inputBindName, const DirectX::XMFLOAT3& value) {}*/

		Texture* GetTextureInput(const std::string& inputBindName);

	private:
		using TextureInputs = std::map<std::string, std::shared_ptr<Texture>>;

		std::shared_ptr<Shader> m_Shader = nullptr;
		TextureInputs m_TextureInputs;
	};

}