#pragma once
#include "PrimitiveComponent.h"
#include "Crystal/Core/Timer.h"


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
			Particle(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity, float scale, float rotation, float lifeTime)
			{
				Position = position;
				Velocity = velocity;
				Scale = scale;
				LifeTime = lifeTime;
				Rotation = rotation;
			}


			void Update(float deltaTime)
			{		
				Position = Vector3::Add(Position, Vector3::Multiply(Velocity, deltaTime));
				
				LifeTimeTimer.Tick();

				if (LifeTimeTimer.GetElapsedTime() >= LifeTime)
				{
					bIsDead = true;
				}
			}

			Particle(Particle&& other) = default;
			Particle& operator=(Particle&& other) = default;

			Particle(const Particle&) = delete;
			Particle& operator=(const Particle&) = delete;


			DirectX::XMFLOAT3 Position = Vector3::Zero;
			DirectX::XMFLOAT3 Velocity = Vector3::Zero;
			float Scale = 100.0f;
			float Rotation = 0.0f;

			DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();

			float LifeTime = 0.0f;

			bool bIsDead = false;

			Timer LifeTimeTimer = {};

			uint32_t SubImageIndex = 0;
			float subImageIndexCounter = 0.0f;
		};

	public:
		ParticleComponent();
		~ParticleComponent() override;

		void RegisterComponent() override;

		void Update(float deltaTime) override;

		void SpawnNewParticle();


		std::vector<Particle>& GetParticles() { return m_Particles; }

		STATIC_TYPE_IMPLE(ParticleComponent)

	private:
		float m_InitScale = 100.0f;
		DirectX::XMFLOAT3 m_InitVelocity = {0.0f, 500.0f, 0.0f};
		float m_InitLifeTime = 1.0f;
		uint64_t m_ParticleSpawnCount = 5;
		uint32_t m_HorizontalSubImageCount = 0;
		uint32_t m_VerticalSubImageCount = 0;

		std::vector<Particle> m_Particles;
	};
}
