#include "cspch.h"
#include "Material.h"

namespace Crystal {

	void Material::Set(const std::string& inputBindName, std::shared_ptr<Texture> texture)
	{
		if (!m_Shader->CheckInputValidation({ inputBindName, D3D_SIT_TEXTURE }))
			CS_ASSERT(false, "%s �̸��� ���� Shader Input�� �������� �ʰų� %s �̸��� ���� Shader Input�� Ÿ�԰� ���� Ÿ���� ��Ī���� �ʽ��ϴ�", inputBindName.c_str(), inputBindName.c_str());
		
		/*�̹� ���Կ� �ؽ��İ� ������ ���� ���޵� �ؽ��ķ� ��ü �մϴ�.*/
		auto it = m_TextureInputs.find(inputBindName);
		if (it != m_TextureInputs.end())
		{
			it->second = std::move(texture);
			return;
		}
		m_TextureInputs.emplace(inputBindName, std::move(texture));
	}

	Texture* Material::GetTextureInput(const std::string& inputBindName)
	{
		auto it = m_TextureInputs.find(inputBindName);
		if (it == m_TextureInputs.end())
			CS_ASSERT(false, "%s�� Texture Input�� Set ���� �ʾҽ��ϴ�.", inputBindName.c_str());
		
		return it->second.get();
	}

}