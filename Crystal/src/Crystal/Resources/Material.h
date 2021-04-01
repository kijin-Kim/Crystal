#pragma once
#include <memory>
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {

	class Material : public Object
	{
	public:
		Material(const std::shared_ptr<Shader>& shader) : m_Shader(shader) {}
		~Material() = default;

		void Set(const std::string& materialName, std::weak_ptr<Texture> texture);

		void Set(const std::string& materialName, const DirectX::XMFLOAT4& value);
		void Set(const std::string& materialName, const DirectX::XMFLOAT3& value);
		void Set(const std::string& materialName, const DirectX::XMFLOAT2& value);
		void Set(const std::string& materialName, float value);

		void Set(const std::string& materialName, const DirectX::XMINT4& value);
		void Set(const std::string& materialName, const DirectX::XMINT3& value);
		void Set(const std::string& materialName, const DirectX::XMINT2& value);
		void Set(const std::string& materialName, int value);

		void Set(const std::string& materialName, bool value);

		bool HasTextureInput(const std::string& inputName) const 
		{ 
			return (m_TextureInputs.count(inputName) != 0 && !m_TextureInputs[inputName].expired());
		}
		bool HasFloatInput(const std::string& inputName) const { return m_FloatInputs.count(inputName) != 0; }
		bool HasIntInput(const std::string& inputName) const { return m_IntInputs.count(inputName) != 0; }
		bool HasBoolInput(const std::string& inputName) const { return m_BoolInputs.count(inputName) != 0; }

		std::shared_ptr<Texture> GetTextureInput(const std::string& textureName) const;
		const DirectX::XMFLOAT4& GetFloatInput(const std::string& floatName) const;
		const DirectX::XMINT4& GetIntInput(const std::string& intName) const;
		bool GetBoolInput(const std::string& boolName) const;

		Shader* GetShader() const { return m_Shader.get(); }

		STATIC_TYPE_IMPLE(Material)
	private:
		using TextureInputs = std::map<std::string, std::weak_ptr<Texture>>;
		using FloatInputs = std::map<std::string, DirectX::XMFLOAT4>;
		using IntInputs = std::map<std::string, DirectX::XMINT4>;
		using BoolInputs = std::map<std::string, bool>;

		std::shared_ptr<Shader> m_Shader = nullptr;

		mutable TextureInputs m_TextureInputs;
		FloatInputs m_FloatInputs;
		IntInputs m_IntInputs;
		BoolInputs m_BoolInputs;
	};
}