#include "catch.hpp"

#include "REL/Relocation.h"
#include "SKSE/SKSE.h"

namespace
{
	class TrampolineFixture
	{
	public:
		static constexpr std::size_t TrampolineSize = 64;

		TrampolineFixture()
		{
			_successPtr = &_success;
#ifdef ENABLE_SKYRIM_VR
			REQUIRE(REL::Module::inject(REL::Module::Runtime::VR));
#else
			REQUIRE(REL::Module::inject(REL::Module::Runtime::SE));
#endif
		}

		~TrampolineFixture()
		{
			_successPtr = nullptr;
			REL::Module::reset();
		}

	protected:
		void TestTrampoline(SKSE::Trampoline& trampoline)
		{
			REL::ID FunctionID(110073);
			REL::Relocation<std::uint64_t(void*)> Function(FunctionID);
			REL::Relocation<void(void*)> Call(FunctionID, 0x4);
			trampoline.write_call<6>(Call.address(), &TestHook);
			Function(nullptr);
			REQUIRE(_success);
		}

	private:
		static void TestHook(void*)
		{
			*_successPtr = true;
		}

		static inline bool* _successPtr{ nullptr };
		bool _success{ false };
	};
}

TEST_CASE_METHOD(TrampolineFixture, "Trampoline/DefaultTrampoline", "[.][integration]")
{
	auto& trampoline = SKSE::GetTrampoline();
	trampoline.create(TrampolineSize);
	TestTrampoline(trampoline);
}

TEST_CASE_METHOD(TrampolineFixture, "Trampoline/CustomTrampoline", "[.][integration]")
{
	SKSE::Trampoline trampoline;
	trampoline.create(TrampolineSize);
	TestTrampoline(trampoline);
}