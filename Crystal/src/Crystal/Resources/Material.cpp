#include "cspch.h"
#include "Material.h"

namespace Crystal {
	void Material::Set(const std::string& materialName, std::weak_ptr<Texture> texture)
	{
		if (!m_Shader->CheckInputValidation(materialName, D3D_SIT_TEXTURE))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_TextureInputs.find(materialName);
		if (it != m_TextureInputs.end())
		{
			it->second = std::move(texture);
			return;
		}
		m_TextureInputs.emplace(materialName, std::move(texture));
	}

	void Material::Set(const std::string& materialName, const DirectX::XMFLOAT4& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "float4"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_FloatInputs.find(materialName);
		if (it != m_FloatInputs.end())
		{
			it->second = value;
			return;
		}
		m_FloatInputs.emplace(materialName, value);
	}

	void Material::Set(const std::string& materialName, const DirectX::XMFLOAT3& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "float3"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_FloatInputs.find(materialName);
		if (it != m_FloatInputs.end())
		{
			it->second = { value.x, value.y, value.z, 0.0f };
			return;
		}
		m_FloatInputs.emplace(materialName, DirectX::XMFLOAT4(value.x, value.y, value.z, 0.0f));
	}

	void Material::Set(const std::string& materialName, const DirectX::XMFLOAT2& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "float2"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_FloatInputs.find(materialName);
		if (it != m_FloatInputs.end())
		{
			it->second = { value.x, value.y, 0.0f, 0.0f };
			return;
		}
		m_FloatInputs.emplace(materialName, DirectX::XMFLOAT4(value.x, value.y, 0.0f, 0.0f));
	}

	void Material::Set(const std::string& materialName, float value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "float"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_FloatInputs.find(materialName);
		if (it != m_FloatInputs.end())
		{
			it->second = { value, 0.0f, 0.0f, 0.0f };
			return;
		}
		m_FloatInputs.emplace(materialName, DirectX::XMFLOAT4(value, 0.0f, 0.0f, 0.0f));
	}

	void Material::Set(const std::string& materialName, const DirectX::XMINT4& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "int4"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_IntInputs.find(materialName);
		if (it != m_IntInputs.end())
		{
			it->second = value;
			return;
		}
		m_IntInputs.emplace(materialName, value);
	}

	void Material::Set(const std::string& materialName, const DirectX::XMINT3& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "int3"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_IntInputs.find(materialName);
		if (it != m_IntInputs.end())
		{
			it->second = { value.x, value.y, value.z, 0 };
			return;
		}
		m_IntInputs.emplace(materialName, DirectX::XMINT4(value.x, value.y, value.z, 0));
	}

	void Material::Set(const std::string& materialName, const DirectX::XMINT2& value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "int2"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_IntInputs.find(materialName);
		if (it != m_IntInputs.end())
		{
			it->second = { value.x, value.y, 0, 0 };
			return;
		}
		m_IntInputs.emplace(materialName, DirectX::XMINT4(value.x, value.y, 0, 0));
	}

	void Material::Set(const std::string& materialName, int value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "int"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		/*이미 슬롯에 Material 있으면 현재 전달된 Material로 대체 합니다.*/
		auto it = m_IntInputs.find(materialName);
		if (it != m_IntInputs.end())
		{
			it->second = { value, 0, 0, 0 };
			return;
		}
		m_IntInputs.emplace(materialName, DirectX::XMINT4(value, 0, 0, 0));
	}

	void Material::Set(const std::string& materialName, bool value)
	{
		if (!m_Shader->CheckInputValidation(materialName, "bool"))
			CS_FATAL(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", materialName.c_str(), materialName.c_str());

		auto it = m_BoolInputs.find(materialName);
		if (it != m_BoolInputs.end())
		{
			it->second = value;
			return;
		}
		m_BoolInputs.emplace(materialName, value);
	}

	std::shared_ptr<Texture> Material::GetTextureInput(const std::string& textureName) const
	{
		auto it = m_TextureInputs.find(textureName);
		if (it == m_TextureInputs.end())
			CS_FATAL(false, "%s의 Texture Input이 Set 되지 않았습니다.", textureName.c_str());

		return it->second.lock();
	}

	const DirectX::XMFLOAT4& Material::GetFloatInput(const std::string& floatName) const
	{
		auto it = m_FloatInputs.find(floatName);
		if (it == m_FloatInputs.end())
			CS_FATAL(false, "%s의 Float Input이 Set 되지 않았습니다.", floatName.c_str());

		return it->second;
	}

	const DirectX::XMINT4& Material::GetIntInput(const std::string& intName) const
	{
		auto it = m_IntInputs.find(intName);
		if (it == m_IntInputs.end())
			CS_FATAL(false, "%s의 Int Input이 Set 되지 않았습니다.", intName.c_str());

		return it->second;
	}

	bool Material::GetBoolInput(const std::string& boolName) const
	{
		auto it = m_BoolInputs.find(boolName);
		if (it == m_BoolInputs.end())
			CS_FATAL(false, "%s의 Bool Input이 Set 되지 않았습니다.", boolName.c_str());

		return it->second;
	}
}