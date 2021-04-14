#pragma once
#include "register_access.hh"
#include "stm32xx.h"
#include <variant>

namespace mdrivlib {
namespace stm32mp1 {
namespace core_a7 {
// Todo: if both cores share same data, then put it in _rcc_common.hh and #include it for both
namespace RCC_Enable {
using RegisterDataT = uint32_t;

// Auto-generated:
using ADC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_ADC12EN>;
using ADFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_ADFSDMEN>;
using BKPSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_BKPSRAMEN>;
using BSEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_BSECEN>;
using CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_CECEN>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_CRC1EN>;
using CRC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_CRC2EN>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_CRYP1EN>;
using CRYP2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_CRYP2EN>;
using DAC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_DAC12EN>;
using DCMI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_DCMIEN>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_DDRPERFMEN>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_DFSDMEN>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMA1EN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMA2EN>;
using DMAMUX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_DMAMUXEN>;
using DSI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_DSIEN>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_DTSEN>;
using ETHCK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHCKEN>;
using ETHMAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHMACEN>;
using ETHRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHRXEN>;
using ETHSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6LPENSETR), RCC_MP_AHB6LPENSETR_ETHSTPEN>;
using ETHTX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_ETHTXEN>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_FDCANEN>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_FMCEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOAEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOBEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOCEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIODEN>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOEEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOFEN>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOGEN>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOHEN>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOIEN>;
using GPIOJ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOJEN>;
using GPIOK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB4ENSETR), RCC_MP_AHB4ENSETR_GPIOKEN>;
using GPIOZ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_GPIOZEN>;
using GPU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_GPUEN>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_HASH1EN>;
using HASH2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_HASH2EN>;
using HDP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_HDPEN>;
using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_HSEMEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C1EN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C2EN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C3EN>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_I2C4EN>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_I2C5EN>;
using I2C6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_I2C6EN>;
using IPCC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_IPCCEN>;
using IWDG1APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_IWDG1APBEN>;
using IWDG2APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_IWDG2APBEN>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_LPTIM1EN>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM2EN>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM3EN>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM4EN>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_LPTIM5EN>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_LTDCEN>;
using MDIOS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_MDIOSEN>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_MDMAEN>;
// using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_TZAHB6ENSETR), RCC_MP_TZAHB6ENSETR_MDMAEN>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_QSPIEN>;
using RETRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_MLAHBENSETR), RCC_MP_MLAHBENSETR_RETRAMEN>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_RNG1EN>;
using RNG2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB3ENSETR), RCC_MP_AHB3ENSETR_RNG2EN>;
using RTCAPB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_RTCAPBEN>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI1EN>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI2EN>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SAI3EN>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_SAI4EN>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_SDMMC1EN>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_SDMMC2EN>;
using SDMMC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_SDMMC3EN>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPDIFEN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI1EN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPI2EN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_SPI3EN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI4EN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_SPI5EN>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_SPI6EN>;
using STGENROSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4LPENSETR), RCC_MP_APB4LPENSETR_STGENROSTPEN>;
using STGENRO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_STGENROEN>;
using STGENSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5LPENSETR), RCC_MP_APB5LPENSETR_STGENSTPEN>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_STGENEN>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_SYSCFGEN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM1EN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM2EN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM3EN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM4EN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM5EN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM6EN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM7EN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM8EN>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM12EN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM13EN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_TIM14EN>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM15EN>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM16EN>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_TIM17EN>;
using TZC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZC1EN>;
using TZC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZC2EN>;
using TZPC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_TZPCEN>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART4EN>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART5EN>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART7EN>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_UART8EN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB5ENSETR), RCC_MP_APB5ENSETR_USART1EN>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_USART2EN>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB1ENSETR), RCC_MP_APB1ENSETR_USART3EN>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB2ENSETR), RCC_MP_APB2ENSETR_USART6EN>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB6ENSETR), RCC_MP_AHB6ENSETR_USBHEN>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB2ENSETR), RCC_MP_AHB2ENSETR_USBOEN>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB4ENSETR), RCC_MP_APB4ENSETR_USBPHYEN>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_APB3ENSETR), RCC_MP_APB3ENSETR_VREFEN>;

// Aliases:

using ADC1_ = ADC12_;
using ADC2_ = ADC12_;

// Blanks:

using ADC3_ = NonexistantRegister;
using BDMA_ = NonexistantRegister;
using TIM9_ = NonexistantRegister;
using TIM10_ = NonexistantRegister;
using TIM11_ = NonexistantRegister;

