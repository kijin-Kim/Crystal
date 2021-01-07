#include "cspch.h"
#include "Material.h"

namespace Crystal {

	void Material::Set(const std::string& inputBindName, std::shared_ptr<Texture> texture)
	{
		if (!m_Shader->CheckInputValidation({ inputBindName, D3D_SIT_TEXTURE }))
			CS_ASSERT(false, "%s 이름을 가진 Shader Input이 존재하지 않거나 %s 이름을 가진 Shader Input의 타입과 현재 타입이 매칭되지 않습니다", inputBindName.c_str(), inputBindName.c_str());
		
		/*이미 슬롯에 텍스쳐가 있으면 현재 전달된 텍스쳐로 대체 합니다.*/
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
			CS_ASSERT(false, "%s의 Texture Input이 Set 되지 않았습니다.", inputBindName.c_str());
		
		return it->second.get();
	}

}