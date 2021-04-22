# mdrivlib

## C++20 Embedded driver library

### Providing high-level functionality using modern C++, with an emphasis on higher-end microcontrollers and microprocessors

This is a work in progress, the API is in flux. Contributions, feedback, and issues are welcome.

### Devices supported:

*Cortex M7*
    * STM32H745/55/47/57 (dual-core M7/M4): full support, except built-in ADC
    * STM32F7xx (especially STM32F746): partial support

*Cortex A7*
  * STM32MP157 (dual-core A7 + M4): partial support, but adding features daily.

## Goals:

  * Easy to use, difficult to misuse.
  * Provide modern C++20 classes for setting up and accessing internal peripherals and common external chips.
  * Provide high-level functionality for common use cases in embedded projects (debouncing switches, wear-leveling, RGB color blending).
  * Allow use of lambdas as callbacks (e.g. interrupt request handlers) with minimal overhead
  * Does not depend on dynamic memory (but provides some structures if you use it)
  * Bare-metal: does not depend on a RTOS
  * Priortize peripheral access efficiency over one-time cost of peripheral setup.
  * Unit tests (using doctest)
  * Example code (TODO: needs work!)

## Status of Drivers:

Peripheral drivers:

| Peripheral | HAL      | Config | Notes |
|:-----------|----------|--------|-------|
| adc        | LL       | (todo) | Circular DMA only. F7 only. |
| i2c        | STM32-HAL| const  | Blocking, IT, DMA modes |
| mpu        | CMSIS    |  -     | For disabling D-Cache on a memory region, Cortex-M only|
| rcc        | CMSIS


  * `adc_builtin_driver.cc`: `AdcPeriph` and `AdcChan`.
      AdcPeriph has a template param representing ADC1, ADC2, ADC3, etc. AdcChan has a template parameter for the AdcPeriph type, and the channel number.
	  This is meant to be used with a DMA continuously (circular mode).
	  This driver is very specific to a certain type of ADC found on some F7 chips (F769, F765, F746)
      - Not using target:: namespace (TODO)
	  - Not using Configuration Structs, except the DMA config (TODO)
	  - Using ST's LL
	
  * i2c: Thin wrapper over ST's I2C HAL.
 	  - Uses ST's HAL

  * mpu: allows you to disable the data cache for a region of memory (useful for DMA buffers). That's all it does so far.
      - Uses target:: namespace system
	  - Uses CMSIS header
	  
  * rcc: uses the RegisterBits template method to enable/disable peripherals using the RCC peripheral.
      - Uses target:: namespace system
	  - Uses CMSIS header
	  - 
  * sai: full duplex audio
  * sdram
  * spi: supports only master-mode. Supports multiple 'slave' chips.
  * system: clock setup, startup (big TODO to support more targets)
  * tim: basic init
  * tim_pwm: provide PWM outputs using TIM (built-in TIM pins only)
  * uart
 
Chip-specific drivers:
  * adc_i2c_max11645
  * adc_spi_max11666
  * codec_WM8731
  * pca9685_led_driver
  * qspi_flash_driver: most of these chips are all the same

Higher-level drivers (wraps a periperal	with a useful interface):
  * analog_in
  * analog_in_ext: external analog input (external ADC chip)
  * codec
  * debounced_switch
  * frame_buffer_led
  * hal_callback
  * interrupt: Allows you to set any callable object to respond to any ISR. Adds only 6 CPU instructions of latency.
  * led_driver
  * pin: general purpose GPIO interface. Lightweight: constructing a pin sets the GPIO register settings, and then the pin object can be discarded if it doesn't need to be accessed directly (i.e. if a peripheral controls it). Or the object can be kept and used to set or read the pin level.
  * rotary: encoder
  * tim_pwm: 
  * timekeeper: setup a timekeeper that excecutes a callable object (function, lambda, etc) that you provide. Easy way to make "threads"


Utilities:
  * colors
  * rgbled
  * qspi_flash_cellblock
  * register_access: defines the RegisterBits templates

## Usage

Add `mdrivlib/drivers` and `mdrivlib/drivers/target/XXX/drivers/` as INCLUDE dirs in your build system.

Compile and link any `mdrivlib/drivers/*.cc` and `mdrivlib/drivers/target/XXX/drivers/*.cc` that you wish to use.

Build with the proper symbols defined for your target: (TODO: specify these clearly in one place)
example: `-DSTM32H7x5 -CORE_CM7`

Use it:

```
#include "pin.hh"

Pin myRedLED {GPIO::A, 2, PinMode::Output};

myRedLED.high();
myRedLED.toggle(); 
```



## Common themes throughout drivers:


### STM32-HAL vs STM32-LL vs CMSIS

