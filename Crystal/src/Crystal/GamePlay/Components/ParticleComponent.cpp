#include "cspch.h"
#include "ParticleComponent.h"

#include <random>


#include "Crystal/GamePlay/World/Level.h"

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
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> disxz(-100.0f, 100.0f);

		std::uniform_real_distribution<> disy(500.0f, 1000.0f);

		if (m_Particles.size() <= m_ParticleSpawnCount)
		{
			DirectX::XMFLOAT3 velocity = { (float)disxz(gen), (float)disy(gen), (float)disxz(gen) };
			m_Particles.emplace_back(m_InitPosition, velocity, m_InitScale, m_InitLifeTime);
		}
	}

}
