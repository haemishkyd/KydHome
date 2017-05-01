#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "uart.h"
#include "osapi.h"
#include "espconn.h"
#include "gpio.h"
#include "ds18b20.h"

static uint8_t run_reset(void);
static void write_zero(void);
static void write_one(void);
static uint8_t read_slot(void);
static void write_byte(uint8_t val_to_write);
static uint8_t read_byte(void);
static inline unsigned get_ccount(void);
static void my_delay_us(uint16_t delay_us);

uint8_t scratch_data[9];
static int16_t Temperature;

void ICACHE_FLASH_ATTR DS18B20_Test_Run_Low(void)
{
	write_zero();
}

void ICACHE_FLASH_ATTR DS18B20_Test_Run_High(void)
{
	write_one();
}

int16_t ICACHE_FLASH_ATTR DS18B20_Get_Temp(void)
{
	return Temperature;
}

uint8_t ICACHE_FLASH_ATTR DS18B20_Read_Temp(void)
{
	uint8_t reset_complete = 0;
	uint8_t read_scratch = 0;
	uint8_t return_value = 0;
	static uint8_t stateMachine=0;
	static uint8_t timeoutCounter=0;
	unsigned char temp_string[50];

	switch (stateMachine)
	{
	case 0:
		reset_complete = run_reset();
		if (reset_complete == 1)
		{
			// Master issues skip ROM command
			write_byte(0xCC);
			// Master issues scratch pad command
			write_byte(0x44);
			return_value = 1;
			timeoutCounter = 0;
			stateMachine = 1;
		}
		else
		{
			return_value = 2;
		}
		break;
	case 1:
		if ((read_byte() == 0xFF) || (timeoutCounter>10))
		{
			if (timeoutCounter > 10)
			{
				return_value = 0xFF;
			}
			else
			{
				return_value = 4;
			}
			stateMachine = 2;
		}
		else
		{
			timeoutCounter++;
			return_value = 3;
		}
		break;
	case 2:
		reset_complete = run_reset();
		if (reset_complete == 1)
		{
			// Master issues skip ROM command
			write_byte(0xCC);
			// Master issues scratch pad command
			write_byte(0xBE);
			// Master writes three values
			for (read_scratch = 0;read_scratch<9;read_scratch++)
			{
				scratch_data[read_scratch] = read_byte();
			}
			Temperature = scratch_data[0]>>4;
			Temperature |= scratch_data[1]<<4;
			return_value = 5;
		}
		else
		{
			return_value = 6;
		}
		stateMachine = 0;
		break;
	default:
		stateMachine = 0;
		break;
	}

	return return_value;
}

static uint8_t ICACHE_FLASH_ATTR read_byte(void)
{
	uint8_t bit_idx;
	uint8_t temp_bit;
	uint8_t return_value=0;

	for (bit_idx = 0;bit_idx<8; bit_idx++)
	{
		return_value = return_value >> 1;
		temp_bit = read_slot();
		if (temp_bit == 1)
			return_value |= 0x80;
	}
	return return_value;
}

static void ICACHE_FLASH_ATTR write_byte(uint8_t val_to_write)
{
	uint8_t bit_idx;
	uint8_t temp_bit;

	for (bit_idx = 0; bit_idx < 8; bit_idx++)
	{
		temp_bit = val_to_write & 0x01;
		val_to_write = val_to_write >> 1;

		if (temp_bit == 1) // Writing 1
		{
			write_one();
		}
		else // Writing 0
		{
			write_zero();
		}
	}
	my_delay_us(10);
}

static uint8_t ICACHE_FLASH_ATTR run_reset(void)
{
	uint8_t return_val = 0;
	unsigned timeout_val;

	gpio_output_set(0, 0, 0, MY_DS1820_PIN);
	my_delay_us(20);
	gpio_output_set(0, MY_DS1820_PIN, MY_DS1820_PIN, 0);
	my_delay_us(500);
	gpio_output_set(0, 0, 0, MY_DS1820_PIN);
	my_delay_us(40);
	if ((gpio_input_get()&MY_DS1820_PIN) == 0)
	{
		return_val = 1;
		timeout_val = get_ccount();
		while ((gpio_input_get()&MY_DS1820_PIN) == 0)
		{
			if ((get_ccount()-timeout_val) > (240*80))
			{
				break;
			}
		}
	}
	my_delay_us(220);

	return return_val;
}

static uint8_t ICACHE_FLASH_ATTR read_slot(void)
{
	uint8_t return_val = 0;

	//pull it low
	gpio_output_set(0, MY_DS1820_PIN, MY_DS1820_PIN, 0);
	my_delay_us(8);
	//let the line go
	gpio_output_set(0, 0, 0, MY_DS1820_PIN);
	my_delay_us(4);

	return_val = ((gpio_input_get()&MY_DS1820_PIN)==MY_DS1820_PIN)?1:0;
	my_delay_us(50);

	return return_val;
}

static void ICACHE_FLASH_ATTR write_zero(void)
{
	//Pull the line low and hold it
	gpio_output_set(0, MY_DS1820_PIN, MY_DS1820_PIN, 0);
	my_delay_us(57);
	//Then let the line go
	gpio_output_set(0, 0, 0, MY_DS1820_PIN);
	my_delay_us(8);
}

static void ICACHE_FLASH_ATTR write_one(void)
{
	//Pull the line low
	gpio_output_set(0, MY_DS1820_PIN, MY_DS1820_PIN, 0);
	my_delay_us(8);
	//Within 10 seconds we need to let the line go
	gpio_output_set(0, 0, 0, MY_DS1820_PIN);
	my_delay_us(57);
}

static void ICACHE_FLASH_ATTR my_delay_us(uint16_t delay_us)
{
	unsigned current_time;

	current_time = get_ccount();
	while((get_ccount() - current_time) < (delay_us*80))
	{}
}

static inline unsigned ICACHE_FLASH_ATTR get_ccount(void)
{
        unsigned r;
        asm volatile ("rsr %0, ccount" : "=r"(r));
        return r;
}
