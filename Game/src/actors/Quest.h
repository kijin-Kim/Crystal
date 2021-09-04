#pragma once
#include "Crystal/GamePlay/Objects/Object.h"

class Quest : public Crystal::Object
{
public:
	enum class QuestType
	{
		QT_Location,
		QT_Destroy
	};

public:
	Quest() = default;
	~Quest() override = default;



	STATIC_TYPE_IMPLE(Quest)

private:
	


};