// special-case: GPIO port base address can be used to calc bit-offset of RCC enable bit
struct GPIO {
  static inline volatile RegisterDataT *const _reg = &(RCC->MC_AHB4ENSETR);

  static uint32_t get_gpio_bit(RegisterDataT periph) { return 1 << ((periph & 0x00003C00) >> 10); }
  static void enable(GPIO_TypeDef *periph) {
    if (periph == GPIOZ)
      GPIOZ_::set();
    else
      *_reg = *_reg | get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
    [[maybe_unused]] bool delay_after_enabling = is_enabled(periph);
  }
  static void disable(GPIO_TypeDef *periph) {
    if (periph == GPIOZ)
      GPIOZ_::clear();
    else
      *_reg = *_reg & ~get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
    [[maybe_unused]] bool delay_after_disabling = is_enabled(periph);
  }
  static bool is_enabled(GPIO_TypeDef *periph) {
    if (periph == GPIOZ)
      return GPIOZ_::read();
    else
      return (*_reg) & get_gpio_bit(reinterpret_cast<RegisterDataT>(periph));
  }
};

using SAI_variant = std::variant<SAI1_, SAI2_, SAI3_, SAI4_>;
template <unsigned N> using SAI = typename std::variant_alternative_t<N - 1, SAI_variant>;

using SPI_variant = std::variant<SPI1_, SPI2_, SPI3_, SPI4_, SPI5_, SPI6_>;
template <unsigned N> using SPI = typename std::variant_alternative_t<N - 1, SPI_variant>;

} // namespace RCC_Enable

namespace RCC_Reset {
// Auto-generated:
// Todo: Fix commented-out lines
using ADC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_ADC12RST>;
// using ADFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_ADFSDMRST>;
// using BKPSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_BKPSRAMRST>;
// using BSEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_BSECRST>;
using CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_CECRST>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_CRC1RST>;
using CRC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_CRC2RST>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_CRYP1RST>;
using CRYP2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_CRYP2RST>;
using DAC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_DAC12RST>;
using DCMI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_DCMIRST>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_DDRPERFMRST>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_DFSDMRST>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMA1RST>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMA2RST>;
using DMAMUX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_DMAMUXRST>;
using DSI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_DSIRST>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_DTSRST>;
// using ETHCK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETHCKRST>;
using ETHMAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETHMACRST>;
// using ETHRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETHRXRST>;
// using ETHSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6LPRSTSETR), RCC_AHB6LPRSTSETR_ETHSTPRST>;
// using ETHTX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_ETHTXRST>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_FDCANRST>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_FMCRST>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOARST>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOBRST>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOCRST>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIODRST>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOERST>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOFRST>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOGRST>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOHRST>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOIRST>;
using GPIOJ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOJRST>;
using GPIOK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB4RSTSETR), RCC_AHB4RSTSETR_GPIOKRST>;
using GPIOZ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_GPIOZRST>;
using GPU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_GPURST>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_HASH1RST>;
using HASH2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_HASH2RST>;
// using HDP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_HDPRST>;
using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_HSEMRST>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C1RST>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C2RST>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C3RST>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_I2C4RST>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_I2C5RST>;
using I2C6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_I2C6RST>;
using IPCC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_IPCCRST>;
// using IWDG1APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_IWDG1APBRST>;
// using IWDG2APB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_IWDG2APBRST>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_LPTIM1RST>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM2RST>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM3RST>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM4RST>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_LPTIM5RST>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_LTDCRST>;
using MDIOS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_MDIOSRST>;
// using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_MDMARST>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, TZAHB6RSTSETR), RCC_TZAHB6RSTSETR_MDMARST>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_QSPIRST>;
// using RETRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MLAHBRSTSETR), RCC_MLAHBRSTSETR_RETRAMRST>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB5RSTSETR), RCC_AHB5RSTSETR_RNG1RST>;
using RNG2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB3RSTSETR), RCC_AHB3RSTSETR_RNG2RST>;
// using RTCAPB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_RTCAPBRST>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SAI1RST>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SAI2RST>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SAI3RST>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_SAI4RST>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_SDMMC1RST>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_SDMMC2RST>;
using SDMMC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_SDMMC3RST>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPDIFRST>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SPI1RST>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPI2RST>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_SPI3RST>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SPI4RST>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_SPI5RST>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_SPI6RST>;
// using STGENROSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4LPRSTSETR), RCC_APB4LPRSTSETR_STGENROSTPRST>;
// using STGENRO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_STGENRORST>;
// using STGRSTSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5LPRSTSETR), RCC_APB5LPRSTSETR_STGRSTSTPRST>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_STGENRST>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_SYSCFGRST>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM1RST>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM2RST>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM3RST>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM4RST>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM5RST>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM6RST>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM7RST>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM8RST>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM12RST>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM13RST>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_TIM14RST>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM15RST>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM16RST>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_TIM17RST>;
// using TZC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_TZC1RST>;
// using TZC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_TZC2RST>;
// using TZPC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_TZPCRST>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART4RST>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART5RST>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART7RST>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_UART8RST>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB5RSTSETR), RCC_APB5RSTSETR_USART1RST>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_USART2RST>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB1RSTSETR), RCC_APB1RSTSETR_USART3RST>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB2RSTSETR), RCC_APB2RSTSETR_USART6RST>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB6RSTSETR), RCC_AHB6RSTSETR_USBHRST>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, AHB2RSTSETR), RCC_AHB2RSTSETR_USBORST>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB4RSTSETR), RCC_APB4RSTSETR_USBPHYRST>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, APB3RSTSETR), RCC_APB3RSTSETR_VREFRST>;
} // namespace RCC_Reset

