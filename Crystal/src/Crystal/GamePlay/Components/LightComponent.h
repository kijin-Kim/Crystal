#pragma once
#include "TransformComponent.h"

namespace Crystal {
	class LightComponent : public TransformComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<TransformComponent>(*this);
			ar & m_LightColor;
			ar & m_Intensity;
		}

	public:
		LightComponent() = default;
		~LightComponent() override = default;

		void RegisterComponent() override;

		void SetLightColor(const DirectX::XMFLOAT3& color) { m_LightColor = color; }
		const DirectX::XMFLOAT3& GetLightColor() const { return m_LightColor; }

		void SetLightIntensity(const float intensity) { m_Intensity = intensity; }
		float GetLightIntensity() const { return m_Intensity; }


		STATIC_TYPE_IMPLE(LightComponent)
	private:
		DirectX::XMFLOAT3 m_LightColor = Vector3::White;
		float m_Intensity = 1.0f;
	};

	class LocalLightComponent : public LightComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightComponent>(*this);

		}

		
	public:
		LocalLightComponent() = default;
		~LocalLightComponent() override = default;


		STATIC_TYPE_IMPLE(LocalLightComponent)
	};

	class DirectionalLightComponent : public LightComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<LightComponent>(*this);

		}
		
	public:
		DirectionalLightComponent() = default;
		~DirectionalLightComponent() override = default;


		STATIC_TYPE_IMPLE(DirectionalLightComponent)
	};
}
