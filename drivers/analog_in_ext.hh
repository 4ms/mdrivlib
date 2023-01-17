#pragma once
#include <utility>

// template<typename AdcSourceT, typename FilterT, typename RawValT, typename FilteredValT>
// concept AdcSourceC = requires(AdcSourceT adc, FilterT filt, RawValT raw, unsigned u) {
// 						 // clang-format off
// 	{ adc.get_val(u) } -> std::convertible_to<RawValT>;
// 	filt.add_val(raw);
// 	{ filt.val() } -> std::convertible_to<FilteredValT>;
// 						 // clang-format on
// 					 };
// requires SOURCE::get_val(unsigned chan)-->UnfiltT, POSTFILTER::add_val(UnfiltT val), POSTFILTER::val()-->FilteredT

template<typename AdcSource, unsigned NumChans, typename PostFilter>
struct AnalogIn : AdcSource {
	AnalogIn() = default;

	template<typename... Ts>
	AnalogIn(Ts &&...init)
		: AdcSource{std::forward<Ts>(init)...} {
	}

	void read(unsigned chan) {
		_postfilter[chan].add_val(this->get_val(chan));
	}
	void read() {
		for (unsigned chan = 0; chan < NumChans; chan++)
			read(chan);
	}
	auto get(unsigned chan) {
		return _postfilter[chan].val();
	}

private:
	PostFilter _postfilter[NumChans];
};
