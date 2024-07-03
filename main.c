#include "stm32f4xx_hal.h"

#define SCK GPIO_PIN_5
#define MOSI GPIO_PIN_7
#define MISO GPIO_PIN_6

#define CS GPIO_PIN_3

#define TIMEOUT_TIME 1000

SPI_HandleTypeDef spi_handle;

uint8_t sendByte(uint8_t byteToSend);
void writeData(uint8_t address, uint8_t dataToWrite);
uint8_t readData(uint8_t address);
uint8_t receiveData[8];

void SPI1_Init(void);
void GPIO_Init(void);
uint8_t L3GD20_ReadWHOAMI(void);

SPI_HandleTypeDef spi_handle = {0};

int main(void)
{
	HAL_Init();
	GPIO_Init();
	SPI1_Init();
	writeData(0x20, 0x0F);

	uint8_t who_am_i = L3GD20_ReadWHOAMI();

	while (1)
	{
		receiveData[0] = readData(0x8F);
	}
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}

void SPI1_Init(void)
{
	spi_handle.Instance = SPI1;
	spi_handle.Init.Mode = SPI_MODE_MASTER;
	spi_handle.Init.Direction = SPI_DIRECTION_2LINES;
	spi_handle.Init.DataSize = SPI_DATASIZE_8BIT;
	spi_handle.Init.CLKPolarity = SPI_POLARITY_HIGH;
	spi_handle.Init.CLKPhase = SPI_PHASE_2EDGE;
	spi_handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
	spi_handle.Init.NSS = SPI_NSS_SOFT;
	spi_handle.Init.TIMode = SPI_TIMODE_DISABLE;
	spi_handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	spi_handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	spi_handle.Init.CRCPolynomial = 7;
	HAL_SPI_MspInit(&spi_handle);

	__HAL_RCC_SPI1_CLK_ENABLE();

	HAL_SPI_Init(&spi_handle);
	__HAL_SPI_ENABLE_IT(&spi_handle, SPI_IT_RXNE);
	__HAL_SPI_ENABLE_IT(&spi_handle, SPI_IT_TXE);
	__HAL_SPI_ENABLE_IT(&spi_handle, SPI_IT_ERR);
}

void GPIO_Init(void)
{
	GPIO_InitTypeDef gpio_spi_out = {0};
	GPIO_InitTypeDef gpio_spi_in = {0};
	GPIO_InitTypeDef gpio_cs = {0};

	gpio_spi_out.Mode = GPIO_MODE_AF_PP;
	gpio_spi_out.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_spi_out.Pin = SCK | MOSI | MISO;
	gpio_spi_out.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpio_spi_out);

	gpio_cs.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_cs.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_cs.Pull = GPIO_NOPULL;
	gpio_cs.Pin = CS;
	HAL_GPIO_Init(GPIOE, &gpio_cs);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	HAL_GPIO_WritePin(GPIOE, CS, GPIO_PIN_SET);
}

uint8_t L3GD20_ReadWHOAMI(void)
{
	int result = 0;
	uint8_t rx_data, tx_data = 0x8F;
	HAL_GPIO_WritePin(GPIOE, CS, GPIO_PIN_RESET);
	result = HAL_SPI_Transmit(&spi_handle, &tx_data, 1, HAL_MAX_DELAY);
	result = HAL_SPI_Receive(&spi_handle, &rx_data, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOE, CS, GPIO_PIN_SET);
	return rx_data;
}

uint8_t sendByte(uint8_t byteToSend)
{
	uint32_t timeout = TIMEOUT_TIME;

	while (HAL_SPI_GetState(&spi_handle) != HAL_SPI_STATE_READY && timeout-- > 0)
		;

	if (timeout == 0)
	{

		return 0;
	}

	uint8_t receivedByte;
	HAL_SPI_TransmitReceive(&spi_handle, &byteToSend, &receivedByte, 1, HAL_MAX_DELAY);

	return receivedByte;
}

void writeData(uint8_t address, uint8_t dataToWrite)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
	sendByte(address);
	sendByte(dataToWrite);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);
}

uint8_t readData(uint8_t address)
{
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET);
	sendByte(address | 0x80);
	uint8_t receivedByte = sendByte(0x00);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);

	return receivedByte;
}
