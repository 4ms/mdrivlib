#pragma once
#include <cstdint>
#include <new> //for placement new
#include <type_traits>
#include <utility>

// Callback is a callable object that takes no parameters,
// and has a maximum of 8 Bytes of captures (on ARM 32-bit system)
// Note: [this] takes 4 Bytes (on ARM 32-bit system), so it's quite useful
// Improvements, possibly: remove m_destroy and make m_data sizeof(void*)
// this would reduce the size (on ARM 32-bit) from 16 Bytes to 8 Bytes
// But it would leak memory if Callable was on the heap
// (which shouldn't happen, since the point of this is to avoid the heap,
// but it could...)

template<unsigned BUFFER_SIZE = 2 * sizeof(void *)>
class CallbackSized {
private:
	// static constexpr unsigned BUFFER_SIZE = 2 * sizeof(void *);

public:
	CallbackSized() = default;

	template<typename Callable>
	CallbackSized(Callable callable) {
		static_assert(sizeof(Callable) <= BUFFER_SIZE);
		static_assert(std::is_invocable_v<Callable>);

		new (&m_data[0]) Callable(callable);
		m_callback = invoke<Callable>;
		m_destroy = destroy<Callable>;
	}

	~CallbackSized() {
		if (m_destroy)
			m_destroy(&m_data[0]);
	}

	void call() {
		// if (m_callback)
		m_callback(&m_data[0]);
		return;
	}

	void operator()() {
		call();
	}

	operator bool() {
		return m_callback;
	}

private:
	template<typename Callable>
	static void invoke(void *object) {
		Callable &callable = *reinterpret_cast<Callable *>(object);
		callable();
	}

	template<typename Callable>
	static void destroy(void *object) {
		Callable &callable = *reinterpret_cast<Callable *>(object);
		callable.~Callable();
	}

private:
	using CallbackM = void (*)(void *);
	CallbackM m_callback{};

	using Deleter = void (*)(void *);
	Deleter m_destroy{};

	alignas(uint64_t) uint8_t m_data[BUFFER_SIZE];
};

using Callback = CallbackSized<2 * sizeof(void *)>;

// Function<T> is a callback that takes parameters and returns something
template<typename Signature>
struct Function {};

template<typename Ret, typename... Args>
class Function<Ret(Args...)> {
private:
	static constexpr uint8_t BUFFER_SIZE = 2 * sizeof(void *);

public:
	Function() = default;

	template<typename Callable>
	Function(Callable callable) {
		static_assert(sizeof(Callable) <= BUFFER_SIZE);
		static_assert(std::is_invocable_v<Callable, Args...>);

		new (&m_data[0]) Callable(callable);
		m_callback = invoke<Callable>;
		m_destroy = destroy<Callable>;
	}

	~Function() {
		if (m_destroy)
			m_destroy(&m_data[0]);
	}

	Ret call(Args... args) {
		if (m_callback)
			return m_callback(&m_data[0], std::forward<Args...>(args)...);
		return Ret();
	}

	Ret operator()(Args... args) {
		return call(std::forward<Args...>(args)...);
	}

private:
	template<typename Callable>
	static Ret invoke(void *object, Args... args) {
		Callable &callable = *reinterpret_cast<Callable *>(object);
		callable(std::forward<Args...>(args)...);
	}

	template<typename Callable>
	static void destroy(void *object) {
		Callable &callable = *reinterpret_cast<Callable *>(object);
		callable.~Callable();
	}

private:
	using Callback = Ret (*)(void *, Args...);
	Callback m_callback{};

	using Deleter = void (*)(void *);
	Deleter m_destroy{};

	alignas(uint64_t) uint8_t m_data[BUFFER_SIZE];
};
