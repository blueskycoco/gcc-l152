#include "main.h"
#define JEDEC_ID 0xbf254a
#define EXT_ID 0xbf25
#define SECTOR_SIZE 64*1024
#define N_SECTORS 64
#define TOTAL_SIZE N_SECTORS*SECTOR_SIZE
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
    /* SPI MOSI GPIO pin configuration  */
    //GPIO_InitStruct.Pin = SPIx_NSS_PIN;
    //GPIO_InitStruct.Alternate = SPIx_NSS_AF;
    //HAL_GPIO_Init(SPIx_NSS_GPIO_PORT, &GPIO_InitStruct);

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
	
	  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	  SpiHandle.Init.Direction		   = SPI_DIRECTION_2LINES;
	  SpiHandle.Init.CLKPhase		   = SPI_PHASE_1EDGE;
	  SpiHandle.Init.CLKPolarity	   = SPI_POLARITY_LOW;
	  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
	  SpiHandle.Init.CRCPolynomial	   = 7;
	  SpiHandle.Init.DataSize		   = SPI_DATASIZE_8BIT;
	  SpiHandle.Init.FirstBit		   = SPI_FIRSTBIT_MSB;
	  SpiHandle.Init.NSS			   = SPI_NSS_HARD_OUTPUT;
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
int flash_write_reg(unsigned char reg,unsigned char *out,int len)
{
	int result=-1;
	//HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)&reg, (uint8_t *)out, len, 5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_RESET);
	if(out!=NULL)
	{
		if(HAL_SPI_Transmit(&SpiHandle,(uint8_t *)&reg,1,5000)==HAL_OK)
			result=HAL_SPI_Transmit(&SpiHandle,(uint8_t *)out,len,5000);
	}
	else
		result=HAL_SPI_Transmit(&SpiHandle,(uint8_t *)&reg,1,5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_SET);
	return result;
}
#define	MAX_READY_WAIT_JIFFIES	(40 * 100)
/*
 * Read the status register, returning its value in the location
 * Return the status register value.
 * Returns negative if error occurred.
 */
static int read_sr()
{
	int ret;
	unsigned char val;

	ret = flash_read_reg(SPINOR_OP_RDSR, &val, 1);
	if (ret < 0) {
		printf("error %d reading SR\n", (int) ret);
		return ret;
	}

	return val;
}

/*
 * Write status register 1 byte
 * Returns negative if error occurred.
 */
static inline int write_sr(uint8_t val)
{
	return flash_write_reg(SPINOR_OP_WRSR, &val, 1);
}

/*
 * Set write enable latch with Write Enable command.
 * Returns negative if error occurred.
 */
static inline int write_enable()
{
	return flash_write_reg(SPINOR_OP_WREN, NULL, 0);
}
/*
 * Send write disble instruction to the chip.
 */
static inline int write_disable()
{
	return flash_write_reg(SPINOR_OP_WRDI, NULL, 0);
}

static int wait_till_ready()
{
	unsigned long deadline;
	int sr;
	
	deadline = HAL_GetTick() + MAX_READY_WAIT_JIFFIES;

	do {
		//cond_resched();

		sr = read_sr();
		if (sr < 0)
			break;
		else if (!(sr & SR_WIP))
		{
			return 0;
		}
	} while (HAL_GetTick()<deadline);

	return 1;
}
static int spi_nor_lock(int ofs, int len)
{
	uint32_t offset = ofs;
	uint8_t status_old, status_new;
	int ret = 0;

	/* Wait until finished previous command */
	ret = wait_till_ready();
	if (ret)
		goto err;

	status_old = read_sr();

	if (offset < TOTAL_SIZE - (TOTAL_SIZE / 2))
		status_new = status_old | SR_BP2 | SR_BP1 | SR_BP0;
	else if (offset < TOTAL_SIZE - (TOTAL_SIZE / 4))
		status_new = (status_old & ~SR_BP0) | SR_BP2 | SR_BP1;
	else if (offset < TOTAL_SIZE - (TOTAL_SIZE / 8))
		status_new = (status_old & ~SR_BP1) | SR_BP2 | SR_BP0;
	else if (offset < TOTAL_SIZE - (TOTAL_SIZE / 16))
		status_new = (status_old & ~(SR_BP0 | SR_BP1)) | SR_BP2;
	else if (offset < TOTAL_SIZE - (TOTAL_SIZE / 32))
		status_new = (status_old & ~SR_BP2) | SR_BP1 | SR_BP0;
	else if (offset < TOTAL_SIZE - (TOTAL_SIZE / 64))
		status_new = (status_old & ~(SR_BP2 | SR_BP0)) | SR_BP1;
	else
		status_new = (status_old & ~(SR_BP2 | SR_BP1)) | SR_BP0;

	/* Only modify protection if it will not unlock other areas */
	if ((status_new & (SR_BP2 | SR_BP1 | SR_BP0)) >
				(status_old & (SR_BP2 | SR_BP1 | SR_BP0))) {
		write_enable();
		ret = write_sr(status_new);
		if (ret)
			goto err;
	}

err:
	return ret;
}

