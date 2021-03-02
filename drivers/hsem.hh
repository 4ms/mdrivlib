#pragma once
#include "register_access.hh"
#include "stm32xx.h"

namespace mdrivlib
{

namespace stm32h7x5
{
struct HSEM_ {
	template<uint32_t Mask>
	using IER = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, IER), Mask>;

	template<uint32_t Mask>
	using ICR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ICR), Mask>;

	template<uint32_t Mask>
	using ISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, ISR), Mask>;

	template<uint32_t Mask>
	using MISR = RegisterBits<ReadWrite, HSEM_BASE + offsetof(HSEM_Common_TypeDef, MISR), Mask>;
};
} // namespace stm32h7x5
} // namespace mdrivlib

class HWSemaphore {
	template<uint32_t SemaphoreID>
	static void set() {
	}
};
