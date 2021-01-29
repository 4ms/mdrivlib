#pragma once
#include "stm32xx.h"
#include "system.hh"

struct DMA_Config {
	DMA_TypeDef *DMAx;
	DMA_Stream_TypeDef *stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
};

struct DMA_LL_Config {
	DMA_TypeDef *DMAx;
	uint32_t stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
};

struct BDMA_Config {
	BDMA_TypeDef *BDMAx;
	BDMA_Channel_TypeDef *stream;
	uint32_t channel;
	IRQn_Type IRQn;
	uint32_t pri;
	uint32_t subpri;
};

// Todo: this should be in some dma manager class

#if defined(BDMA)
template<typename T>
constexpr volatile uint32_t *__HAL_DMA_GET_ISR(T stream) {
	// clang-format off
	uint32_t s = (uint32_t)stream;
	return (s > (uint32_t)DMA2_Stream7) ? &(BDMA->ISR) \
		 : (s > (uint32_t)DMA2_Stream3) ? &(DMA2->HISR) \
		 : (s > (uint32_t)DMA1_Stream7) ? &(DMA2->LISR) \
		 : (s > (uint32_t)DMA1_Stream3) ? &(DMA1->HISR) \
		 : &(DMA1->LISR);
	// clang-format on
}

template<typename T>
constexpr volatile uint32_t *__HAL_DMA_GET_IFCR(T stream) {
	// clang-format off
	uint32_t s = (uint32_t)stream;
	return s > (uint32_t)DMA2_Stream7 ? &(BDMA->IFCR) \
		 : s > (uint32_t)DMA2_Stream3 ? &(DMA2->HIFCR) \
		 : s > (uint32_t)DMA1_Stream7 ? &(DMA2->LIFCR) \
		 : s > (uint32_t)DMA1_Stream3 ? &(DMA1->HIFCR) \
		 : &(DMA1->LIFCR);
	// clang-format on
}

#else
template<typename T>
constexpr volatile uint32_t *__HAL_DMA_GET_ISR(T stream) {
	// clang-format off
	uint32_t s = (uint32_t)stream;
	return (s > (uint32_t)DMA2_Stream3) ? &(DMA2->HISR) \
		 : (s > (uint32_t)DMA1_Stream7) ? &(DMA2->LISR) \
		 : (s > (uint32_t)DMA1_Stream3) ? &(DMA1->HISR) \
		 : &(DMA1->LISR);
	// clang-format on
}

template<typename T>
constexpr volatile uint32_t *__HAL_DMA_GET_IFCR(T stream) {
	// clang-format off
	uint32_t s = (uint32_t)stream;
	return s > (uint32_t)DMA2_Stream3 ? &(DMA2->HIFCR) \
		 : s > (uint32_t)DMA1_Stream7 ? &(DMA2->LIFCR) \
		 : s > (uint32_t)DMA1_Stream3 ? &(DMA1->HIFCR) \
		 : &(DMA1->LIFCR);
	// clang-format on
}
#endif
