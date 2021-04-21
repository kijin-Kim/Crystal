#include "cspch.h"
#include "Material.h"

namespace Crystal {

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
