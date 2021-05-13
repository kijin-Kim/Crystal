#include "cspch.h"
#include "Material.h"

namespace Crystal {

	bool Material::UsingSameTextures(Material* material)
	{
		auto other = material;

		if (!material)
			return false;

		bool result = true;
		if (!AlbedoTexture && !other->AlbedoTexture)
		{
			result &= AlbedoTexture == other->AlbedoTexture;
			if (!result)
				return false;
		}
		else if ((AlbedoTexture != other->AlbedoTexture))
		{
			return false;
		}

		if (!MetallicTexture && !other->MetallicTexture)
		{
			result &= MetallicTexture == other->MetallicTexture;
			if (!result)
				return false;
		}
		else if ((MetallicTexture != other->MetallicTexture))
		{
			return false;
		}



		if (!RoughnessTexture && !other->RoughnessTexture)
		{
			result &= RoughnessTexture == other->RoughnessTexture;
			if (!result)
				return false;
		}
		else if ((RoughnessTexture != other->RoughnessTexture))
		{
			return false;
		}


		if (!NormalTexture && !other->NormalTexture)
		{
			result &= NormalTexture == other->NormalTexture;
			if (!result)
				return false;
		}
		else if ((NormalTexture != other->NormalTexture))
		{
			return false;
		}

		if (!EmissiveTexture && !other->EmissiveTexture)
		{
			result &= EmissiveTexture == other->EmissiveTexture;
			if (!result)
				return false;
		}
		else if ((EmissiveTexture != other->EmissiveTexture))
		{
			return false;
		}

		return result;
	}
}
