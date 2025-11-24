#pragma once
#include "drivers/ltdc_screen_config_struct.hh"
#include "drivers/pin.hh"
#include "stm32xx.h"
#include "util/base_concepts.hh"

// General LTDC driver, setup for double-buffering.
// Uses STM32-HAL.
//
// Does not handle configuring the display driver chip, that typically needs to be done before calling init()
// To use: call init() to begin. Call set_buffer() to change the framebuffer on the next Vsync
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
	ScreenParallelWriter() = default;

	void set_buffer(auto *buffer) {
		while (LTDC->SRCR != 0) {
		}

		_buffer_addr = reinterpret_cast<uint32_t>(buffer);

		HAL_LTDC_SetAddress_NoReload(&hltdc_F, _buffer_addr, 1);
		HAL_LTDC_Reload(&hltdc_F, LTDC_RELOAD_VERTICAL_BLANKING);
	}

	void init(auto *buffer) {
		_buffer_addr = reinterpret_cast<uint32_t>(buffer);
		_init_pins_as_ltdc();
		_init_ltdc();
	}

private:
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

	void _init_ltdc() {
		HAL_LTDC_DeInit(&hltdc_F);

		LTDC_LayerCfgTypeDef pLayerCfg;
		hltdc_F.Init.HSPolarity = LTDC_HSPOLARITY_AL;
		hltdc_F.Init.VSPolarity = LTDC_VSPOLARITY_AL;
		hltdc_F.Init.DEPolarity = LTDC_DEPOLARITY_AL;
		hltdc_F.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
		hltdc_F.Init.HorizontalSync = (ConfT::HSyncWidth - 1);
		hltdc_F.Init.VerticalSync = (ConfT::VSyncWidth - 1);
		hltdc_F.Init.AccumulatedHBP = (ConfT::HSyncWidth + ConfT::HBackPorch - 1);
		hltdc_F.Init.AccumulatedVBP = (ConfT::VSyncWidth + ConfT::VBackPorch - 1);
		hltdc_F.Init.AccumulatedActiveH = (_height + ConfT::VSyncWidth + ConfT::VBackPorch - 1);
		hltdc_F.Init.AccumulatedActiveW = (_width + ConfT::HSyncWidth + ConfT::HBackPorch - 1);
		hltdc_F.Init.TotalHeigh = (_height + ConfT::VSyncWidth + ConfT::VBackPorch + ConfT::VFrontPorch - 1);
		hltdc_F.Init.TotalWidth = (_width + ConfT::HSyncWidth + ConfT::HBackPorch + ConfT::HFrontPorch - 1);

		hltdc_F.Init.Backcolor.Blue = 0x00;
		hltdc_F.Init.Backcolor.Green = 0x3F;
		hltdc_F.Init.Backcolor.Red = 0;

		hltdc_F.Instance = LTDC;

		pLayerCfg.WindowX0 = 0;
		pLayerCfg.WindowX1 = _width;
		pLayerCfg.WindowY0 = 0;
		pLayerCfg.WindowY1 = _height;
		pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
		pLayerCfg.FBStartAdress = _buffer_addr;
		pLayerCfg.Alpha = 255;

		pLayerCfg.Alpha0 = 255;
		pLayerCfg.Backcolor.Blue = 0;
		pLayerCfg.Backcolor.Green = 0;
		pLayerCfg.Backcolor.Red = 0;

		pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
		pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

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
