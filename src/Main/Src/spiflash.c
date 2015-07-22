#include "main.h"
#define JEDEC_ID 0xbf254a
#define EXT_ID 0xbf25
#define SECTOR_SIZE 64*1024
#define N_SECTORS 64
#define PAGE_SIZE 256


/* Flash opcodes. */
#define SPINOR_OP_WREN		0x06	/* Write enable */
#define SPINOR_OP_RDSR		0x05	/* Read status register */
#define SPINOR_OP_WRSR		0x01	/* Write status register 1 byte */
#define SPINOR_OP_READ		0x03	/* Read data bytes (low frequency) */
#define SPINOR_OP_READ_FAST	0x0b	/* Read data bytes (high frequency) */
#define SPINOR_OP_READ_1_1_2	0x3b	/* Read data bytes (Dual SPI) */
#define SPINOR_OP_READ_1_1_4	0x6b	/* Read data bytes (Quad SPI) */
#define SPINOR_OP_PP		0x02	/* Page program (up to 256 bytes) */
#define SPINOR_OP_BE_4K		0x20	/* Erase 4KiB block */
#define SPINOR_OP_BE_4K_PMC	0xd7	/* Erase 4KiB block on PMC chips */
#define SPINOR_OP_BE_32K	0x52	/* Erase 32KiB block */
#define SPINOR_OP_CHIP_ERASE	0xc7	/* Erase whole flash chip */
#define SPINOR_OP_SE		0xd8	/* Sector erase (usually 64KiB) */
#define SPINOR_OP_RDID		0x9f	/* Read JEDEC ID */
#define SPINOR_OP_RDCR		0x35	/* Read configuration register */
#define SPINOR_OP_RDFSR		0x70	/* Read flag status register */
#define SPINOR_OP_BP		0x02	/* Byte program */
#define SPINOR_OP_WRDI		0x04	/* Write disable */
#define SPINOR_OP_AAI_WP	0xad	/* Auto address increment word program */
/* Status Register bits. */
#define SR_WIP			1	/* Write in progress */
#define SR_WEL			2	/* Write enable latch */
/* meaning of other SR_* bits may differ between vendors */
#define SR_BP0			4	/* Block protect 0 */
#define SR_BP1			8	/* Block protect 1 */
#define SR_BP2			0x10	/* Block protect 2 */
#define SR_SRWD			0x80	/* SR write protect */

/* Flag Status Register bits */
#define FSR_READY		0x80
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_AF                      GPIO_AF5_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_AF                     GPIO_AF5_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_AF                     GPIO_AF5_SPI1
#define SPIx_NSS_PIN 					 GPIO_PIN_4
#define SPIx_NSS_GPIO_PORT               GPIOA
#define SPIx_NSS_AF                      GPIO_AF5_SPI1
SPI_HandleTypeDef SpiHandle;

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
  if(hspi->Instance == SPIx)
  {     
    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    /* Enable SPI clock */
    SPIx_CLK_ENABLE(); 
    
    /*##-2- Configure peripheral GPIO ##########################################*/  
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin    = SPIx_NSS_PIN;
	GPIO_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull   = GPIO_PULLUP;
	GPIO_InitStruct.Speed  = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_SET);
  }
}
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if(hspi->Instance == SPIx)
  {   
    /*##-1- Reset peripherals ##################################################*/
    SPIx_FORCE_RESET();
    SPIx_RELEASE_RESET();
    
    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
  }
}
int config_spi()
{
	SpiHandle.Instance				 = SPIx;
	
	  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
	  SpiHandle.Init.Direction		   = SPI_DIRECTION_2LINES;
	  SpiHandle.Init.CLKPhase		   = SPI_PHASE_1EDGE;
	  SpiHandle.Init.CLKPolarity	   = SPI_POLARITY_LOW;
	  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	  SpiHandle.Init.CRCPolynomial	   = 7;
	  SpiHandle.Init.DataSize		   = SPI_DATASIZE_8BIT;
	  SpiHandle.Init.FirstBit		   = SPI_FIRSTBIT_MSB;
	  SpiHandle.Init.NSS			   = SPI_NSS_SOFT;
	  SpiHandle.Init.TIMode 		   = SPI_TIMODE_DISABLE;
	
	  SpiHandle.Init.Mode = SPI_MODE_MASTER;
	
	  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
	  {
		/* Initialization Error */
		printf("init spi failed \n");
		return 0;
	  }
	return 1;
}
int flash_read_reg(unsigned char reg,unsigned char *out,int len)
{
	int result=-1;
	//HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)&reg, (uint8_t *)out, len, 5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_RESET);
	if(HAL_SPI_Transmit(&SpiHandle,(uint8_t *)&reg,1,5000)==HAL_OK)
		result=HAL_SPI_Receive(&SpiHandle,(uint8_t *)out,len,5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_SET);
	return result;
}
int spi_nor_read_id()
{
	int			tmp;
	unsigned char			id[5];
	unsigned long			jedec;
	unsigned short                     ext_jedec;
	config_spi();

	tmp = flash_read_reg(SPINOR_OP_RDID, id, 5);
	if (tmp != HAL_OK) {
		printf(" error %d reading JEDEC ID\n", tmp);
		return 0;
	}
	printf("ID %x %x %x %x %x\n",id[0],id[1],id[2],id[3],id[4]);
	jedec = id[0];
	jedec = jedec << 8;
	jedec |= id[1];
	jedec = jedec << 8;
	jedec |= id[2];

	ext_jedec = id[3] << 8 | id[4];

	if (JEDEC_ID == jedec) 
	{
		if (EXT_ID == ext_jedec)
		{
			printf("Flash ID:%x ext_id %x found\n",jedec,ext_jedec);
			return jedec;
		}
	}
	printf("unrecognized JEDEC id %06x ext id %x\n", jedec,ext_jedec);
	return 0;
}

