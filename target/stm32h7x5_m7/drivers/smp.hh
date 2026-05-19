#pragma once
#include <cstdint>
#include <functional>

namespace mdrivlib
{

struct SMPControl {
	static constexpr uint32_t NumCores = 1;

	template<uint32_t channel>
	static void notify() {
	}

	template<uint32_t channel = 31>
	static void write(uint32_t) {
	}

	static void write(uint32_t, uint32_t) {
	}

	template<uint32_t channel = 31>
	static uint32_t read() {
		return 0;
	}
};

struct SMPThread {
	static void launch(std::function<void()> entry) {
	}

	template<typename Fn>
	static auto run(Fn fn) -> std::invoke_result_t<Fn> {
		using R = std::invoke_result_t<Fn>;

		if constexpr (std::is_void_v<R>)
			return;
		else
			return R{};
	}

	template<uint32_t command_id, uint32_t data_reg = 0>
	static void launch_command(uint32_t) {
	}

	template<uint32_t command_id>
	static void split_with_command() {
	}
	static void join() {
	}
	static void execute() {
	}
	static bool is_running() {
		return true;
	}
	static void init() {
	}
};
} // namespace mdrivlib
