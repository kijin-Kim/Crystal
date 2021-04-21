#include "cspch.h"
#include "Material.h"

namespace Crystal {
	void Material::Set(const std::string& materialName, std::weak_ptr<Texture> texture)
	{
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}
			

		if (!shader->CheckInputValidation(materialName, D3D_SIT_TEXTURE) && !shader->CheckInputValidation(materialName, D3D_SIT_UAV_RWTYPED))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "float4"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "float3"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "float2"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "float"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "int4"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "int3"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "int2"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "int"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

		/*�̹� ���Կ� Material ������ ���� ���޵� Material�� ��ü �մϴ�.*/
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
		auto shader = Cast<Shader>(GetObjectOwner(Material::MaterialOwnerType::Owner_Shader));
		if (!shader)
		{
			CS_WARN("Material�� Shader�� �������� �ʽ��ϴ�");
			return;
		}

		if (!shader->CheckInputValidation(materialName, "bool"))
			CS_FATAL(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", materialName.c_str(), materialName.c_str());

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
			CS_FATAL(false, "%s�� Texture Input�� Set ���� �ʾҽ��ϴ�.", textureName.c_str());

		return it->second.lock();
	}

	const DirectX::XMFLOAT4& Material::GetFloatInput(const std::string& floatName) const
	{
		auto it = m_FloatInputs.find(floatName);
		if (it == m_FloatInputs.end())
			CS_FATAL(false, "%s�� Float Input�� Set ���� �ʾҽ��ϴ�.", floatName.c_str());

		return it->second;
	}

	const DirectX::XMINT4& Material::GetIntInput(const std::string& intName) const
	{
		auto it = m_IntInputs.find(intName);
		if (it == m_IntInputs.end())
			CS_FATAL(false, "%s�� Int Input�� Set ���� �ʾҽ��ϴ�.", intName.c_str());

		return it->second;
	}

	bool Material::GetBoolInput(const std::string& boolName) const
	{
		auto it = m_BoolInputs.find(boolName);
		if (it == m_BoolInputs.end())
			CS_FATAL(false, "%s�� Bool Input�� Set ���� �ʾҽ��ϴ�.", boolName.c_str());

		return it->second;
	}

	bool NewMaterial::UsingSameTextures(NewMaterial* material)
	{
		auto other = material;

		if (!material)
			return false;

		bool result = true;
		if (!AlbedoTexture.expired() && !other->AlbedoTexture.expired())
		{
			result &= AlbedoTexture.lock() == other->AlbedoTexture.lock();
			if (!result)
				return false;
		}
		else if ((AlbedoTexture.expired() != other->AlbedoTexture.expired()))
		{
			return false;
		}

		if (!MetallicTexture.expired() && !other->MetallicTexture.expired())
		{
			result &= MetallicTexture.lock() == other->MetallicTexture.lock();
			if (!result)
				return false;
		}
		else if ((MetallicTexture.expired() != other->MetallicTexture.expired()))
		{
			return false;
		}



		if (!RoughnessTexture.expired() && !other->RoughnessTexture.expired())
		{
			result &= RoughnessTexture.lock() == other->RoughnessTexture.lock();
			if (!result)
				return false;
		}
		else if ((RoughnessTexture.expired() != other->RoughnessTexture.expired()))
		{
			return false;
		}


		if (!NormalTexture.expired() && !other->NormalTexture.expired())
		{
			result &= NormalTexture.lock() == other->NormalTexture.lock();
			if (!result)
				return false;
		}
		else if ((NormalTexture.expired() != other->NormalTexture.expired()))
		{
			return false;
		}

		if (!EmissiveTexture.expired() && !other->EmissiveTexture.expired())
		{
			result &= EmissiveTexture.lock() == other->EmissiveTexture.lock();
			if (!result)
				return false;
		}
		else if ((EmissiveTexture.expired() != other->EmissiveTexture.expired()))
		{
			return false;
		}

		return result;
	}
}
