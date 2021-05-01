#include "cspch.h"
#include "LightActor.h"

namespace Crystal {


	void LocalLightActor::Initialize()
	{
		m_LightComponent = CreateSubObject<LocalLightComponent>({ "LocalLightComponent" });
		m_LightComponent->SetLocalPosition(Vector3::Zero);
		m_LightComponent->SetLocalPosition(Vector3::White);

		SetMainComponent(m_LightComponent);
		
	}

	void DirectionalLightActor::Initialize()
	{
		m_LightComponent = CreateSubObject<DirectionalLightComponent>({ "DirectionalLightComponent" });
		m_LightComponent->SetLocalPosition(Vector3::Zero);
		m_LightComponent->SetLocalPosition(Vector3::White);

		SetMainComponent(m_LightComponent);
		
	}
}
