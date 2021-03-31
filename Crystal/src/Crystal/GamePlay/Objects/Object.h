#pragma once
#include <array>

namespace Crystal {

	

	// Has name
	class Object
	{
	public:
		enum ObjectOwnerType
		{
			OOT_Level = 0,
			OOT_World,
			OOT_Actor,
			OOT_Component,
			OOT_Spawner,
			Object_Owner_Type_Count
		};
	public:
		Object() = default;
		virtual ~Object() = default;

		void SetObjectName(const std::string& name) { m_Name = name; }
		void SetObjectOwner(Object* object, ObjectOwnerType ownerType) { m_Owners[ownerType] = object; }

		const std::string& GetObjectName() const { return m_Name; }
		Object* GetObjectOwner(ObjectOwnerType ownerType) const { return m_Owners[ownerType]; }


	private:
		std::string m_Name;
		std::array<Object*, Object_Owner_Type_Count> m_Owners;
	};
	
	// Update-able
	class Updatable : public Object
	{
	public:
		Updatable() = default;
		~Updatable() override = default;

		virtual void Update(const float deltaTime) {}
	};
}