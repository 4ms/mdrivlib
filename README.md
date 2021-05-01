# mdrivlib

## C++17/20 Embedded driver library

This is a work in progress, the API is in flux. Contributions, feedback, and issues are welcome.

### Devices supported:

*Cortex M7*

    * STM32H745/55/47/57 (dual-core M7/M4): full support, except built-in ADC
    * STM32F7xx (especially STM32F746, F76x): partial support

*Cortex A7*

  * STM32MP157 (dual-core A7 + M4): partial support, but adding features daily.

## Goals:

  * Easy to use, difficult to misuse.
  * Provide modern C++20 classes for setting up and accessing internal peripherals and common external chips.
  * Provide high-level functionality for common use cases in embedded projects (debouncing switches, wear-leveling, RGB color blending, filtering ADC readings).
  * Allow use of lambdas as callbacks (e.g. interrupt request handlers) with minimal overhead
  * Does not depend on dynamic memory (but provides some structures if you use it)
  * Bare-metal: does not depend on a RTOS
  * Priortize peripheral access efficiency over one-time cost of peripheral setup.
  * Unit tests (using doctest)
  * Example code (TODO: needs work!)
  * Emphasis on higher-end microcontrollers and microprocessors

## Status of Drivers:

See next section for description of Config types and HAL types.

#### STM32 Internal Peripheral Drivers:

| Peripheral | HAL      | Config          | Notes |
|:-----------|:---------|:----------------|:------|
| ADC        | STM32-LL | template params | Circular DMA mode. F7/H7 only. Todo: split off target-specific code. Needs modernizing |
| BDMA       | CMSIS    | template struct | Most configurations supported (H7 only, not present on F7 or MP1) |
| DMA2D      | CMSIS    |  -              | Supports FillRect with Rgb565 on H7, no other modes yet. (Not present on F7 or MP1) |
| I2C        | STM32-HAL| config struct   | Blocking, IT, DMA modes |
| Interrupt  | Cortex   | -               | Assign a lambda or any callable as any IRQ Handler at run-time. Supports NVIC (Cortex-M) and GIC (Cortex-A) |
| MDMA       | CMSIS    | template struct | Supports mem-to-mem only. |
| MPU        | CMSIS    |  -           	  | For disabling D-Cache on a memory region, Cortex-M only|
| GPIO (Pin) | STM32-LL | init/ctor params| Supports all GPIO functions (except Locking?). Run-time values. |
| GPIO (FPin)| CMSIS    | template params | "Fast Pin". Constexpr-configured, maximum efficiency. Supports high(), low(), read().
| GPIO (pin change ISR) | CMSIS | template params | Assign ISR callback for rising and/or falling edge for any pin. Current implementation uses a config struct |
| QSPI       | STM32-HAL| config struct   | Supports blocking and IT modes. Some chip-specific setup may be required in qspi_flash_registers.h file. Does not yet use mdrivlib::Interrupt class |
| RCC        | CMSIS    |  -    	 	  | Safer wrapper over raw register access |
| SAI        | STM32-HAL | config struct  | For streaming audio. DMA mode only. Supports RX- or TX-master. Provide it with tx/rx buffers and callbacks |


#### External Chip Drivers:

| Peripheral          | Config          | Notes |
|:--------------------|:----------------|:------|
| ADC, I2C (MAX11645) | config struct   | Blocking and IT mode |
| ADC, SPI (MAX11666) | template struct | Heavily optimized. Takes FilterType template param |
| Codec, I2C/I2S (WM8731) | config struct | Virtual class, ties I2C init with SAI+DMA init. Todo: make template class|
| DAC, SPI (MCP48FVBxx)| template struct| Supports streaming and blocking modes |
| GPIO Expander, I2C (TCA9535) | config struct | Supports inputs only (so far), I2C in IT mode |
| LED Driver, I2C (PCA9685) | | Uses STM32-HAL, heavily (HAL callbacks). Supports DMA, IT, and blocking modes. Supports mono and RGB LEDs. Works well with Frame Buffer LED class. Needs modernizing |


#### High-level helpers (target-agnostic):

| Peripheral   | Config          | Notes |
|:-------------|:----------------|:------|
| Clocks       |     -           | enable/disable/reset peripheral clocks |
| AnalogIn     | template params | Generic class to combine multi-channel ADC sources with post-filtering. WIP |
| CycleCounter | -               | Measures cycles spent between a start and stop point |
| DAC Stream   | template struct | Allows circular buffer to be filled in bursts, and output to DAC a steady rate |
| GPIO Stream  | template struct | Same as DAC Stream, but for a GPIO Pin output |
| Debounced Pin | template params |  Basic debouncing of physical pins (for buttons and other inputs). Requires `util` lib
| Frame Buffer LED| -             | Allows random access to a contigious frame buffer, used with LED DMA or IT drivers |
| QSPI Flash Cell Block | template params | Store and recall any data struct in a block or sector or FLASH memory, 
| RGB LED      | -              | Supports color blending 24-bit color, floats, RGB565 mode, flashing and fading animations. Supports different LED element types (e.g. Red and Blue are PWM-controlled, but Green is GPIO-controlled)
| Rotary Encoder | ctor params  | Supports half-step and full-step rotaries. Accumalates net position until read. |





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
#include "timekeeper.hh"

Pin myRedLED {GPIO::A, 2, PinMode::Output};

myRedLED.high();

Timekeeper lightFlasher({.TIMx=TIM2, .period=100000000/*ns*/}, [](){
	myRedLED.toggle(); 
});
lightFlasher.start();

```



## Common themes throughout drivers:


### STM32-HAL vs STM32-LL vs CMSIS

mdrivlib contains a mix of these three methods of hardware access:

   - STM32-HAL (`stm32xxxxx_XXX_hal.c/.h` files): The STM32 HAL library is used only when the peripheral is complex enough that re-writing it would be non-trivial **and** would offer very little benefit in terms of efficiency, code space, or additional features.
 
   - STM32-LL (`stm32xxxxx_XXX_ll.c/.h` files): In some drivers, ST's LL drivers are used, which are thin wrappers over direct register control via CMSIS headers. I am mostly migrating the drivers that use LL to use CMSIS since the LL drivers aren't as portable as I originally believed.
 
   - CMSIS (`stm32xxxxx.h` files): These are huge (2MB+) header files provided by the vendor (ST) that map all the registers to C-style structs and macros. The benefit of these is that the datasheet (Reference Manual) can be used along-side to setup or debug peripherals. For type safety I often use register classes called `RegisterBits` and `RegisterSection`. These can be specified as ReadWrite, ReadOnly, or WriteOnly, making this a bit safer to use than raw CMSIS register macros, while still compiling to at least as efficient assembly.



### Configuring a driver

The tables above mentions three types of configuration:

  * Template params: e.g. 
  
  ```
  AdcChan<AdcPeriphNum::_1, AdcChanNum::_13, uint16_t> myADC;
  ```
  
  * Template config struct: wraps multiple params into a struct passed as a template argument:
  
  ```
  struct MySpiAdcConfig : DefaultSpiConfig { 
      static constexpr uint16_t NumChips = 2;
      //...
  };
  
  AdcSpi_MAX11666<MySpiAdcConfig> myADC;
  ```
  
  * Config struct: wraps multiple params into a struct passed as a constructor argument:

  ```
  I2CPeriph MyI2C { 
      .I2Cx = I2C3, 
	   .SCL = {GPIO::B, 8, LL_GPIO_AF4},
	   .SDA = {GPIO::B, 9, LL_GPIO_AF4},
	   // ...
  };

  MyI2C.write(deviceAddr, data, dataSize);
  ```
 
