#include <stdio.h>
#include "soc/reg_base.h" // DR_REG_GPIO_BASE, DR_REG_IO_MUX_BASE
#include "driver/rtc_io.h" // rtc_gpio_*
#include "pin.h"

// NOTE: Remember to enclose the macro values in parenthesis, as below
#define GPIO_OUT_REG            (DR_REG_GPIO_BASE+0x04)
#define GPIO_OUT_W1TS_REG		(DR_REG_GPIO_BASE+0X08)
#define GPIO_OUT_W1TC_REG		(DR_REG_GPIO_BASE+0X0c)
#define GPIO_OUT1_REG			(DR_REG_GPIO_BASE+0x10)
#define	GPIO_OUT1_W1TS_REG		(DR_REG_GPIO_BASE+0x14)
#define GPIO_OUT1_W1TC_REG		(DR_REG_GPIO_BASE+0x18)
#define GPIO_ENABLE_REG			(DR_REG_GPIO_BASE+0X20)
#define GPIO_ENABLE_W1TS_REG	(DR_REG_GPIO_BASE+0x24)
#define GPIO_ENABLE_W1TC_REG	(DR_REG_GPIO_BASE+0x28)
#define GPIO_ENABLE1_REG		(DR_REG_GPIO_BASE+0x2c)
#define GPIO_ENABLE1_W1TS_REG	(DR_REG_GPIO_BASE+0x30)
#define GPIO_ENABLE1_W1TC_REG   (DR_REG_GPIO_BASE+0x34)
#define GPIO_STRAP_REG			(DR_REG_GPIO_BASE+0x38)
#define GPIO_IN_REG				(DR_REG_GPIO_BASE+0x3c)
#define GPIO_IN1_REG			(DR_REG_GPIO_BASE+0x40)
#define GPIO_STATUS_REG			(DR_REG_GPIO_BASE+0x44)
#define GPIO_STATUS_W1TS_REG	(DR_REG_GPIO_BASE+0x48)
#define GPIO_STATUS_W1TC_REG	(DR_REG_GPIO_BASE+0x4c)
#define GPIO_STATUS1_REG		(DR_REG_GPIO_BASE+0x50)
#define GPIO_STATUS1_W1TS_REG	(DR_REG_GPIO_BASE+0x54)
#define GPIO_STATUS1_W1TC_REG	(DR_REG_GPIO_BASE+0x58)
#define GPIO_ACPU_INT_REG		(DR_REG_GPIO_BASE+0x60)
#define GPIO_ACPU_NMI_INT_REG	(DR_REG_GPIO_BASE+0x64)
#define GPIO_PCPU_INT_REG		(DR_REG_GPIO_BASE+0x68)
#define GPIO_PCPU_NMI_INT_REG	(DR_REG_GPIO_BASE+0x6c)
#define GPIO_ACPU_INT1_REG		(DR_REG_GPIO_BASE+0x74)
#define GPIO_ACPU_NMI_INT1_REG	(DR_REG_GPIO_BASE+0x78)
#define	GPIO_PCPU_INT1_REG		(DR_REG_GPIO_BASE+0x7c)
#define	GPIO_PCPU_NMI_INT1_REG	(DR_REG_GPIO_BASE+0x80)
#define GPIO_PIN_REG(n)			(DR_REG_GPIO_BASE+(0x88 + 0x4*(n)))
#define GPIO_FUNC_IN_SEL_CFG_REG(n)	(DR_REG_GPIO_BASE+(0x130 + 0x4*(n)))
#define GPIO_FUNC_OUT_SEL_CFG_REG(n) (DR_REG_GPIO_BASE+(0x530 + 0x4*(n)))


#define IO_MUX_REG(n) (DR_REG_IO_MUX_BASE + PIN_MUX_REG_OFFSET[n])


#define MCU_OE	 0
#define SLP_SEL	 1
#define MCU_WPD  2
#define MCU_WPU	 3
#define MCU_IE   4
#define MCU_DRV  5
#define FUN_WPD  7
#define FUN_WPU  8
#define FUN_IE   9
#define FUN_DRV  10
#define MCU_SEL  12


#define REG(r) (*(volatile uint32_t *)(r))
#define REG_BITS 32
#define REG_SET_BIT(r,b) (REG(r) |= (1U << b))
#define REG_CLR_BIT(r,b) (REG(r) &= ~(1U << b))
#define REG_GET_BIT(r,b) ((REG(r) >> b) & 1U) 

#define IO_MUX_RESET 0x2900
#define GPIO_FUNC_OUT_SEL_CFG_REG_RESET 0x100
#define PAD_DRIVER_BIT 2



// Gives byte offset of IO_MUX Configuration Register
// from base address DR_REG_IO_MUX_BASE
static const uint8_t PIN_MUX_REG_OFFSET[] = 
{
    0x44, 0x88, 0x40, 0x84, 0x48, 0x6c, 0x60, 0x64, // pin  0- 7
    0x68, 0x54, 0x58, 0x5c, 0x34, 0x38, 0x30, 0x3c, // pin  8-15
    0x4c, 0x50, 0x70, 0x74, 0x78, 0x7c, 0x80, 0x8c, // pin 16-23
    0x90, 0x24, 0x28, 0x2c, 0xFF, 0xFF, 0xFF, 0xFF, // pin 24-31
    0x1c, 0x20, 0x14, 0x18, 0x04, 0x08, 0x0c, 0x10, // pin 32-39
};


