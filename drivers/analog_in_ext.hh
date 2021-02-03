#pragma once
#include <algorithm>

// Todo: is this necessary?

// requires SOURCE::get_val(int chan)-->int, POSTFILTER::add_val(int val), POSTFILTER::val()-->FilteredT
template<typename SOURCE, int NUM_CHANS, typename POSTFILTER>
struct AnalogIn : SOURCE {
	AnalogIn() {
	}

	template<typename T>
	AnalogIn(T &&init)
		: SOURCE{std::move(init)} {
	}

	void read(int chan) {
		oversampler_[chan].add_val(this->get_val(chan));
	}
	void read() {
		for (int chan = 0; chan < NUM_CHANS; chan++)
			read(chan);
	}
	auto get(int chan) {
		return oversampler_[chan].val();
	}

private:
	POSTFILTER oversampler_[NUM_CHANS];
};
