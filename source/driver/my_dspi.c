#include "fsl_dspi.h"
#include "my_dspi.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* DSPI user callback */
void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData);

/*******************************************************************************
 * Variables
 ******************************************************************************/
dspi_master_handle_t g_m_handle;
volatile bool isTransferCompleted  = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
void DSPI_MasterUserCallback(SPI_Type *base, dspi_master_handle_t *handle, status_t status, void *userData)
{
    if (status == kStatus_Success) {
        __NOP();
    }

    isTransferCompleted = true;
    DisableIRQ(SPI0_IRQn);
}

void SPI_Config_Request(uint32_t bitlen, uint32_t speed, uint32_t spi_cs_type)
{
    DSPI_MasterTransferCreateHandle(SPI0, &g_m_handle, DSPI_MasterUserCallback, NULL);
}

void  SPI_Tx_Rx_Data(uint8_t *txdata, uint8_t tx_length, uint8_t *rxdata, uint8_t rx_length)
{
    dspi_transfer_t masterXfer;
    status_t dspi_status;

    isTransferCompleted    = false;
    masterXfer.txData      = txdata;
    masterXfer.rxData      = rxdata;
    masterXfer.dataSize    = tx_length;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;
    dspi_status = DSPI_MasterTransferNonBlocking(SPI0, &g_m_handle, &masterXfer);
    if(dspi_status != kStatus_Success) while(1);

    while (!isTransferCompleted){;}

	return;
}


uint32_t DSPI0_TxRxData(uint32_t txdata)
{
  uint32_t rxdata;

  SPI_Tx_Rx_Data((uint8_t *)&txdata, 1, (uint8_t *)&rxdata, 1);

  return rxdata;
}


void DSPI0_TxData(uint32_t txdata)
{
	DSPI0_TxRxData(txdata);
}


uint32_t SPI_RxData(void)
{
    uint32_t rxdata;

    rxdata = DSPI0_TxRxData(0xff);
    return rxdata;
}


void DSPI0_Send_Request(uint8_t dat)
{
	DSPI0_TxData(dat);
}

void DSPI0_Send_Request16(uint16_t dat)
{
	DSPI0_Send_Request((uint8_t)(dat>>8));
	DSPI0_Send_Request((uint8_t)dat);
}
