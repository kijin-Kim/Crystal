#include "cspch.h"
#include "ParticleComponent.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::ParticleComponent)


namespace Crystal {


	ParticleComponent::ParticleComponent()
	{

		m_InitVelocity = {0.0f, 100.0f, 0.0f};
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

		TimerManager::Instance().SetTimer("ParticleSpawnTimer", CS_TIMER_FN(ParticleComponent::SpawnNewParticle), 3.0f, true);
		
		
	
		
		for(auto& particle : m_Particles)
		{			
			particle.Position = Vector3::Add(particle.Position, Vector3::Multiply(particle.Velocity, deltaTime));
			particle.World = Matrix4x4::Multiply(Matrix4x4::Scale(particle.Scale), Matrix4x4::Translation(particle.Position));
		}

		/*for(auto it = m_Particles.begin(); it!= m_Particles.end();)
		{
			
		}*/
		
	}

	void ParticleComponent::SpawnNewParticle()
	{
		Particle newParticle = {};
		newParticle.Position = m_InitPosition;
		newParticle.Scale = m_InitScale;
		newParticle.Velocity = m_InitVelocity;
		
		m_Particles.push_back(newParticle);
	}
	
}

