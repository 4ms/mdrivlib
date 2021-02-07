### mdrivlib ###
## Embedded driver library, providing high-level functionality using modern C++ ##

This is highly a work in progress, the API is in flux. Contributions, feedback, and issues are welcome.

Currently only some STM32 devices are supported. As much as possible, the vendor-provided LL (low-level) libraries are used.

Goals:

  * Easy to use, difficult to misuse.
  * Provide modern C++20 classes for setting up and accessing internal peripherals and common external chips.
  * Provide high-level functionality for common use cases (debouncing switches, wear-leveling, RGB color blending).
  * Allow use of C++ callable objects as interrupt request handlers and callbacks (not limited to static or C-style functions).
  * Priortize peripheral access efficiency over one-time cost of peripheral setup.
  * Unit tests
  * Example code (ideally on a low-cost vendor-provided development board, such as Discovery Boards)

Common themes throughout drivers:

  * Configuration structs: These are named xyz_config_struct.hh and let you define things like the pins to use, peripheral number, DMA settings, etc. Your project should define a const or constexpr instance of this struct and pass it to the constructor of the driver.
    - Todo: move all config_struct.hh files to a conf_struct dir.
	- Todo: create conf structs for adc_builtin, ... (others)

  * HAL vs LL vs CMSIS:
	  - I'm playing with several methods of hardware access and seeing which one(s) are best as I continue to develope and use this library in real-world situations. 
	  - HAL: The STM32 HAL library is used only when the peripheral is complex enough that re-writing it would be non-trivial (e.g. the I2C event state machine). STM32's HAL is inefficient, uses C APIs, and contains bugs and short-comings, so my goal is to get rid of it eventually.
	  - LL: In some drivers, ST's LL drivers are used, which are thin wrappers over direct register control. The benefit of this approach is transparency, but the limitation is limited portability and readability. From device to device, the LL drivers have variations in naming conventions which counter-act the benefit of using a HAL. I've started moving away from this and towards CMSIS or my own register access class.
      - CMSIS: These are huge header files provided by the vendor (ST) that map all the registers to C-style structs. (e.g. "stm32h745xx.h"). The benefit of these is that the datasheet (Reference Manual) can be used along-side to setup peripherals. CMSIS is specified by ARM, so it's likely to be easier to port between vendors. The drawback is that a complete HAL must be written, so it means re-inventing some wheels. For simple peripherals, I've been using a custom register class wrapper over the CMSIS files.
	   
Driver:

Peripheral drivers:
  * adc_builtin_driver: AdcPeriph and AdcChan.
      AdcPeriph is has a template param representing ADC1, ADC2, ADC3, etc. AdcChan has a template parameter for the AdcPeriph, and the channel number.
	  This is meant to be used with the DMA continuously (circular mode).
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
