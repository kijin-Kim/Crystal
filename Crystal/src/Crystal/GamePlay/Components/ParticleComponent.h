#pragma once
#include "PrimitiveComponent.h"
#include "Crystal/Core/Timer.h"


namespace Crystal {
	class ParticleComponent;


	struct Particle
	{
		Particle(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity, float startScale, float endScale, float rotation, float lifeTime);

		void Update(float deltaTime);

		Particle(Particle&& other) = default;
		Particle& operator=(Particle&& other) = default;

		Particle(const Particle&) = delete;
		Particle& operator=(const Particle&) = delete;


		DirectX::XMFLOAT3 Position = Vector3::Zero;
		DirectX::XMFLOAT3 Velocity = Vector3::Zero;
		float Scale = 1.0f;
		float Rotation = 0.0f;

		DirectX::XMFLOAT4X4 World = Matrix4x4::Identity();

		float LifeTime = 0.0f;

		bool bIsDead = false;

		Timer LifeTimeTimer = {};

		uint32_t SubImageIndex = 0;
		float subImageIndexCounter = 0.0f;


		//=====
		float StartScale = 1.0f;
		float EndScale = 1.0f;
	};


	class ParticleEmitter : public Object
	{
	public:
		ParticleEmitter() = default;
		~ParticleEmitter() override = default;

		void Update(float deltaTime) override;


		Shared<ParticleComponent> GetParticleComponent();
		DirectX::XMFLOAT3 GetParticleOrigin();

		void SpawnNewParticle();

		const std::vector<Particle>& GetParticles() const { return m_Particles; }

		STATIC_TYPE_IMPLE(ParticleEmitter)

	public:
		float MinStartScale = 1.0f;
		float MaxStartScale = 1.0f;

		float MinEndScale = 1.0f;
		float MaxEndScale = 1.0f;


		DirectX::XMFLOAT3 MinVelocity = {0.0f, 0.0f, 0.0f};
		DirectX::XMFLOAT3 MaxVelocity = {0.0f, 0.0f, 0.0f};

		float MinRotationAngle = 0.0f;
		float MaxRotationAngle = 0.0f;

		float MinLifeTime = 1.0f;
		float MaxLifeTime = 1.0f;

		float MinRadius = 0.0f;
		float MaxRadius = 0.0f;

		uint64_t MinSpawnCount = 0;
		uint64_t MaxSpawnCount = 1;

		


		Shared<Material> Material = nullptr;
		uint32_t HorizontalSubImageCount = 1;
		uint32_t VerticalSubImageCount = 1;
		

	private:
		std::vector<Particle> m_Particles;
	};

	class ParticleSystem : public Object
	{
	public:
		ParticleSystem() = default;
		~ParticleSystem() override = default;

		void AddParticleEmitter(const Shared<ParticleEmitter>& emitter);

		void Update(float deltaTime) override;

		const std::vector<Shared<ParticleEmitter>>& GetParticleEmitters() const { return m_ParticleEmitters; }

		STATIC_TYPE_IMPLE(ParticleSystem)


	private:
		std::vector<Shared<ParticleEmitter>> m_ParticleEmitters;
	};


	class ParticleComponent : public PrimitiveComponent
	{
	public:
		ParticleComponent() = default;
		~ParticleComponent() override = default;

		void RegisterComponent() override;

		void Update(float deltaTime) override;

		void SetParticleSystem(const Shared<ParticleSystem>& particleSystem);

		const Shared<ParticleSystem>& GetParticleSystem() const { return m_ParticleSystem; }

		STATIC_TYPE_IMPLE(ParticleComponent)

	private:
		Shared<ParticleSystem> m_ParticleSystem;
	};

}
