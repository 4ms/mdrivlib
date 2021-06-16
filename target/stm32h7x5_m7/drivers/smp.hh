#include <cstdint>
#include <functional>
struct SMPControl {
	template<uint32_t channel>
	static void notify() {
	}

	template<uint32_t channel = 31>
	static void write(uint32_t) {
	}

	template<uint32_t channel = 31>
	static uint32_t read() {
		return 0;
	}
};

struct SMPThread {
	static void run(std::function<void()> entry) {
	}

	static void join() {
	}
	static bool is_running() {
		return true;
	}
	static void init() {
	}
};
