#include "cspch.h"
#include "LineActor.h"



namespace Crystal {
	void LineActor::Update(float deltaTime)
	{
		Actor::Update(deltaTime);

		
		timer.Tick();
		
		if(timer.GetElapsedTime() >= m_LifeTime)
		{
			Destroy();
		}
	}
	
}



