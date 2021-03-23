#pragma once
#include <memory>
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"

namespace Crystal {

	// TODO : Material 단위로 텍스쳐를 로딩

	class Material final
	{
	public:
		Material(std::shared_ptr<Shader> shader) : m_Shader(std::move(shader)) {}
		~Material() = default;

		void Set(const std::string& materialName, std::shared_ptr<Texture> texture);


		void Set(const std::string& materialName, const DirectX::XMFLOAT4& value);
		void Set(const std::string& materialName, const DirectX::XMFLOAT3& value);
		void Set(const std::string& materialName, const DirectX::XMFLOAT2& value);
		void Set(const std::string& materialName, float value);

		void Set(const std::string& materialName, const DirectX::XMINT4& value);
		void Set(const std::string& materialName, const DirectX::XMINT3& value);
		void Set(const std::string& materialName, const DirectX::XMINT2& value);
		void Set(const std::string& materialName, int value);

		void Set(const std::string& materialName, bool value);



		bool HasTextureInput(const std::string& inputName) const { return m_TextureInputs.count(inputName) != 0; }
		bool HasFloatInput(const std::string& inputName) const { return m_FloatInputs.count(inputName) != 0; }
		bool HasIntInput(const std::string& inputName) const { return m_IntInputs.count(inputName) != 0; }
		bool HasBoolInput(const std::string& inputName) const { return m_BoolInputs.count(inputName) != 0; }

		Texture* GetTextureInput(const std::string& textureName) const;
		const DirectX::XMFLOAT4& GetFloatInput(const std::string& floatName) const;
		const DirectX::XMINT4& GetIntInput(const std::string& intName) const;
		bool GetBoolInput(const std::string& boolName) const;

		Shader* GetShader() const { return m_Shader.get(); }

	private:
		using TextureInputs = std::map<std::string, std::shared_ptr<Texture>>;
		using FloatInputs = std::map<std::string, DirectX::XMFLOAT4>;
		using IntInputs = std::map<std::string, DirectX::XMINT4>;
		using BoolInputs = std::map<std::string, bool>;

		std::shared_ptr<Shader> m_Shader = nullptr;

		TextureInputs m_TextureInputs;
		FloatInputs m_FloatInputs;
		IntInputs m_IntInputs;
		BoolInputs m_BoolInputs;
	};

}