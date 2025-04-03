#include "doctest.h"

#ifndef RK3566
#define RK3566
#endif

#include "drivers/i2c.hh"

TEST_CASE("Transmit buffer sizes") {
	mdrivlib::I2CConfig conf{.I2C_periph_num = 2};

	mdrivlib::I2CPeriph periph{conf};

	std::array<uint8_t, 128> data;

	// {
	// 	// Write 0 data bytes
	mdrivlib::I2C2->test_tx_log.clear();
	// 	data[0] = 0x44;
	// 	periph.mem_write(0x22, 0x33, 1, data.data(), 0);

	// 	CHECK(mdrivlib::I2C2->test_tx_log.size() == 1);
	// 	CHECK(mdrivlib::I2C2->test_tx_log[0] == 0x3322);
	// }
	{
		// Write 1 data byte
		mdrivlib::I2C2->test_tx_log.clear();

		data[0] = 0x44;
		periph.mem_write(0x22, 0x33, 1, data.data(), 1);

		CHECK(mdrivlib::I2C2->test_tx_log.size() == 1);
		CHECK(mdrivlib::I2C2->test_tx_log[0] == 0x443322);
	}
	{
		// Write 32 total bytes
		mdrivlib::I2C2->test_tx_log.clear();

		unsigned i = 0x04;
		for (auto &d : data)
			d = i++;
		periph.mem_write(0x02, 0x03, 1, data.data(), 30);

		CHECK(mdrivlib::I2C2->test_tx_log.size() == 8);
		uint32_t startval = 0x05040302;
		for (auto i = 0; i < 8; i++) {
			CHECK(mdrivlib::I2C2->test_tx_log[i] == 0x04040404 * i + startval);
		}
	}
	{
		// Write 33 total bytes (filled up FIFO + 1 more)
		mdrivlib::I2C2->test_tx_log.clear();

		unsigned i = 0x04;
		for (auto &d : data)
			d = i++;
		periph.mem_write(0x02, 0x03, 1, data.data(), 31);

		CHECK(mdrivlib::I2C2->test_tx_log.size() == 9);
		uint32_t startval = 0x05040302;
		for (auto i = 0; i < 8; i++) {
			CHECK(mdrivlib::I2C2->test_tx_log[i] == 0x04040404 * i + startval);
		}

		CHECK(mdrivlib::I2C2->test_tx_log[8] == 0x00000022);
	}
	{
		printf("\n\nWrite 65\n");
		// Write 65 data bytes (two full FIFOs + 1 more)
		mdrivlib::I2C2->test_tx_log.clear();

		unsigned i = 6;
		for (auto &d : data)
			d = i++ / 4;
		periph.mem_write(0x01, 0x01, 1, data.data(), 63);

		CHECK(mdrivlib::I2C2->test_tx_log.size() == 17); //16 * 4 = 64, + 1 more for last byte
		CHECK(mdrivlib::I2C2->test_tx_log[0] == 0x01010101);
		CHECK(mdrivlib::I2C2->test_tx_log[1] == 0x02020202);
		CHECK(mdrivlib::I2C2->test_tx_log[2] == 0x03030303);
		CHECK(mdrivlib::I2C2->test_tx_log[3] == 0x04040404);
		CHECK(mdrivlib::I2C2->test_tx_log[4] == 0x05050505);
		CHECK(mdrivlib::I2C2->test_tx_log[5] == 0x06060606);
		CHECK(mdrivlib::I2C2->test_tx_log[6] == 0x07070707);
		CHECK(mdrivlib::I2C2->test_tx_log[7] == 0x08080808);

		// FAIL:
		CHECK(mdrivlib::I2C2->test_tx_log[8] == 0x09090909); //0x00000909

		// CHECK(mdrivlib::I2C2->test_tx_log[8] == 0x00000022);
	}
}
