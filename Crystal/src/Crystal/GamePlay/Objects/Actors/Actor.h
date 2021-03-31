#pragma once
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class Component;
	class TransformComponent;
	class Level;
	class World;


	// Spawn-able object
	class Actor : public Updatable
	{
	public:
		Actor() = default;
		~Actor() override = default;

		virtual void Begin() {}
		virtual void End() {}
		void Update(const float deltaTime) override { Updatable::Update(deltaTime); }

		void UpdateComponents(float deltaTime);
		/*Component�� actor�� �����̳ʿ� �����ϰ� Owner�� ���� Actor�� �����մϴ�.*/
		void RegisterComponent(Component* component);
		/*Component�� actor�� �����̳ʿ��� �����ϰ� Owner�� nullptr�� �����մϴ�.*/
		void UnRegisterComponent(Component* component);

		/*Component�� TransformComponent���� Hierarchy�� �ű�ϴ�.*/
		void MoveToTransformComponentHierarchy(TransformComponent* component);

		TransformComponent* GetMainComponent() const { return m_MainComponent; }

		void SetPosition(const DirectX::XMFLOAT3& position);

		STATIC_TYPE_IMPLE(Actor)
	protected:
		/*Component�� �����ϰ� Register�մϴ�.*/
		template <class T>
		T* CreateComponent(const std::string& name = "")
		{
			/*TODO : Component�� �ް� üũ...*/
			T* newComponent = new T();
			RegisterComponent(newComponent);
			newComponent->OnCreate();
			if (!name.empty())
				newComponent->SetObjectName(name);
			
			return newComponent;
		}
	protected:
		/*MainComponent�� ����Ű�� Raw Pointer�Դϴ�. ���Ǹ� ���� Pointer�� ��,
		LifeTime�� �����̳��� Unique Pointer �� ���� �����˴ϴ�.*/
		TransformComponent* m_MainComponent = nullptr;
		std::vector<std::unique_ptr<Component>> m_Components;
		/*�θ�, �ڽļ����� Transform �� ��ġ�Ǿ��ִ� �����̳� (MainComponent ����)*/
		std::vector<std::unique_ptr<TransformComponent>> m_TransformHierarchy;
	};
}
