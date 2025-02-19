#include <cstdint>

namespace RockchipPeriph
{
struct Uart {
	struct LSR {
		uint32_t data_ready : 1;
		uint32_t overrun_err : 1;
		uint32_t parity_err : 1;
		uint32_t framing_err : 1;
		uint32_t break_interrupt : 1;
		uint32_t tx_holding_reg_empty : 1;
		uint32_t tx_empty : 1;
		uint32_t rx_fifo_err : 1;
		uint32_t : 24;
	};

	uint32_t data;
	uint32_t ier;
	uint32_t iir;
	uint32_t lcr;

	uint32_t mcr;
	LSR lsr;
	uint32_t msr;
	uint32_t scr;

	uint32_t _pad0[4];

	uint32_t srbr; /* 0x30 */
	uint32_t _pad1[3];

	uint32_t _pad2[12];

	uint32_t far;	 /* 0x70 */
	uint32_t tfr;	 /* 0x74 */
	uint32_t rfw;	 /* 0x78 */
	uint32_t status; /* 0x7c */

	uint32_t tfl;  /* 0x80 */
	uint32_t rfl;  /* 0x84 */
	uint32_t srr;  /* 0x88 */
	uint32_t srts; /* 0x8c */

	uint32_t sbcr;	/* 0x90 */
	uint32_t sdmam; /* 0x94 */
	uint32_t sfe;	/* 0x98 */
	uint32_t srt;	/* 0x9c */

	uint32_t stet;	/* 0xa0 */
	uint32_t htx;	/* 0xa4 */
	uint32_t dmasa; /* 0xa8 */
	uint32_t _pad3; /* 0xac */

	uint32_t _pad4[16];

	uint32_t _pad5; /* 0xf0 */
	uint32_t cpr;	/* 0xf4 */
	uint32_t ucv;	/* 0xf8 */
	uint32_t ctr;	/* 0xfc */
};

} // namespace RockchipPeriph

namespace HW
{

static inline volatile RockchipPeriph::Uart *const UART0 = reinterpret_cast<RockchipPeriph::Uart *>(0xfdd50000);
static inline volatile RockchipPeriph::Uart *const UART1 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe650000);
static inline volatile RockchipPeriph::Uart *const UART2 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe660000);
static inline volatile RockchipPeriph::Uart *const UART3 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe670000);
static inline volatile RockchipPeriph::Uart *const UART4 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe680000);
static inline volatile RockchipPeriph::Uart *const UART5 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe690000);
static inline volatile RockchipPeriph::Uart *const UART6 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe6a0000);
static inline volatile RockchipPeriph::Uart *const UART7 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe6b0000);
static inline volatile RockchipPeriph::Uart *const UART8 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe6c0000);
static inline volatile RockchipPeriph::Uart *const UART9 = reinterpret_cast<RockchipPeriph::Uart *>(0xfe6d0000);

}; // namespace HW
