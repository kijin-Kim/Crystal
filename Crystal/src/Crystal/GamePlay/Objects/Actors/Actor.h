#pragma once
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {
	class Component;
	class TransformComponent;
	class Level;
	class World;


	// Spawn-able object
	class Actor : public Object
	{
	public:
		Actor();
		~Actor() override = default;

		void OnCreate() override;

		virtual void Begin() {}
		virtual void End() {}

		void UpdateComponents(float deltaTime);
		/*Component를 actor의 컨테이너에 저장하고 Owner를 현재 Actor로 지정합니다.*/
		void AddComponent(const std::shared_ptr<Component>& component);
		void RegisterComponents();

		/*Component를 TransformComponent들의 Hierarchy에 옮깁니다.*/
		void MoveToTransformComponentHierarchy(const std::shared_ptr <TransformComponent>& component);

		std::weak_ptr<TransformComponent> GetMainComponent() const { return m_MainComponent; }

		void SetPosition(const DirectX::XMFLOAT3& position);

		std::weak_ptr<Component> GetComponentByName(const std::string& name);
		std::weak_ptr<Component> GetComponentByClass(const std::string& classType);


		void SetAttachment(const std::shared_ptr<TransformComponent>& from, 
			const std::shared_ptr<TransformComponent>& to);


		STATIC_TYPE_IMPLE(Actor)
	protected:
		/*Component를 생성하고 Register합니다.*/
		template <class T>
		std::shared_ptr<T> CreateComponent(const std::string& name = "")
		{
			/*TODO : Component만 받게 체크...*/
			std::shared_ptr<T> newComponent = CreateObject<T>(name, weak_from_this());
			AddComponent(newComponent);
			return newComponent;
		}





	protected:
		std::shared_ptr<TransformComponent> m_MainComponent = nullptr;
		std::vector<std::shared_ptr<Component>> m_Components;
		/*부모, 자식순으로 Transform 가 배치되어있는 컨테이너 (MainComponent 제외)*/
		std::vector<std::shared_ptr<TransformComponent>> m_TransformHierarchy;
	};
}
