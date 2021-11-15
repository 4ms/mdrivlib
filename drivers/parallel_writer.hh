#pragma once
#include "parallel_writer_conf.hh"
#include "util/base_concepts.hh"

namespace mdrivlib
{

template<Derived<ParallelWriterConf>>
struct ParallelWriter {

	void write_data(uint8_t d) {
		//
	}

	void write_cmd(uint8_t d) {
		//
	}
};
} // namespace mdrivlib
