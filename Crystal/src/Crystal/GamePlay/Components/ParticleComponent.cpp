#include "cspch.h"
#include "ParticleComponent.h"


#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

BOOST_CLASS_EXPORT(Crystal::ParticleComponent)


namespace Crystal {

	ParticleComponent::ParticleComponent()
	{
	}

	ParticleComponent::~ParticleComponent()
	{
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


		SpawnNewParticle();


		for (auto& particle : m_Particles)
		{
			particle.Update(deltaTime);
			particle.subImageIndexCounter += 36.0f * deltaTime;
			particle.SubImageIndex = (int)particle.subImageIndexCounter % (6 * 6);
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

	void ParticleComponent::SpawnNewParticle()
	{
		
		if (m_Particles.size() < m_ParticleSpawnCount)
		{
			m_Particles.emplace_back(GetWorldPosition(), Vector3::RandomInRange({0.0f, 0.0f, 0.0f}, { 500.0f, 500.0f, 500.0f }), GetScale().x, RandomFloatInRange(0.0f, 359.0f), 2.0f);
		}
	}

}
