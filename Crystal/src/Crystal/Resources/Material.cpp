#include "cspch.h"
#include "Material.h"

namespace Crystal {

	bool Material::UsingSameTextures(Material* other)
	{
		if (!other)
			return false;
		
		bool bHasSameTexture = true;
		
		if(!AlbedoTexture.expired() && !other->AlbedoTexture.expired())
		{
			bHasSameTexture &= AlbedoTexture.lock() == other->AlbedoTexture.lock();
			if (!bHasSameTexture)
			{
				return bHasSameTexture;
			}
		}

		if (!MetallicTexture.expired() && !other->MetallicTexture.expired())
		{
			bHasSameTexture &= MetallicTexture.lock() == other->MetallicTexture.lock();
			if (!bHasSameTexture)
			{
				return bHasSameTexture;
			}
		}

		if (!RoughnessTexture.expired() && !other->RoughnessTexture.expired())
		{
			bHasSameTexture &= RoughnessTexture.lock() == other->RoughnessTexture.lock();
			if (!bHasSameTexture)
			{
				return bHasSameTexture;
			}
		}

		if (!NormalTexture.expired() && !other->NormalTexture.expired())
		{
			bHasSameTexture &= NormalTexture.lock() == other->NormalTexture.lock();
			if (!bHasSameTexture)
			{
				return bHasSameTexture;
			}	
		}

		if (!EmissiveTexture.expired() && !other->EmissiveTexture.expired())
		{
			bHasSameTexture &= EmissiveTexture.lock() == other->EmissiveTexture.lock();
			if (!bHasSameTexture)
			{
				return bHasSameTexture;
			}
		}

		
	
		return bHasSameTexture;
	}
}
