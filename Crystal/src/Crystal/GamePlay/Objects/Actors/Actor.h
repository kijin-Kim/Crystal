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
		enum ActorOwnerType
		{
			Owner_Level = 0,
			Owner_Spawner = 1
		};

	public:
		Actor() = default;
		~Actor() override = default;

		void OnCreate() override;

		virtual void Begin() {}
		virtual void End() {}
		void Update(const float deltaTime) override { Updatable::Update(deltaTime); }

		void UpdateComponents(float deltaTime);
		/*Component�� actor�� �����̳ʿ� �����ϰ� Owner�� ���� Actor�� �����մϴ�.*/
		void RegisterComponent(const std::shared_ptr<Component>& component);

		/*Component�� TransformComponent���� Hierarchy�� �ű�ϴ�.*/
		void MoveToTransformComponentHierarchy(const std::shared_ptr <TransformComponent>& component);

		std::weak_ptr<TransformComponent> GetMainComponent() const { return m_MainComponent; }

		void SetPosition(const DirectX::XMFLOAT3& position);


		void SetAttachment(const std::shared_ptr<TransformComponent>& from, 
			const std::shared_ptr<TransformComponent>& to);


		STATIC_TYPE_IMPLE(Actor)
	protected:
		/*Component�� �����ϰ� Register�մϴ�.*/
		template <class T>
		std::shared_ptr<T> CreateComponent(const std::string& name = "")
		{
			/*TODO : Component�� �ް� üũ...*/
			std::shared_ptr<T> newComponent = std::make_shared<T>();
			newComponent->SetObjectOwner(weak_from_this(), Component::ComponentOwnerType::Owner_Actor);

			RegisterComponent(newComponent);
			newComponent->OnCreate();
			if (!name.empty())
				newComponent->SetObjectName(name);

			return newComponent;
		}





	protected:
		std::shared_ptr<TransformComponent> m_MainComponent = nullptr;
		std::vector<std::shared_ptr<Component>> m_Components;
		/*�θ�, �ڽļ����� Transform �� ��ġ�Ǿ��ִ� �����̳� (MainComponent ����)*/
		std::vector<std::shared_ptr<TransformComponent>> m_TransformHierarchy;
	};
}