mdrivlib contains a mix of these three methods of hardware access:

   - STM32-HAL (`stm32xxxxx_XXX_hal.c/.h` files): The STM32 HAL library is used only when the peripheral is complex enough that re-writing it would be non-trivial *and* offer very little benefit in terms of efficiency, code space, or additional features.
   - STM32-LL (`stm32xxxxx_XXX_ll.c/.h` files): In some drivers, ST's LL drivers are used, which are thin wrappers over direct register control via CMSIS headers. These are used when
   - CMSIS (`stm32xxxxx.h` files): These are huge (2MB+) header files provided by the vendor (ST) that map all the registers to C-style structs and macros. The benefit of these is that the datasheet (Reference Manual) can be used along-side to setup or debug peripherals. For type safety I often use register classes called `RegisterBits` and `RegisterSection`. These can be specified as ReadWrite, ReadOnly, or WriteOnly, making this a bit safer to use than raw CMSIS register macros, while still compiling to very efficient assembly.



### Configuration Structs

These are structs found in files named `*_config_struct.hh`. They let you define board-specific things like the pins to use, peripheral number, DMA settings, etc. Your project should pass them to the constructor of the driver (either as a template parameter or constructor parameter).
  
  - Todo: move all `config_struct.hh` files to a `conf_struct/` dir
  - Todo: create conf structs for adc_builtin, ... (others)

There are currently two types of structs: I'm calling them `const` and `constexpr`. Both are simple to use.
I'm moving towards using more and more constexpr structs because it allows for compile-time values to be used in the drivers (hence less RAM lookups and better performance). However many early drivers perform just fine and use the const style.

### const struct configuration:

For an example, see the I2C config struct in `drivers/i2c_config_struct.hh`:

```
struct I2CConfig {
	I2C_TypeDef *I2Cx;
	PinNoInit SCL;
	PinNoInit SDA;
	// etc..
};
```

To use the I2C peripheral driver, you can specify the struct values inline in the driver constructor:

```
mdrivlib::I2CPeriph MyI2C { 
	.I2Cx = I2C3, 
	.SCL = {GPIO::B, 8, LL_GPIO_AF4},
	.SDA = {GPIO::B, 9, LL_GPIO_AF4},
	// etc..
};

MyI2C.write(deviceAddr, data, dataSize);

```

Or you can define all your conf structs in one or more header files like this:

```
//.hh file:
const mdrivlib::I2CConfig MyI2Cconf {
	.I2Cx = I2C3,
	.SCL = {GPIO::B, 8, LL_GPIO_AF4},
	.SDA = {GPIO::B, 9, LL_GPIO_AF4},
	// etc..
};

//.cc file:
mdrivlib::I2CPeriph MyI2C {MyI2Cconf};
MyI2C.write(deviceAddr, data, dataSize);
```

### constexpr struct configuration: ###

For an example, see the SPI config struct in `drivers/spi_config_struct.hh`

```
struct DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 1; // 1 ==> SPI1, 2 ==> SPI2, ...
	static constexpr uint16_t NumChips = 1;
	static constexpr bool is_controller = true; // aka "master"
	static constexpr IRQType IRQn = SPI1_IRQn;
	static constexpr uint16_t priority1 = 3;
	static constexpr uint16_t priority2 = 3;
	static constexpr PinNoInit SCLK = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit COPI = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CIPO = {GPIO::A, 0, LL_GPIO_AF_0};
	static constexpr PinNoInit CS0 = {GPIO::A, 0, LL_GPIO_AF_0};
	//...
};
```

To use these types of structs, create your own struct type that derives from the DefaultSpiConf type, and override whatever values you don't want to be default:

```
using namespace mdrivlib;

struct MyDACConf : DefaultSpiConf {
	static constexpr uint16_t PeriphNum = 2; // SPI2
	static constexpr IRQType IRQn = SPI2_IRQn;
	static constexpr PinNoInit SCLK = {GPIO::A, 9, LL_GPIO_AF_5};
	static constexpr PinNoInit COPI = {GPIO::B, 15, LL_GPIO_AF_5};
	static constexpr PinNoInit CIPO = {GPIO::A, 0};
	static constexpr PinNoInit CS0 = {GPIO::B, 9, 0};
	static constexpr PinNoInit CS1 = {GPIO::B, 8, 0};
	static constexpr uint16_t clock_division = 1;
	//...
};


DacSpi_MCP48FVBxx<MM_DACConf> MyDAC;
MyDAC.init();
```

These `constexpr` structs are used when configuration values are required during the peripheral's normal operation (as opposed to just during the peripheral's setup), and using values stored in RAM effects performance. For instance, a high-speed SPI DAC driver with multiple chips on a single SPI bus requires manually toggling the Chip Select pins in the SPI interrupt, because the STM32 SPI peripheral does not support multiple CS lines. Using a constexpr struct allows for the CS pins' GPIO register values to be hard-coded into the interrupt's assembly, saving valuable cycles.



