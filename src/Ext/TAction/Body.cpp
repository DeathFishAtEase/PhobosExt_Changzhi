#include "Body.h"

#include <YRpp.h>
#include <TagClass.h>
#include <TechnoClass.h>
#include <ArrayClasses.h>
#include <MessageListClass.h>

#include <Utilities/SavegameDef.h>
#include <Utilities/SpawnerHelper.h>

#include <MyNew/WaypointLabelClass.h>

#include <vector>
#include <string>

//Static init
TActionExt::ExtContainer TActionExt::ExtMap;

// =============================
// load / save

template <typename T>
void TActionExt::ExtData::Serialize(T& Stm)
{
	//Stm;
}

void TActionExt::ExtData::LoadFromStream(PhobosStreamReader& Stm)
{
	Extension<TActionClass>::LoadFromStream(Stm);
	this->Serialize(Stm);
}

void TActionExt::ExtData::SaveToStream(PhobosStreamWriter& Stm)
{
	Extension<TActionClass>::SaveToStream(Stm);
	this->Serialize(Stm);
}

bool TActionExt::Execute(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject,
	TriggerClass* pTrigger, CellStruct const& location, bool& bHandled)
{
	bHandled = true;

	// Vanilla
	// switch (pThis->ActionKind)
	// {
	// default:
	// 	break;
	// };

	// Phobos
	switch (static_cast<PhobosTriggerAction>(pThis->ActionKind))
	{

	case PhobosTriggerAction::SetWaypointLabel:
		return TActionExt::SetWaypointLabel(pThis, pHouse, pObject, pTrigger, location);
	case PhobosTriggerAction::ClearWaypointLabel:
		return TActionExt::ClearWaypointLabel(pThis, pHouse, pObject, pTrigger, location);
	case PhobosTriggerAction::ClearAllWaypointLabels:
		return TActionExt::ClearAllWaypointLabels(pThis, pHouse, pObject, pTrigger, location);
	case PhobosTriggerAction::BindAllTeamMemberToTag:
		return TActionExt::BindAllTeamMemberToTag(pThis, pHouse, pObject, pTrigger, location);
	case PhobosTriggerAction::BindAllTechnoTypeToTag:
		return TActionExt::BindAllTechnoTypeToTag(pThis, pHouse, pObject, pTrigger, location);
	case PhobosTriggerAction::BindOwnerTechnoTypeToTag:
		return TActionExt::BindOwnerTechnoTypeToTag(pThis, pHouse, pObject, pTrigger, location);

	default:
		bHandled = false;
		return true;
	}
}

bool TActionExt::SetWaypointLabel(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	// 1, text
	const char* csfLabel = pThis->Text;

	// 2, waypoint index
	int wpIndex = pThis->Param3;

	// 3, max width
	int maxWidth = pThis->Param4;
	maxWidth = std::clamp(maxWidth, 0, 1000);
	if (maxWidth == 0) maxWidth = 250; // default is 250 pixels

	// 4, opacity
	int opacityPercent = pThis->Param5;
	opacityPercent = std::clamp(opacityPercent, 0, 100);

	// 5, color
	WaypointLabel::Color color;
	if (pThis->Param6 < 0 || pThis->Param6 >= WaypointLabel::Color::Count)
		color = WaypointLabel::Color::gold; // default is gold
	else
		color = static_cast<WaypointLabel::Color>(pThis->Param6);

	// set
	if (wpIndex >= 0 && csfLabel && csfLabel[0])
	{
		WaypointLabelClass::SetLabel(wpIndex, csfLabel, maxWidth, opacityPercent, color);
	}
	return true;
}

bool TActionExt::ClearWaypointLabel(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	int wpIndex = pThis->Param3;
	if (wpIndex >= 0)
	{
		WaypointLabelClass::ClearLabel(wpIndex);
	}
	return true;
}

bool TActionExt::ClearAllWaypointLabels(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	WaypointLabelClass::ClearAll();
	return true;
}

bool TActionExt::BindAllTeamMemberToTag(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	int teamIndex = pThis->Param3;
	int tagIndex = pThis->Param4;

	TagClass* targetTagClass = nullptr;
	for (auto const pTag : TagClass::Array)
	{
		if (!std::strcmp(pTag->Type->get_ID(), std::to_string(tagIndex).c_str()))
		{
			targetTagClass = pTag;
			break;
		}
	}
	if (!targetTagClass) return false;

	for (auto const pTechno : TechnoClass::Array)
	{
		if (pTechno->WhatAmI() != AbstractType::BuildingType)
		{
			if (FootClass* pFoot = abstract_cast<FootClass*>(pTechno))
			{
				if (pFoot->BelongsToATeam()
					&& pFoot->Team
					&& std::strcmp(pFoot->Team->Type->get_ID(), std::to_string(teamIndex).c_str()) == 0)
				{
					for (auto pUnit = pFoot->Team->FirstUnit; pUnit; pUnit = pUnit->NextTeamMember)
					{
						pUnit->AttachTrigger(targetTagClass);
					}
				}
			}
		}
	}

	return true;
}

bool TActionExt::BindAllTechnoTypeToTag(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	const char* techno = pThis->Text;
	int tagIndex = pThis->Param3;

	Debug::Log("[TActionExt::BindAllTechnoTypeToTag] Techno: %s, Tag: %d\n", techno, tagIndex);

	TagClass* pTagClass = nullptr;
	for (auto const pTag : TagClass::Array)
	{
		Debug::Log("[TActionExt::BindAllTechnoTypeToTag] get Tag is %s\n", pTag->Type->get_ID());
		if (pTag->Type && std::stoi(pTag->Type->get_ID()) == tagIndex)
		{
			pTagClass = pTag;
			Debug::Log("[TActionExt::BindAllTechnoTypeToTag] successful get targerTagClass %s\n", pTagClass->Type->get_ID());
			break;
		}
	}
	if (!pTagClass) return false;

	Debug::Log("[TActionExt::BindAllTechnoTypeToTag] successful get targerTagClass %s\n", pTagClass->Type->get_ID());

	for (auto const pTechno : TechnoClass::Array)
	{
		Debug::Log("[TActionExt::BindAllTechnoTypeToTag] check %s\n", pTechno);
		if (pTechno->get_ID() == techno)
		{
			pTechno->AttachTrigger(pTagClass);
			Debug::Log("[TActionExt::BindAllTechnoTypeToTag] bind %s to tag %s\n", pTechno, pTagClass->Type->get_ID());
		}
	}

	Debug::Log("[TActionExt::BindAllTechnoTypeToTag] successful bind all techno type to tag\n");

	return true;
}

bool TActionExt::BindOwnerTechnoTypeToTag(TActionClass* pThis, HouseClass* pHouse, ObjectClass* pObject, TriggerClass* pTrigger, CellStruct const& location)
{
	return true;
}

// =============================
// container

TActionExt::ExtContainer::ExtContainer() : Container("TActionClass") { }

TActionExt::ExtContainer::~ExtContainer() = default;

