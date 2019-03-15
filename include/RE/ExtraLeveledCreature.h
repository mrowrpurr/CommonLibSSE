#pragma once

#include "RE/BSExtraData.h"  // BSExtraData
#include "RE/ExtraDataTypes.h"  // ExtraDataType
#include "RE/FormTypes.h"  // TESNPC


namespace RE
{
	class ExtraLeveledCreature : public BSExtraData
	{
	public:
		enum { kExtraTypeID = ExtraDataType::kLeveledCreature };


		virtual ~ExtraLeveledCreature();				// 00

		// override (BSExtraData)
		virtual ExtraDataType GetType() const override;	// 01 - { return kLeveledCreature; }


		// members
		TESNPC*	unk10;	// 10
		TESNPC*	unk18;	// 18
	};
	STATIC_ASSERT(sizeof(ExtraLeveledCreature) == 0x20);
}