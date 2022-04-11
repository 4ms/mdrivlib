
These are structs found in files named `*_config_struct.hh`. They let you define board-specific things like the pins to use, peripheral number, DMA settings, etc. Your project should pass them to the constructor of the driver (either as a template parameter or constructor parameter).
  
There are currently three types of structs: I'm calling them `const`, `static constexpr` and `constexpr`. 
I'm moving towards using more and more constexpr structs because it allows for compile-time values to be used in the drivers (hence better performance). However many early drivers perform just fine and use the const style.

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

Note that you cannot make `MyI2Cconf` constexpr because it contains a reinterpret_cast (I2C3).

### "static constexpr" struct configuration: ###

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

These are just static constants, similar to the STM32-HAL method of using #defines for all configuration, except with the added benefit of type-safety and inheritance of default values. This means typos will typically be compile-time errors, not runtime errors, and you don't need to copy/paste as much when starting a new project.

To use these types of structs, create your own struct type that derives from the DefaultSpiConf type, and override whatever values you don't want to be default:

```
using namespace mdrivlib;

struct MyDACConf : DefaultSpiConf {
	static constexpr auto PeriphNum = 2; // SPI2
	static constexpr auto IRQn = SPI2_IRQn;
	static constexpr auto SCLK = {GPIO::A, 9, LL_GPIO_AF_5};
	static constexpr auto COPI = {GPIO::B, 15, LL_GPIO_AF_5};
	static constexpr auto CIPO = {GPIO::A, 0};
	static constexpr auto CS0 = {GPIO::B, 9, 0};
	static constexpr auto CS1 = {GPIO::B, 8, 0};
	static constexpr auto clock_division = 1;
	//...
};


DacSpi_MCP48FVBxx<MM_DACConf> MyDAC;
MyDAC.init();
```

These `constexpr` structs are used when configuration values are required during the peripheral's normal operation (as opposed to just during the peripheral's setup), and using values stored in RAM effects performance. For instance, a high-speed SPI DAC driver with multiple chips on a single SPI bus requires manually toggling the Chip Select pins in the SPI interrupt, because the STM32 SPI peripheral does not support multiple CS lines. Using a constexpr struct allows for the CS pins' GPIO register values to be hard-coded into the interrupt's assembly, saving valuable cycles and cache lines.

### "constexpr" NTTP configuration: ###

This method has the best of both of the previous methods. I'm moving everything to this. It relies on a C++20 feature of allowing structural types as non-type template parameters. 

```
  // mdrivlib configuration struct:
  struct AdcConfig {
  	uint32_t ADCx;
	uint32_t BitDepth = 12;
	bool oversample = false;
  	...
  };

  // mdrivlib peripheral:
  template<AdcConfig Conf> AdcPeriph {...};

  // your application:
  constexpr AdcConfig MyConfig {
  	.ADCx = 1,
	// BitDepth is not initialized here so it will use default value of 12
	.oversample = true;
  }

  AdcPeriph<MyConfig> adc(...);
  ```

  You create a constexpr object with static storage (that is, global or a static member of a class or struct), e.g. `MyConfig`, filling in the configuration values into the fields. Direct initialization can be used since the members are not static in the struct definition, which I find to be very readable. Then you pass the object as a template parameter to the peripheral driver. The driver is able to access all the fields as constexpr values, so it can do optimizations like:
  ```
  if constexpr(Conf.oversample)
  	val += reading; //...
  ```
  and do things like:

  ```
  template<SomeConfig Conf> class SomePeripheral {
  	uint32_t the_data[Conf.num_channels];
  }
  ```
	
  It's easy to inherit the default configuration values: just don't specify them in your configuration. 

  Some peripherals will have nested structs (e.g. AdcConfig has a DmaConfig inside it). There will be a static constexpr member with the name `default_XXX_config` containing the default values, and the actual member will be initialized to this. If you specify some of the values of the inner struct, the values that you didn't specify will be set to the default values of the inner stuct type -- not to the default values from the `default_XXX_config` type. I'm working on a solution to this (unique types for nested config classes).



  - Todo: move all `config_struct.hh` files to a `conf_struct/` dir
  - Todo: create conf structs for adc_builtin, ... (others)