static int spi_nor_unlock(int ofs, uint len)
{
	uint32_t offset = ofs;
	uint8_t status_old, status_new;
	int ret = 0;

	/* Wait until finished previous command */
	ret = wait_till_ready();
	if (ret)
		goto err;

	status_old = read_sr();

	if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 64))
		status_new = status_old & ~(SR_BP2 | SR_BP1 | SR_BP0);
	else if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 32))
		status_new = (status_old & ~(SR_BP2 | SR_BP1)) | SR_BP0;
	else if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 16))
		status_new = (status_old & ~(SR_BP2 | SR_BP0)) | SR_BP1;
	else if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 8))
		status_new = (status_old & ~SR_BP2) | SR_BP1 | SR_BP0;
	else if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 4))
		status_new = (status_old & ~(SR_BP0 | SR_BP1)) | SR_BP2;
	else if (offset+len > TOTAL_SIZE - (TOTAL_SIZE / 2))
		status_new = (status_old & ~SR_BP1) | SR_BP2 | SR_BP0;
	else
		status_new = (status_old & ~SR_BP0) | SR_BP2 | SR_BP1;

	/* Only modify protection if it will not lock other areas */
	if ((status_new & (SR_BP2 | SR_BP1 | SR_BP0)) <
				(status_old & (SR_BP2 | SR_BP1 | SR_BP0))) {
		write_enable();
		ret = write_sr(status_new);
		if (ret)
			goto err;
	}

err:
	return ret;
}

int flash_write(int write_second,int op_code,int addr,int len,int *ret,const unsigned char *buf)
{	
	unsigned char send_buf[6];
	int send_len=0;
	int result=-1;
	if(op_code==SPINOR_OP_BP)//byte program
	{
		send_len=5;
	}
	else
	{
		if(write_second)
		{			
			send_len=3;
			send_buf[2]=buf[1];
		}
		else
		{
			send_len=6;
			send_buf[5]=buf[1];
		}
	}
	send_buf[0]=op_code;
	if(write_second&&(op_code!=SPINOR_OP_BP))
	{
		send_buf[1]=buf[0];
	}
	else
	{	
		send_buf[1]=((addr & 0xFFFFFF) >> 16);
		send_buf[2]=((addr & 0xFFFF) >> 8);
		send_buf[3]=(addr & 0xFF);
		send_buf[4]=buf[0];
	}
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_RESET);
	result=HAL_SPI_Transmit(&SpiHandle,(uint8_t *)send_buf,send_len,5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_SET);
	if(result==HAL_OK)
		*ret=len;
	else
	{
		*ret=0;
		printf("write failed\n");
	}
	return result;
}
int flash_read(int addr,int len,int *ret,unsigned char *buf)
{
	int result=-1;
	unsigned char send_buf[5];
	send_buf[0]=SPINOR_OP_READ_FAST;
	send_buf[1]=((addr & 0xFFFFFF) >> 16);
	send_buf[2]=((addr & 0xFFFF) >> 8);
	send_buf[3]=(addr & 0xFF);
	send_buf[4]=0xff;
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_RESET);
	if(HAL_SPI_Transmit(&SpiHandle,(uint8_t *)send_buf,5,5000)==HAL_OK)
		result=HAL_SPI_Receive(&SpiHandle,(uint8_t *)buf,len,5000);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_SET);
	if(result==HAL_OK)
	{
		*ret=len;
	}
	else
		*ret=0;
	return result;

}
int flash_erase_chip()
{
	int ret;
	ret = wait_till_ready();
	if (ret)
		return ret;
	write_enable();
	return flash_write_reg(SPINOR_OP_CHIP_ERASE, NULL, 0);
}
int flash_erase(int addr)
{
	unsigned char send_buf[4];
	int result=-1;
	
	int ret;
	ret = wait_till_ready();
	if (ret)
		return ret;
	write_enable();
	send_buf[0]=SPINOR_OP_BE_4K;
	send_buf[1]=((addr & 0xFFFFFF) >> 16);
	send_buf[2]=((addr & 0xFFFF) >> 8);
	send_buf[3]=(addr & 0xFF);
	HAL_GPIO_WritePin(SPIx_NSS_GPIO_PORT,SPIx_NSS_PIN, GPIO_PIN_RESET);
	result=HAL_SPI_Transmit(&SpiHandle,(uint8_t *)send_buf,4,5000);
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
	//ret=write_enable();
	//if(ret!=0)
	//	return 0;
	//ret=write_sr(0);
	//if(ret!=0)
	//	return 0;

	if (JEDEC_ID == jedec) 
	{
		if (EXT_ID == ext_jedec)
		{
			
			printf("Flash ID:%x ext_id %x found\n",(unsigned int)jedec,ext_jedec);
			return jedec;
		}
	}
	printf("unrecognized JEDEC id %06x ext id %x\n", (unsigned int)jedec,ext_jedec);
	return 0;
}


