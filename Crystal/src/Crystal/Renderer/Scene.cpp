﻿#include "cspch.h"
#include "Scene.h"

#include "Crystal/Core/Device.h"
#include "SceneConstantBufferData.h"

namespace Crystal {


	void Scene::SetMainCamera(const Shared<CameraComponent>& camera)
	{
		auto it = std::find_if(Cameras.begin(), Cameras.end(), [&camera](const Weak<CameraComponent>& other)-> bool
		{
			return other.lock() == camera;
		});

		if (it != Cameras.begin())
		{
			// 카메라가 이미 존재하고 메인 카메라(Camera[0])이므로 아무것도 할 필요가 없음.
			return;
		}


		if (it == Cameras.end())
		{
			Cameras.insert(Cameras.begin(), camera);
		}
		else
		{
			Cameras[0] = camera;
		}
	}

	void Scene::RemoveGarbage()
	{
		for (auto it = StaticMeshes.begin(); it != StaticMeshes.end();)
		{
			if (it->expired())
			{
				it = StaticMeshes.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = SkeletalMeshes.begin(); it != SkeletalMeshes.end();)
		{
			if (it->expired())
			{
				it = SkeletalMeshes.erase(it);
			}
			else
			{
				++it;
			}
		}

		for (auto it = Particles.begin(); it != Particles.end();)
		{
			if (it->expired())
			{
				it = Particles.erase(it);
			}
			else
			{
				++it;
			}
		}


		for (auto it = Lights.begin(); it != Lights.end();)
		{
			if (it->expired())
			{
				it = Lights.erase(it);
			}
			else
			{
				++it;
			}
		}


		for (auto it = Cameras.begin(); it != Cameras.end();)
		{
			if (it->expired())
			{
				it = Cameras.erase(it);
			}
			else
			{
				++it;
			}
		}
	}


}
;
