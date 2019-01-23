#pragma once

#include "skse64/GameFormComponents.h"  // BaseFormComponent

#include "RE/BSFixedString.h"  // BSFixedString


namespace RE
{
	class TESFullName : public BaseFormComponent
	{
	public:
		virtual ~TESFullName();

		// override (BaseFormComponent)
		virtual void		Init() override;									// 01
		virtual void		ReleaseRefs() override;								// 02
		virtual void		CopyFromBase(BaseFormComponent* a_rhs) override;	// 03

		// add
		virtual UInt32		GetFullNameLength();								// 04
		virtual const char*	GetName();											// 05


		// members
		BSFixedString name;	// 08 - FULL
	};
	STATIC_ASSERT(sizeof(TESFullName) == 0x10);
}