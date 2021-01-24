#pragma once

// requires SOURCE::get_val(int)-->T, POSTFILTER::add_val(T), POSTFILTER::val()-->FilteredT
template<typename SOURCE, int NUM_CHANS, typename POSTFILTER>
struct AnalogIn : SOURCE {
	AnalogIn() {}

	template<typename T>
	AnalogIn(T init)
		: SOURCE{std::move(init)} {}

	void read(int chan) { oversampler_[chan].add_val(this->get_val(chan)); }
	auto get(int chan) { return oversampler_[chan].val(); }

private:
	POSTFILTER oversampler_[NUM_CHANS];
};