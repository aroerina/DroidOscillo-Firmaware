#ifndef M0APP_INIT_
#define M0APP_INIT_

#include "chip.h"
#include "app_usbd_cfg.h"
#include "usbd_rom_api.h"
#include "mc_shared_mem.h"

#define I2C_DAC_ADDR_7BIT       (0x60)	// 7bit 右づめ
#define I2C_COMMAND_WRITE_VOLATILE_MEMORY 0x2
#define SPEED_100KHZ            (100000)

#define I2C_EEPROM_ADDR				0x50
#define I2C_EEPROM_PAGEADDR			0
#ifdef LPCLINK2_OSCILLOSCOPE
#define I2C_EEPROM_PAGE_SIZE		16
#else
#define I2C_EEPROM_PAGE_SIZE		8		// 8byte for AT24C02A
#endif

#define I2C_BUFFER_SIZE				32		// I2Cリードに使用するバッファ・サイズ




typedef struct{
	uint8_t divE_divider;
	uint16_t desc_div;
}timescalse_div_val_T;

extern timescalse_div_val_T timescale_div_val[NUM_TIMESCALE];
extern uint16_t sample_length[5];
extern const USBD_API_T *g_pUsbApi;
extern USBD_HANDLE_T g_hUsb;

void set_LED_state(bool);
void m0app_init();
void setup_input_switches();
void I2CTransfer(uint8_t devAddr,uint8_t *txBuffPtr,uint16_t txSize,uint8_t *rxBuffPtr,uint16_t rxSize);
void I2CTransferBlock(uint8_t devAddr,uint8_t *txBuffPtr,uint16_t txSize,uint8_t *rxBuffPtr,uint16_t rxSize);

#endif