// Reset the configuration of a pin to not be an input or an output.
// Pull-up is enabled so the pin does not float.
// Return zero if successful, or non-zero otherwise.
int32_t pin_reset(pin_num_t pin)
{
	if (rtc_gpio_is_valid_gpio(pin)) 
	{ // hand-off work to RTC subsystem
		rtc_gpio_deinit(pin);
		rtc_gpio_pullup_en(pin);
		rtc_gpio_pulldown_dis(pin);
	}
	REG(GPIO_PIN_REG(pin)) = 0;
	REG(GPIO_FUNC_OUT_SEL_CFG_REG(pin)) = GPIO_FUNC_OUT_SEL_CFG_REG_RESET;
	REG(IO_MUX_REG(pin)) = IO_MUX_RESET;
	// NOTE: By default, pin should not float, save power with FUN_WPU=1
	// Now that the pin is reset, set the output level to zero
	return pin_set_level(pin, 0);
}

// Enable or disable a pull-up on the pin.
// Return zero if successful, or non-zero otherwise.
int32_t pin_pullup(pin_num_t pin, bool enable)
{
	if (rtc_gpio_is_valid_gpio(pin)) 
	{ // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pullup_en(pin);
		else return rtc_gpio_pullup_dis(pin);
	}
	if(enable)
	{
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPU);
	} 
	else
	{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPU);
	}
	return 0;
}

// Enable or disable a pull-down on the pin.
// Return zero if successful, or non-zero otherwise.
int32_t pin_pulldown(pin_num_t pin, bool enable)
{
	if (rtc_gpio_is_valid_gpio(pin)) 
	{ // hand-off work to RTC subsystem
		if (enable) return rtc_gpio_pulldown_en(pin);
		else return rtc_gpio_pulldown_dis(pin);
	}
	if(enable)
	{
		REG_SET_BIT(IO_MUX_REG(pin), FUN_WPD);
		
	} 
	else if(!enable)
	{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_WPD);
	
	} 
	return 0;
}

// Enable or disable the pin as an input signal.
// Return zero if successful, or non-zero otherwise.
int32_t pin_input(pin_num_t pin, bool enable)
{
	if(enable)
	{
		REG_SET_BIT(IO_MUX_REG(pin), FUN_IE);
	} 
	else if(!enable)
	{
		REG_CLR_BIT(IO_MUX_REG(pin), FUN_IE);
	} 
	else
	{
		return 1;
	}
	return 0;
}

// Enable or disable the pin as an output signal.
// Return zero if successful, or non-zero otherwise.
int32_t pin_output(pin_num_t pin, bool enable)
{
	if(pin < REG_BITS)
	{
		if(enable)
		{
			REG_SET_BIT(GPIO_ENABLE_REG, pin);
		} else if(!enable)
		{
			REG_CLR_BIT(GPIO_ENABLE_REG, pin);
		} else
		{
			return 1;
		}
	} 
	else if (pin >= REG_BITS)
	{
		if(enable)
		{
			REG_SET_BIT(GPIO_ENABLE1_REG, (pin-REG_BITS));
		} else if(!enable)
		{
			REG_CLR_BIT(GPIO_ENABLE1_REG, (pin-REG_BITS));
		} else
		{
			return 1;
		}
	}
	else{
		return 1;
	}
	
	return 0;
}

// Enable or disable the pin as an open-drain signal.
// Return zero if successful, or non-zero otherwise.
int32_t pin_odrain(pin_num_t pin, bool enable)
{
	if(enable)
	{
		REG_SET_BIT(GPIO_PIN_REG(pin), PAD_DRIVER_BIT);
	} 
	else if(!enable)
	{
		REG_CLR_BIT(GPIO_PIN_REG(pin), PAD_DRIVER_BIT);
	} 
	else
	{
		return 1;
	}
	return 0;
}

// Sets the output signal level if the pin is configured as an output.
// Return zero if successful, or non-zero otherwise.
int32_t pin_set_level(pin_num_t pin, int32_t level)
{
	if(pin < REG_BITS){
			if(level){
				REG_SET_BIT(GPIO_OUT_REG, pin);
			} 
			else if(!level)
			{
				REG_CLR_BIT(GPIO_OUT_REG, pin);
			} 

	} else if (pin >= REG_BITS){
			if(level)
			{
				REG_SET_BIT(GPIO_OUT1_REG, (pin-REG_BITS));
			} 
			else if(!level)
			{
				REG_CLR_BIT(GPIO_OUT1_REG, (pin-REG_BITS));
			}
		} 
	return 0;
}

// Gets the input signal level if the pin is configured as an input.
// Return zero or one if successful, or negative otherwise.
int32_t pin_get_level(pin_num_t pin)
{
	if(pin < REG_BITS)
	{
		return REG_GET_BIT(GPIO_IN_REG, pin);
	} 
	else if (pin >= REG_BITS)
	{
		return REG_GET_BIT(GPIO_IN1_REG, (pin-REG_BITS));
	}
	return -1;
}

// Get the value of the input registers, one pin per bit.
// The two 32-bit input registers are concatenated into a uint64_t.
uint64_t pin_get_in_reg(void)
{
	return (uint64_t)REG(GPIO_IN1_REG) << REG_BITS | REG(GPIO_IN_REG);
}

// Get the value of the output registers, one pin per bit.
// The two 32-bit output registers are concatenated into a uint64_t.
uint64_t pin_get_out_reg(void)
{
	return (uint64_t)REG(GPIO_OUT1_REG) << REG_BITS | REG(GPIO_OUT_REG);
}
