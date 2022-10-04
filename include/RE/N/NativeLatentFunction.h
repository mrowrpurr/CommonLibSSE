#pragma once

#include "RE/B/BSCoreTypes.h"
#include "RE/B/BSTSmartPointer.h"
#include "RE/I/IStackCallbackFunctor.h"
#include "RE/N/NativeFunction.h"

namespace RE
{
	namespace BSScript
	{
		template <class R>
		struct LatentPromise;

		template <class R = void, class = std::enable_if_t<is_valid_return_v<R>>>
		struct LatentResult : std::coroutine_handle<LatentPromise<R>>
		{
			using super = std::coroutine_handle<LatentPromise<R>>;
			using result_type = R;
			using promise_type = LatentPromise<R>;

			constexpr LatentResult() noexcept = default;

			LatentResult(const super& a_other) :
				super(a_other) {}

			LatentResult(super&& a_other) :
				super(std::forward<super>(a_other)) {}

			operator bool() const noexcept { return true; }
		};

		struct LatentPromiseBase
		{
			LatentPromiseBase(IVirtualMachine* a_vm, VMStackID a_stackID) :
				vm(a_vm), stackID(a_stackID)
			{
			}

			virtual ~LatentPromiseBase()
			{
				if (!returned) {
					ReturnFromLatent();
				}
			}

			virtual Variable GetResult() = 0;

			void ReturnFromLatent()
			{
				returned = true;
				vm->ReturnFromLatent(stackID, GetResult());
			}

			std::suspend_never initial_suspend() { return {}; }

			std::suspend_always final_suspend() noexcept { return {}; }

			void unhandled_exception() { exception = std::current_exception(); }

			IVirtualMachine*   vm;
			VMStackID          stackID;
			bool               returned{ false };
			std::exception_ptr exception;
		};

		template <class R>
		struct LatentPromise final : public LatentPromiseBase
		{
			template <class... Args>
			LatentPromise(IVirtualMachine* a_vm, VMStackID a_stackID, Args...) :
				LatentPromiseBase(a_vm, a_stackID)
			{
			}

			Variable GetResult() override
			{
				Variable var;
				var.Pack(value);
				return var;
			}

			LatentResult<R> get_return_object() { return LatentResult<R>::from_promise(*this); }

			void return_value(R&& a_value)
			{
				value = std::forward<R>(a_value);
				ReturnFromLatent();
			}

			R value;
		};

		template <>
		struct LatentPromise<void> final : public LatentPromiseBase
		{
			template <class... Args>
			LatentPromise(IVirtualMachine* a_vm, VMStackID a_stackID, Args...) :
				LatentPromiseBase(a_vm, a_stackID)
			{
			}

			Variable GetResult() override { return {}; }

			LatentResult<> get_return_object() { return LatentResult<>::from_promise(*this); }

			void return_void() { ReturnFromLatent(); }
		};

		template <class F, class R, class Base, class... Args>
		class NativeLatentFunction : public NativeFunction<true, F, bool, Base, Args...>
		{
		private:
			using super = NativeFunction<true, F, bool, Base, Args...>;

		public:
			using result_type = typename R::result_type;
			using base_type = typename super::base_type;
			using function_type = typename super::function_type;

			using super::super;

			NativeLatentFunction(std::string_view a_fnName, std::string_view a_className, function_type a_callback) :
				super(a_fnName, a_className, a_callback)
			{
				super::_retType = GetRawType<result_type>();
				super::_isLatent = true;
			}
		};
	}

	template <class Int, class R, class Cls, class... Args>
	class NativeFunction<R(BSScript::IVirtualMachine*, Int, Cls, Args...), std::enable_if_t<BSScript::is_valid_latent_sig_v<Int, R, Cls, Args...>>> :
		public BSScript::NativeLatentFunction<R(BSScript::IVirtualMachine*, Int, Cls, Args...), R, Cls, Args...>
	{
	private:
		using super = BSScript::NativeLatentFunction<R(BSScript::IVirtualMachine*, Int, Cls, Args...), R, Cls, Args...>;

	public:
		using result_type = typename super::result_type;
		using base_type = typename super::base_type;
		using function_type = typename super::function_type;

		using super::super;
	};
}
