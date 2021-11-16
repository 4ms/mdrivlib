#pragma once
#include "RGB565_480x272.h"
#include "debug.hh"
#include "drivers/ltdc_screen_config_struct.hh"
#include "drivers/pin.hh"
#include "stm32xx.h"
#include "util/base_concepts.hh"

// General LTDC driver, setup for double-buffering.
// Uses STM32-HAL.
//
// Provide a configuration struct that derives from LTDCScreenConf.
// Does not handle configuring the display driver chip, that typically needs to be done before calling init()
// Call set_buffer
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
		HAL_LTDC_SetAddress_NoReload(&hltdc_F, _buffer_addr, 0);
		HAL_LTDC_Reload(&hltdc_F, LTDC_RELOAD_VERTICAL_BLANKING);
	}

	void init() {
		_init_pins_as_ltdc();
		_init_ltdc();
	}

private:
	GCC_OPTIMIZE_OFF
	void _init_pins_as_ltdc() {
		__HAL_RCC_LTDC_CLK_ENABLE();
		for (auto &p : ConfT::r) {
			if (p.gpio != GPIO::Unused)
				Pin _init{p, PinMode::Alt};
		}
		for (auto &p : ConfT::g) {
			if (p.gpio != GPIO::Unused)
				Pin _init{p, PinMode::Alt};
		}
		for (auto &p : ConfT::b) {
			if (p.gpio != GPIO::Unused)
				Pin _init{p, PinMode::Alt};
		}
		Pin{ConfT::de.gpio, ConfT::de.pin, PinMode::Alt, ConfT::de.af};
		Pin{ConfT::clk.gpio, ConfT::clk.pin, PinMode::Alt, ConfT::clk.af};
		Pin{ConfT::vsync.gpio, ConfT::vsync.pin, PinMode::Alt, ConfT::vsync.af};
		Pin{ConfT::hsync.gpio, ConfT::hsync.pin, PinMode::Alt, ConfT::hsync.af};
	}

	GCC_OPTIMIZE_OFF
	void _init_ltdc() {
		//RCCEx_PLL3_Config(&(PeriphClkInit->PLL3),DIVIDER_R_UPDATE);
		//RCCEx_PLL4_Config()

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
		hltdc_F.Init.HorizontalSync = (ConfT::HSyncWidth - 1);
		hltdc_F.Init.VerticalSync = (ConfT::VSyncWidth - 1);
		hltdc_F.Init.AccumulatedHBP = (ConfT::HSyncWidth + ConfT::HBackPorch - 1);
		hltdc_F.Init.AccumulatedVBP = (ConfT::VSyncWidth + ConfT::VBackPorch - 1);
		hltdc_F.Init.AccumulatedActiveH = (_height + ConfT::VSyncWidth + ConfT::VBackPorch - 1);
		hltdc_F.Init.AccumulatedActiveW = (_width + ConfT::HSyncWidth + ConfT::HBackPorch - 1);
		hltdc_F.Init.TotalHeigh = (_height + ConfT::VSyncWidth + ConfT::VBackPorch + ConfT::VFrontPorch - 1);
		hltdc_F.Init.TotalWidth = (_width + ConfT::HSyncWidth + ConfT::HBackPorch + ConfT::HFrontPorch - 1);

		/* Configure R,G,B component values for LCD background color */
		hltdc_F.Init.Backcolor.Blue = 0x00;
		hltdc_F.Init.Backcolor.Green = 0x3F;
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