namespace RCC_Clocks {
/*
using CR = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), 0xFFFFFFFF>;
using D1CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D1CKRDY>;
using D2CKREADY = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, CR), RCC_CR_D2CKRDY>;
*/
} // namespace RCC_Clocks

} // namespace core_a7

namespace core_m4 {
namespace RCC_Enable {
using ADC12LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_ADC12LPEN>;
using ADC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_ADC12EN>;
using ADFSDMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_ADFSDMLPEN>;
using ADFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_ADFSDMEN>;
using AXIMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MP_AHB5ENSETR), RCC_MP_AHB5ENSETR_AXIMCEN>;
using BKPSRAMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5LPENSETR), RCC_MC_AHB5LPENSETR_BKPSRAMLPEN>;
using BKPSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_BKPSRAMEN>;
using BSECLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_BSECLPEN>;
using BSEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_BSECEN>;
using CECLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_CECLPEN>;
using CEC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_CECEN>;
using CRC1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_CRC1LPEN>;
using CRC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_CRC1EN>;
using CRC2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_CRC2LPEN>;
using CRC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_CRC2EN>;
using CRYP1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5LPENSETR), RCC_MC_AHB5LPENSETR_CRYP1LPEN>;
using CRYP1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_CRYP1EN>;
using CRYP2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_CRYP2LPEN>;
using CRYP2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_CRYP2EN>;
using DAC12LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_DAC12LPEN>;
using DAC12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_DAC12EN>;
using DCMILP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_DCMILPEN>;
using DCMI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_DCMIEN>;
using DDRPERFMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_DDRPERFMLPEN>;
using DDRPERFM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4ENSETR), RCC_MC_APB4ENSETR_DDRPERFMEN>;
using DFSDMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_DFSDMLPEN>;
using DFSDM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_DFSDMEN>;
using DMA1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_DMA1LPEN>;
using DMA1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_DMA1EN>;
using DMA2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_DMA2LPEN>;
using DMA2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_DMA2EN>;
using DMAMUXLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_DMAMUXLPEN>;
using DMAMUX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_DMAMUXEN>;
using DSILP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_DSILPEN>;
using DSI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4ENSETR), RCC_MC_APB4ENSETR_DSIEN>;
using DTSLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_DTSLPEN>;
using DTS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_DTSEN>;
using ETHCKLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_ETHCKLPEN>;
using ETHCK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_ETHCKEN>;
using ETHMACLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_ETHMACLPEN>;
using ETHMAC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_ETHMACEN>;
using ETHRXLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_ETHRXLPEN>;
using ETHRX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_ETHRXEN>;
using ETHSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_ETHSTPEN>;
using ETHTXLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_ETHTXLPEN>;
using ETHTX_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_ETHTXEN>;
using FDCANLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_FDCANLPEN>;
using FDCAN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_FDCANEN>;
using FMCLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_FMCLPEN>;
using FMC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_FMCEN>;
using GPIOALP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOALPEN>;
using GPIOA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOAEN>;
using GPIOBLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOBLPEN>;
using GPIOB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOBEN>;
using GPIOCLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOCLPEN>;
using GPIOC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOCEN>;
using GPIODLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIODLPEN>;
using GPIOD_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIODEN>;
using GPIOELP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOELPEN>;
using GPIOE_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOEEN>;
using GPIOFLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOFLPEN>;
using GPIOF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOFEN>;
using GPIOGLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOGLPEN>;
using GPIOG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOGEN>;
using GPIOHLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOHLPEN>;
using GPIOH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOHEN>;
using GPIOILP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOILPEN>;
using GPIOI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOIEN>;
using GPIOJLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOJLPEN>;
using GPIOJ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOJEN>;
using GPIOKLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4LPENSETR), RCC_MC_AHB4LPENSETR_GPIOKLPEN>;
using GPIOK_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB4ENSETR), RCC_MC_AHB4ENSETR_GPIOKEN>;
using GPIOZLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5LPENSETR), RCC_MC_AHB5LPENSETR_GPIOZLPEN>;
using GPIOZ_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_GPIOZEN>;
using GPULP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_GPULPEN>;
using GPU_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_GPUEN>;
using HASH1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5LPENSETR), RCC_MC_AHB5LPENSETR_HASH1LPEN>;
using HASH1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_HASH1EN>;
using HASH2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_HASH2LPEN>;
using HASH2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_HASH2EN>;
using HDP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_HDPEN>;
using HSEMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_HSEMLPEN>;
using HSEM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_HSEMEN>;
using I2C1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_I2C1LPEN>;
using I2C1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_I2C1EN>;
using I2C2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_I2C2LPEN>;
using I2C2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_I2C2EN>;
using I2C3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_I2C3LPEN>;
using I2C3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_I2C3EN>;
using I2C4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_I2C4LPEN>;
using I2C4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_I2C4EN>;
using I2C5LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_I2C5LPEN>;
using I2C5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_I2C5EN>;
using I2C6LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_I2C6LPEN>;
using I2C6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_I2C6EN>;
using IPCCLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_IPCCLPEN>;
using IPCC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_IPCCEN>;
using LPTIM1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_LPTIM1LPEN>;
using LPTIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_LPTIM1EN>;
using LPTIM2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_LPTIM2LPEN>;
using LPTIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_LPTIM2EN>;
using LPTIM3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_LPTIM3LPEN>;
using LPTIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_LPTIM3EN>;
using LPTIM4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_LPTIM4LPEN>;
using LPTIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_LPTIM4EN>;
using LPTIM5LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_LPTIM5LPEN>;
using LPTIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_LPTIM5EN>;
using LTDCLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_LTDCLPEN>;
using LTDC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4ENSETR), RCC_MC_APB4ENSETR_LTDCEN>;
using MDIOSLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_MDIOSLPEN>;
using MDIOS_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_MDIOSEN>;
using MDMALP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_MDMALPEN>;
using MDMA_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_MDMAEN>;
using QSPILP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_QSPILPEN>;
using QSPI_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_QSPIEN>;
using RETRAMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_MLAHBLPENSETR), RCC_MC_MLAHBLPENSETR_RETRAMLPEN>;
using RETRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_MLAHBENSETR), RCC_MC_MLAHBENSETR_RETRAMEN>;
using RNG1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5LPENSETR), RCC_MC_AHB5LPENSETR_RNG1LPEN>;
using RNG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB5ENSETR), RCC_MC_AHB5ENSETR_RNG1EN>;
using RNG2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3LPENSETR), RCC_MC_AHB3LPENSETR_RNG2LPEN>;
using RNG2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB3ENSETR), RCC_MC_AHB3ENSETR_RNG2EN>;
using RTCAPBLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_RTCAPBLPEN>;
using RTCAPB_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_RTCAPBEN>;
using SAI1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SAI1LPEN>;
using SAI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SAI1EN>;
using SAI2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SAI2LPEN>;
using SAI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SAI2EN>;
using SAI3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SAI3LPEN>;
using SAI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SAI3EN>;
using SAI4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_SAI4LPEN>;
using SAI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_SAI4EN>;
using SDMMC1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_SDMMC1LPEN>;
using SDMMC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_SDMMC1EN>;
using SDMMC2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_SDMMC2LPEN>;
using SDMMC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_SDMMC2EN>;
using SDMMC3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_SDMMC3LPEN>;
using SDMMC3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_SDMMC3EN>;
using SPDIFLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_SPDIFLPEN>;
using SPDIF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_SPDIFEN>;
using SPI1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SPI1LPEN>;
using SPI1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SPI1EN>;
using SPI2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_SPI2LPEN>;
using SPI2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_SPI2EN>;
using SPI3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_SPI3LPEN>;
using SPI3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_SPI3EN>;
using SPI4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SPI4LPEN>;
using SPI4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SPI4EN>;
using SPI5LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_SPI5LPEN>;
using SPI5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_SPI5EN>;
using SPI6LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_SPI6LPEN>;
using SPI6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_SPI6EN>;
using SRAM1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_MLAHBLPENSETR), RCC_MC_MLAHBLPENSETR_SRAM1LPEN>;
using SRAM2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_MLAHBLPENSETR), RCC_MC_MLAHBLPENSETR_SRAM2LPEN>;
using SRAM34LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_MLAHBLPENSETR), RCC_MC_MLAHBLPENSETR_SRAM34LPEN>;
using STGENLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_STGENLPEN>;
using STGENROLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_STGENROLPEN>;
using STGENROSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_STGENROSTPEN>;
using STGENRO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4ENSETR), RCC_MC_APB4ENSETR_STGENROEN>;
using STGENSTP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_STGENSTPEN>;
using STGEN_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_STGENEN>;
using SYSCFGLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_SYSCFGLPEN>;
using SYSCFG_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_SYSCFGEN>;
using SYSRAMLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AXIMLPENSETR), RCC_MC_AXIMLPENSETR_SYSRAMLPEN>;
using SYSRAM_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AXIMENSETR), RCC_MC_AXIMENSETR_SYSRAMEN>;
using TIM12LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM12LPEN>;
using TIM12_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM12EN>;
using TIM13LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM13LPEN>;
using TIM13_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM13EN>;
using TIM14LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM14LPEN>;
using TIM14_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM14EN>;
using TIM15LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_TIM15LPEN>;
using TIM15_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_TIM15EN>;
using TIM16LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_TIM16LPEN>;
using TIM16_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_TIM16EN>;
using TIM17LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_TIM17LPEN>;
using TIM17_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_TIM17EN>;
using TIM1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_TIM1LPEN>;
using TIM1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_TIM1EN>;
using TIM2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM2LPEN>;
using TIM2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM2EN>;
using TIM3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM3LPEN>;
using TIM3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM3EN>;
using TIM4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM4LPEN>;
using TIM4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM4EN>;
using TIM5LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM5LPEN>;
using TIM5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM5EN>;
using TIM6LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM6LPEN>;
using TIM6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM6EN>;
using TIM7LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_TIM7LPEN>;
using TIM7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_TIM7EN>;
using TIM8LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_TIM8LPEN>;
using TIM8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_TIM8EN>;
using TZC1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_TZC1LPEN>;
using TZC1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_TZC1EN>;
using TZC2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_TZC2LPEN>;
using TZC2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_TZC2EN>;
using TZPCLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_TZPCLPEN>;
using TZPC_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_TZPCEN>;
using UART4LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_UART4LPEN>;
using UART4_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_UART4EN>;
using UART5LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_UART5LPEN>;
using UART5_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_UART5EN>;
using UART7LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_UART7LPEN>;
using UART7_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_UART7EN>;
using UART8LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_UART8LPEN>;
using UART8_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_UART8EN>;
using USART1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5LPENSETR), RCC_MC_APB5LPENSETR_USART1LPEN>;
using USART1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB5ENSETR), RCC_MC_APB5ENSETR_USART1EN>;
using USART2LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_USART2LPEN>;
using USART2_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_USART2EN>;
using USART3LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_USART3LPEN>;
using USART3_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_USART3EN>;
using USART6LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2LPENSETR), RCC_MC_APB2LPENSETR_USART6LPEN>;
using USART6_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB2ENSETR), RCC_MC_APB2ENSETR_USART6EN>;
using USBHLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6LPENSETR), RCC_MC_AHB6LPENSETR_USBHLPEN>;
using USBH_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB6ENSETR), RCC_MC_AHB6ENSETR_USBHEN>;
using USBOLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2LPENSETR), RCC_MC_AHB2LPENSETR_USBOLPEN>;
using USBO_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_AHB2ENSETR), RCC_MC_AHB2ENSETR_USBOEN>;
using USBPHYLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4LPENSETR), RCC_MC_APB4LPENSETR_USBPHYLPEN>;
using USBPHY_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB4ENSETR), RCC_MC_APB4ENSETR_USBPHYEN>;
using VREFLP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3LPENSETR), RCC_MC_APB3LPENSETR_VREFLPEN>;
using VREF_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB3ENSETR), RCC_MC_APB3ENSETR_VREFEN>;
using WWDG1LP_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1LPENSETR), RCC_MC_APB1LPENSETR_WWDG1LPEN>;
using WWDG1_ = RegisterBits<ReadWrite, RCC_BASE + offsetof(RCC_TypeDef, MC_APB1ENSETR), RCC_MC_APB1ENSETR_WWDG1EN>;
} // namespace RCC_Enable
} // namespace core_m4
} // namespace stm32mp1
} // namespace mdrivlib
