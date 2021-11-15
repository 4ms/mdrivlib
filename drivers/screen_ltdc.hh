#pragma once
#include "RGB565_480x272.h"
#include "drivers/ltdc_screen_config_struct.hh"
#include "drivers/parallel_writer.hh"
#include "drivers/pin.hh"
#include "spi_screen_ST77XX.hh"
#include "stm32xx.h"
#include "util/base_concepts.hh"

//TODO: use a single base class and swap out ParallelWriter for Spi version
template<Derived<mdrivlib::ParallelWriterConf> ConfT>
class ST77XXParallelSetup {

	mdrivlib::ParallelWriter<ConfT> writer;

public:
	void setup_driver_chip() {
		using InitCommands = mdrivlib::ST7789Init<ConfT::width, ConfT::height, mdrivlib::ST77XX::NotInverted>;
		_init_display_driver<InitCommands>();
		set_rotation(ConfT::rotation);
	}

private:
	void _init_pins_as_gpios() {
	}

	template<typename InitCmds>
	void _init_display_driver() {
		for (auto c : InitCmds::cmds) {
			transmit_blocking<Cmd>(c.cmd);
			int numArgs = c.num_args;
			uint32_t args = c.args;
			while (numArgs--) {
				transmit_blocking<Data>(args & 0x000000FF);
				args >>= 8;
			}
			if (c.delay_ms)
				HAL_Delay(c.delay_ms);
		}
	}

	enum DataCmd { Data, Cmd };
	template<DataCmd Mode>
	void transmit_blocking(uint8_t d) {
		if constexpr (Mode == Data)
			writer.write_data(d);
		else
			writer.write_cmd(d);
	}
};

template<Derived<mdrivlib::LTDCScreenConf> ConfT>
class ScreenParallelWriter {
	using GPIO = mdrivlib::GPIO;
	using PinMode = mdrivlib::PinMode;
	using Pin = mdrivlib::Pin;

	LTDC_HandleTypeDef hltdc_F;
	uint32_t _buffer_addr;

	static constexpr size_t _width = ConfT::viewWidth;
	static constexpr size_t _height = ConfT::viewHeight;

public:
	ScreenParallelWriter(auto *buffer)
		: _buffer_addr{reinterpret_cast<uint32_t>(buffer)} {
	}

	void set_buffer(void *buffer) {
		_buffer_addr = reinterpret_cast<uint32_t>(buffer);
		//Do something with LTDC peripheral?
	}

	void init() {
		init_pins_as_gpios();
		init_driver_chip();

		init_pins_as_ltdc();
		init_ltdc();
	}

	void init_pins_as_ltdc() {
		__HAL_RCC_LTDC_CLK_ENABLE();

		Pin ltdc_pin_init[] = {
		Pin{GPIO::D, 10, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_B3
		Pin{GPIO::I, 4, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_B4
		Pin{GPIO::I, 5, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_B5
		Pin{GPIO::I, 6, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_B6
		Pin{GPIO::I, 7, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_B7
		Pin{GPIO::E, 14, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_CLK
		Pin{GPIO::E, 13, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_DE
		Pin{GPIO::H, 13, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_G2
		Pin{GPIO::H, 14, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_G3
		Pin{GPIO::H, 15, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_G4
		Pin{GPIO::H, 4, PinMode::Analog, LL_GPIO_AF_9},	  // LTDC_G5 Note: this one is AF9
		Pin{GPIO::C, 7, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_G6
		Pin{GPIO::D, 3, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_G7
		Pin{GPIO::C, 6, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_HSYNC
		Pin{GPIO::H, 9, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_R3
		Pin{GPIO::H, 10, PinMode::Analog, LL_GPIO_AF_14}, // LTDC_R4
		Pin{GPIO::A, 9, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_R5
		Pin{GPIO::A, 8, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_R6
		Pin{GPIO::G, 6, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_R7
		Pin{GPIO::I, 9, PinMode::Analog, LL_GPIO_AF_14},  // LTDC_VSYNC
		};
	}

	void init_ltdc() {

		RCC_PeriphCLKInitTypeDef periph_clk_init_struct;
		HAL_LTDC_DeInit(&hltdc_F);

		LTDC_LayerCfgTypeDef pLayerCfg;
		/* Polarity configuration */
		/* Initialize the horizontal synchronization polarity as active low */
		hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
		/* Initialize the vertical synchronization polarity as active low */
		hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL;
		/* Initialize the data enable polarity as active low */
		hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL;
		/* Initialize the pixel clock polarity as input pixel clock */
		hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
		/* Timing configuration */
		hltdc_F.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
		hltdc_F.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
		hltdc_F.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
		hltdc_F.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
		hltdc_F.Init.AccumulatedActiveH = (_height + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
		hltdc_F.Init.AccumulatedActiveW = (_width + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
		hltdc_F.Init.TotalHeigh = (_height + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
		hltdc_F.Init.TotalWidth = (_width + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

		/* Configure R,G,B component values for LCD background color */
		hltdc_F.Init.Backcolor.Blue = 0;
		hltdc_F.Init.Backcolor.Green = 0;
		hltdc_F.Init.Backcolor.Red = 0;

		hltdc_F.Instance = LTDC;

		/* Layer1 Configuration ------------------------------------------------------*/

		/* Windowing configuration */
		/* In this case all the active display area is used to display a picture then 
		 Horizontal start = horizontal synchronization + Horizontal back porch = 43 
		 Vertical start   = vertical synchronization + vertical back porch     = 12
		 Horizontal stop = Horizontal start + window width -1 = 43 + 480 -1 
		 Vertical stop   = Vertical start + window height -1  = 12 + 272 -1      */
		pLayerCfg.WindowX0 = 0;
		pLayerCfg.WindowX1 = _width;
		pLayerCfg.WindowY0 = 0;
		pLayerCfg.WindowY1 = _height;

		/* Pixel Format configuration*/
		pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

		/* Start Address configuration : frame buffer */
		// pLayerCfg.FBStartAdress = _buffer_addr;
		pLayerCfg.FBStartAdress = (uint32_t)&RGB565_480x272;

		/* Alpha constant (255 == totally opaque) */
		pLayerCfg.Alpha = 255;

		/* Default Color configuration (configure A,R,G,B component values) : no background color */
		pLayerCfg.Alpha0 = 0; /* fully transparent */
		pLayerCfg.Backcolor.Blue = 0;
		pLayerCfg.Backcolor.Green = 0;
		pLayerCfg.Backcolor.Red = 0;

		/* Configure blending factors */
		pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
		pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

		/* Configure the number of lines and number of pixels per line */
		pLayerCfg.ImageWidth = _width;
		pLayerCfg.ImageHeight = _height;

		if (HAL_LTDC_Init(&hltdc_F) != HAL_OK) {
			__BKPT(122);
		}

		if (HAL_LTDC_ConfigLayer(&hltdc_F, &pLayerCfg, 1) != HAL_OK) {
			__BKPT(123);
		}
	}
};
