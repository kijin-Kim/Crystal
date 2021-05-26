#pragma once
#include "PrimitiveComponent.h"


namespace Crystal {

	
	
	class ParticleComponent : public PrimitiveComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
		}

	public:
		struct Particle
		{
			DirectX::XMFLOAT3 Position = Vector3::Zero;
			DirectX::XMFLOAT3 Velocity = Vector3::Zero;
			float Scale = 100.0f;
			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();

			Particle() = default;
			~Particle() = default;
			Particle(const Particle&) = default;
			Particle& operator=(const Particle&) = default;
		};

	public:
		ParticleComponent();
		~ParticleComponent() override = default;

		void RegisterComponent() override;

		void Update(float deltaTime) override;


		std::vector<Particle>& GetParticles() { return m_Particles; }

		STATIC_TYPE_IMPLE(ParticleComponent)

	private:
		

		std::vector<Particle> m_Particles;
	};
}
