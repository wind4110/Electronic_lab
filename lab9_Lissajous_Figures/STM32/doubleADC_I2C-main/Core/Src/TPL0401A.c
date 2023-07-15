#include "TPL0401A.h"
#include "i2c.h"

void TPL0401A_Init(void)
{
	MX_I2C1_Init();
}

static void TPL0401A_Write(uint8_t *data, uint16_t len)
{
	while(HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(TPL0401A_ADDR << 1), (uint8_t *)data, len, 0xff) != HAL_OK)
	{
		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
		{
			Error_Handler();
		}
	}
}

// static void TPL0401A_Read(uint8_t *data, uint16_t len)
// {
// 	while(HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(TPL0401A_ADDR << 1), (uint8_t *)data, len, 0xff) != HAL_OK)
// 	{
// 		if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
// 		{
// 			Error_Handler();
// 		}		
// 	}
// }

// uint8_t Get_TPL0401A_Value(void)
// {
// 	uint8_t ACR[2] = {0x02, 0x02};
// 	uint8_t WR_Access = 0x00;
// 	uint8_t res = 0;
	
// 	TPL0401A_Write(ACR, 2);
// 	TPL0401A_Write(&WR_Access, 1);
// 	TPL0401A_Read(&res, 1);
	
// 	return res;
// }

void Set_TPL0401A_Value(uint8_t num)
{
	uint8_t data[2] = {0x00, num};

	TPL0401A_Write(data, 2);
}