int spi_nor_write(int to, int len,
		int *retlen, const unsigned char *buf)
{
	int actual;
	int ret=1,sst_write_second=0;
	unsigned char	program_opcode;
	
	printf("to 0x%08x, len %zd\n", to, len);
	ret=spi_nor_unlock(to,len);
	if(!ret)
	{
		/* Wait until finished previous write command. */
		ret = wait_till_ready();
		if (ret)
			goto time_out;

		write_enable();
		sst_write_second = 0;

		actual = to % 2;
		/* Start write from odd address. */
		if (actual) {
				program_opcode = SPINOR_OP_BP;

			/* write one byte. */
			ret=flash_write(sst_write_second,program_opcode,to, 1, retlen, buf);
			if(!ret)
				ret = wait_till_ready();
			
			if (ret)
				goto time_out;
		}
		to += actual;

		/* Write out most of the data here. */
		for (; actual < len - 1; actual += 2) {
			program_opcode = SPINOR_OP_AAI_WP;

			/* write two bytes. */
			ret=flash_write(sst_write_second,program_opcode, to, 2, retlen, buf + actual);
			if(!ret)
				ret = wait_till_ready();
			if (ret)
				goto time_out;
			to += 2;
			sst_write_second = 1;
		}
		sst_write_second = 0;

		write_disable();
		ret = wait_till_ready();
		if (ret)
			goto time_out;

		/* Write out trailing byte if it exists. */
		if (actual != len) {
			write_enable();

			program_opcode = SPINOR_OP_BP;
			ret=flash_write(sst_write_second,program_opcode, to, 1, retlen, buf + actual);
			if(!ret)
				ret = wait_till_ready();
			if (ret)
				goto time_out;
			write_disable();
		}
	}
time_out:
	if(!ret)
	spi_nor_lock(to,len);
	return ret;
}
int spi_nor_read(int from, int len,
			int *retlen, unsigned char *buf)
{
	int ret;

	//printf("from 0x%08x, len %d\n", from, len);
	ret = flash_read(from, len, retlen, buf);
	return ret;
}
/*
 * Erase an address range on the nor chip.  The address range may extend
 * one or more erase sectors.  Return an error is there is a problem erasing.
 */
int spi_nor_erase(int addr ,int len)
{
	int ret=0;
	int erasesize=4096;
	printf("at 0x%llx, len %lld\n", (long long)addr,
			(long long)len);
	ret=spi_nor_unlock(addr,len);
	if(!ret)
	{
		/* whole-chip erase? */
		if (len == TOTAL_SIZE) {
			if (flash_erase_chip()) {
				ret = -1;
				goto erase_err;
			}

		/* REVISIT in some cases we could speed up erasing large regions
		 * by using SPINOR_OP_SE instead of SPINOR_OP_BE_4K.  We may have set up
		 * to use "small sector erase", but that's not always optimal.
		 */

		/* "sector"-at-a-time erase */
		} else {
			while (len>0) {
				if (flash_erase(addr)) {
					ret = -1;
					goto erase_err;
				}

				addr += erasesize;
				len -= erasesize;
			}
		}
	}
erase_err:
	if(!ret) 
		spi_nor_lock(addr,len);
	return ret;
}

