#include "cspch.h"
#include "ParticleComponent.h"


#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"


namespace Crystal {

	Particle::Particle(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& velocity, float startScale, float endScale, float rotation, float lifeTime)
	{
		Position = position;
		Velocity = velocity;
		Scale = startScale;
		LifeTime = lifeTime;
		Rotation = rotation;

		StartScale = startScale;
		EndScale = endScale;
	}

	void Particle::Update(float deltaTime)
	{
		Position = Vector3::Add(Position, Vector3::Multiply(Velocity, deltaTime));

		LifeTimeTimer.Tick();

		if (LifeTimeTimer.GetElapsedTime() >= LifeTime)
		{
			bIsDead = true;
		}
	}

	void ParticleEmitter::Update(float deltaTime)
	{
		SpawnNewParticle();


		for (auto& particle : m_Particles)
		{
			particle.Update(deltaTime);
			particle.Scale = Lerp(particle.StartScale, particle.EndScale, particle.LifeTimeTimer.GetElapsedTime() / particle.LifeTime);
			particle.subImageIndexCounter += VerticalSubImageCount * HorizontalSubImageCount * deltaTime;
			particle.SubImageIndex = (int)particle.subImageIndexCounter % (VerticalSubImageCount * HorizontalSubImageCount);
		}

		for (auto it = m_Particles.begin(); it != m_Particles.end();)
		{
			if (it->bIsDead)
			{
				it = m_Particles.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	Shared<ParticleComponent> ParticleEmitter::GetParticleComponent()
	{
		return Crystal::Cast<ParticleComponent>(GetOuter().lock()->GetOuter());
	}

	DirectX::XMFLOAT3 ParticleEmitter::GetParticleOrigin()
	{
		auto particleComponent = GetParticleComponent();
		return particleComponent->GetWorldPosition();
	}

	void ParticleEmitter::SpawnNewParticle()
	{
		if (m_Particles.size() < RandomFloatInRange(MinSpawnCount, MaxSpawnCount))
		{
			auto position = Vector3::RandomPositionInSphere(GetParticleOrigin(), RandomFloatInRange(MinRadius, MaxRadius));
			auto startScale = RandomFloatInRange(MinStartScale, MaxStartScale);
			auto endScale = RandomFloatInRange(MinEndScale, MaxEndScale);
			auto velocity = Vector3::RandomInRange(MinVelocity, MaxVelocity);
			auto rotation = RandomFloatInRange(MinRotationAngle, MaxRotationAngle);
			auto lifeTime = RandomFloatInRange(MinLifeTime, MaxLifeTime);

			m_Particles.emplace_back(position, velocity, startScale, endScale, rotation, lifeTime);
		}
	}

	void ParticleSystem::AddParticleEmitter(const Shared<ParticleEmitter>& emitter)
	{
		emitter->SetOuter(weak_from_this());
		m_ParticleEmitters.push_back(emitter);
	}

	void ParticleSystem::Update(float deltaTime)
	{
		for (auto& emitter : m_ParticleEmitters)
		{
			emitter->Update(deltaTime);
		}
	}

	void ParticleComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->Particles.push_back(Cast<ParticleComponent>(shared_from_this()));
#endif
	}

	void ParticleComponent::Update(float deltaTime)
	{
		PrimitiveComponent::Update(deltaTime);

		m_ParticleSystem->Update(deltaTime);
	}

	void ParticleComponent::SetParticleSystem(const Shared<ParticleSystem>& particleSystem)
	{
		particleSystem->SetOuter(weak_from_this());
		m_ParticleSystem = particleSystem;
	}


}


