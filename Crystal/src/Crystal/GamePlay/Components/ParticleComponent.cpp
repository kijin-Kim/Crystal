﻿#include "cspch.h"
#include "ParticleComponent.h"

#include "Crystal/GamePlay/World/Level.h"

BOOST_CLASS_EXPORT(Crystal::ParticleComponent)


namespace Crystal {


	ParticleComponent::ParticleComponent()
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

		
		
		
		
		for(auto& particle : m_Particles)
		{			
			particle.Position = Vector3::Add(particle.Position, Vector3::Multiply(particle.Velocity, deltaTime));
			particle.World = Matrix4x4::Multiply(Matrix4x4::Scale(particle.Scale), Matrix4x4::Translation(particle.Position));
		}

		/*for(auto it = m_Particles.begin(); it!= m_Particles.end();)
		{
			
		}*/
		
	}
}

