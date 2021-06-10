/*!
 * \file sample-static.c
 *
 * \author FTDI
 * \date 20110512
 *
 * Copyright © 2000-2014 Future Technology Devices International Limited
 *
 *
 * THIS SOFTWARE IS PROVIDED BY FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL FUTURE TECHNOLOGY DEVICES INTERNATIONAL LIMITED
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Project: libMPSSE
 * Module: I2C Sample Application - Interfacing 24LC024H I2C EEPROM and ADS1015IDGST I2C ADC
 *
 * Rivision History:
 * 0.1 - 20110513 - initial version
 * 0.2 - 20110801 - Changed LatencyTimer to 255
 *					Attempt to open channel only if available
 *					Added & modified macros
 *					Change APIs I2C_GetChannelInfo & OpenChannel to start indexing from 0
 * 0.3 - 20111212 - Added comments
 * 0.5 - 20140918 - Changed to multiple byte transfer and continuous transfer
 *					Added testing of ADC I2C device in addition to the EEPROM I2C device
 */

/******************************************************************************/
/* 							 Include files										   */
/******************************************************************************/
/* Standard C libraries */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/* OS specific libraries */
#ifdef _WIN32
#include<windows.h>
#endif // _WIN32

/* Include D2XX header*/
#include "ftd2xx.h"

/* Include libMPSSE header */
#include "libMPSSE_i2c.h"


/******************************************************************************/
/*								Macro and type defines							   */
/******************************************************************************/
/* Helper macros */


#define APP_CHECK_STATUS(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
	!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);exit(1);}else{;}};
#define APP_CHECK_STATUS_NOEXIT(exp) {if(exp!=FT_OK){printf("%s:%d:%s(): status(0x%x) \
	!= FT_OK\n",__FILE__, __LINE__, __FUNCTION__,exp);}else{;}};
#define CHECK_NULL(exp){if(exp==NULL){printf("%s:%d:%s():  NULL expression \
	encountered \n",__FILE__, __LINE__, __FUNCTION__);exit(1);}else{;}};

/* Application specific macro definations */
#define I2C_DEVICE_ADDRESS_EEPROM		0x57
#define I2C_DEVICE_BUFFER_SIZE			256
#define I2C_WRITE_COMPLETION_RETRY		10
#define START_ADDRESS_EEPROM 			0x00 /*read/write start address inside the EEPROM*/
#define END_ADDRESS_EEPROM				0x10 /*16 bytes buffer only*/
#define RETRY_COUNT_EEPROM				10	/* number of retries if read/write fails */
#define EEPROM_DATA_OFFSET				5
#define EEPROM_DATA_LEN					(END_ADDRESS_EEPROM-START_ADDRESS_EEPROM)
#define I2C_DEVICE_ADDRESS_ADC			0x48
#define ADC_DATA_LEN					2
#define CHANNEL_TO_OPEN					0	/*0 for first available channel, 1 for next... */

/* Application configuration/debugging */
#define TEST_EEPROM						1
#define FAST_TRANSFER					0
#define WRITE_ONCE						0
#define CATCH_GLITCH					0


/******************************************************************************/
/*								Global variables							  	    */
/******************************************************************************/
static FT_HANDLE ftHandle;
static uint8 buffer[I2C_DEVICE_BUFFER_SIZE] = {0};
static uint32 timeWrite = 0;
static uint32 timeRead = 0;
#ifdef _WIN32
static LARGE_INTEGER llTimeStart = {0};
static LARGE_INTEGER llTimeEnd = {0};
static LARGE_INTEGER llFrequency = {0};
#endif // _WIN32

/******************************************************************************/
/*						Public function definitions						  		   */
/******************************************************************************/
/*!
 * \brief Initialize libMPSSE library
 *
 * This function initialize the static library or dynamic library
 *
 * \param[in] None
 * \return Returns true if successful
 * \note
 * \warning
 */
static uint8 initialize_library()
{
#ifdef _MSC_VER
	Init_libMPSSE();
#endif

	return 1;
}

/*!
 * \brief Cleans up libMPSSE library
 *
 * This function cleans up the static library or dynamic library
 *
 * \param[in] None
 * \return None
 * \note
 * \warning
 */
static void cleanup_library()
{
#ifdef _MSC_VER
	Cleanup_libMPSSE();
#endif // _MSC_VER
}

/*!
 * \brief Initialize timer
 *
 * This function initializes the timer
 *
 * \param[in] None
 * \return None
 * \note
 * \warning
 */
static void init_time()
{
#ifdef _WIN32
	QueryPerformanceFrequency(&llFrequency);
#endif
}

/*!
 * \brief Starts timer
 *
 * This function starts the timer
 *
 * \param[in] None
 * \return None
 * \note
 * \warning
 */
static void start_time()
{
#ifdef _WIN32
    QueryPerformanceCounter(&llTimeStart);
#endif
}

/*!
 * \brief Stops timer
 *
 * This function stops the timer
 *
 * \param[in] None
 * \return None
 * \note
 * \warning
 */
static uint32 stop_time()
{
	uint32 dwTemp = 0;
#ifdef _WIN32
    QueryPerformanceCounter(&llTimeEnd);
    dwTemp = (uint32)((llTimeEnd.QuadPart - llTimeStart.QuadPart) * 1000 / (float)llFrequency.QuadPart);
#endif
    return dwTemp;
}

/*!
 * \brief Writes to EEPROM
 *
 * This function writes a number of bytes to a specified address within the 24LC024H EEPROM
 *
 * \param[in] slaveAddress Address of the I2C slave (EEPROM)
 * \param[in] registerAddress Address of the memory location inside the slave to where the byte
 *			is to be written
 * \param[in] data Bytes that is to be written
 * \param[in] numBytes Number of bytes to read
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Datasheet of 24LC024H http://ww1.microchip.com/downloads/en/devicedoc/22102a.pdf
 * \note
 * \warning
 */
static FT_STATUS write_bytes(uint8 slaveAddress, uint8 registerAddress, const uint8 *data, uint32 numBytes)
{
	FT_STATUS status;
	uint32 bytesToTransfer = 0;
	uint32 bytesTransfered = 0;
	uint32 options = 0;
	uint32 trials = 0;

	
#if FAST_TRANSFER
	options = I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT|I2C_TRANSFER_OPTIONS_FAST_TRANSFER_BYTES;
#else // FAST_TRANSFER
	options = I2C_TRANSFER_OPTIONS_START_BIT|I2C_TRANSFER_OPTIONS_STOP_BIT;
#endif // FAST_TRANSFER

	buffer[bytesToTransfer++] = registerAddress;
	memcpy(buffer + bytesToTransfer, data, numBytes);
	bytesToTransfer += numBytes;

	start_time();
	status = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
	timeWrite = stop_time();
	while (status != FT_OK && trials < 10)
	{
		APP_CHECK_STATUS_NOEXIT(status);
		start_time();
		status = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
		timeWrite = stop_time();
		trials++;
	}

	return status;
}

/*!
 * \brief Reads from EEPROM
 *
 * This function reads a number of bytes from a specified address within the 24LC024H EEPROM
 *
 * \param[in] slaveAddress Address of the I2C slave (EEPROM)
 * \param[in] registerAddress Address of the memory location inside the slave from where the
 *			byte is to be read
 * \param[in] *data Address to where the bytes are to be read
 * \param[in] numBytes Number of bytes to read
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa Datasheet of 24LC024H http://ww1.microchip.com/downloads/en/devicedoc/22102a.pdf
 * \note
 * \warning
 */
static FT_STATUS read_bytes(uint8 slaveAddress, uint8 registerAddress, uint8 bRegisterAddress, uint8 *data, uint32 numBytes)
{
	FT_STATUS status = FT_OK;
	uint32 bytesToTransfer = 0;
	uint32 bytesTransfered = 0;
	uint32 options = 0;
	uint32 trials = 0;

	
	if (bRegisterAddress)
	{
#if FAST_TRANSFER
		options = I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT | I2C_TRANSFER_OPTIONS_FAST_TRANSFER_BYTES;
#else // FAST_TRANSFER
		options = I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT;
#endif // FAST_TRANSFER
		buffer[bytesToTransfer++] = registerAddress;
		status = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
		trials = 0;
		while (status != FT_OK && trials < 10)
		{
			APP_CHECK_STATUS_NOEXIT(status);
			status = I2C_DeviceWrite(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
			trials++;
		}
		if (status != FT_OK)
		{
			return status;
		}
		//APP_CHECK_STATUS(status);
	}

	bytesTransfered = 0;
	bytesToTransfer = numBytes;
#if FAST_TRANSFER
	options = I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT | I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE | I2C_TRANSFER_OPTIONS_FAST_TRANSFER_BYTES;
#else // FAST_TRANSFER
	options = I2C_TRANSFER_OPTIONS_START_BIT | I2C_TRANSFER_OPTIONS_STOP_BIT | I2C_TRANSFER_OPTIONS_NACK_LAST_BYTE;
#endif // FAST_TRANSFER

	start_time();
	status |= I2C_DeviceRead(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
	timeRead = stop_time();

	trials = 0;
	while (status != FT_OK && trials < 10)
	{
		APP_CHECK_STATUS_NOEXIT(status);
		start_time();
		status = I2C_DeviceRead(ftHandle, slaveAddress, bytesToTransfer, buffer, &bytesTransfered, options);
		timeRead = stop_time();
		trials++;
	}
	if (status == FT_OK)
	{
		memcpy(data, buffer, bytesToTransfer);
	}

	return status;
}

/*!
 * \brief EEPROM testing / Entry point of the sample application for testing EEPROM
 *
 * This function is the entry point to the sample application for EEPROM. 
 * It opens the channel, writes to the EEPROM and reads back.
 *
 * \param[in] none
 * \return none
 * \sa
 * \note
 * \warning
 */
void TestDeviceEEPROM()
{
	FT_STATUS status = FT_OK;
	uint32 glitch = 0;
	uint32 count=0;
	uint8 dataOUT[EEPROM_DATA_LEN] = {0};		
	uint8 address = 0;


#if WRITE_ONCE
	// Prepare the data to write
	for(address=START_ADDRESS_EEPROM; address<END_ADDRESS_EEPROM; address++)
	{
		dataOUT[address] = address + 10;
	}
		
	// Write the data
	{
		address = START_ADDRESS_EEPROM;	
		status = write_bytes(I2C_DEVICE_ADDRESS_EEPROM, address, dataOUT, EEPROM_DATA_LEN);
		APP_CHECK_STATUS(status);
		printf("write_bytes %d\n", status);
		//Sleep(1000);
	}		
#endif // WRITE_ONCE


#if CATCH_GLITCH		
	uint8 ucGPIOvalue = 0;
	status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
	printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);
#endif // CATCH_GLITCH

	// Read the data multiple times
	while (++count)
	{
#if !WRITE_ONCE
		// Prepare the data to write
		for(address=START_ADDRESS_EEPROM; address<END_ADDRESS_EEPROM; address++)
		{
			dataOUT[address] = address + (uint8)count; // count changes so data always changes every while loop
		}
			
		// Write the data
		{
			address = START_ADDRESS_EEPROM;	
			status = write_bytes(I2C_DEVICE_ADDRESS_EEPROM, address, dataOUT, EEPROM_DATA_LEN);
			if (status != FT_OK)
			{
				printf("Glitch 0 write_bytes failed! count=%ud glitch=%ud\n", (unsigned int)count, (unsigned int)glitch);
				glitch++;
				continue;
			}
			//printf("write_bytes %d\n", status);
			//Sleep(1000);
		}		
#endif // WRITE_ONCE
		
		do
		{
			uint8 dataIN[EEPROM_DATA_LEN] = {0};
			address = START_ADDRESS_EEPROM;
			status = read_bytes(I2C_DEVICE_ADDRESS_EEPROM, address, 1, dataIN, EEPROM_DATA_LEN);
			if (status != FT_OK)
			{
				printf("Glitch 1 write_bytes failed! count=%ud glitch=%ud\n", (unsigned int)count, (unsigned int)glitch);
				glitch++;
				break;
			}

			// Compare the write and read data
			if (memcmp(dataIN, dataOUT, EEPROM_DATA_LEN) == 0)
			{
#ifdef _WIN32
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				printf("[%02d-%02d:%02d:%02d] write & read %d bytes same (glitch = %d) (write = %d, read = %d)\n", 
					st.wDay, st.wHour, st.wMinute, st.wSecond, EEPROM_DATA_LEN, glitch, timeWrite, timeRead);
#else // _WIN32
				printf("write & read %d bytes same (glitch = %ud)\n", EEPROM_DATA_LEN, (unsigned int)glitch);
#endif // _WIN32
				break;
			}
			else
			{
				// NOTES:
				// Google Chrome is changing the Windows timer resolution.
				// http://randomascii.wordpress.com/2013/07/08/windows-timer-resolution-megawatts-wasted/
				// Visual Studio is also changing the Windows timer resolution when mouse cursor is pointed at the scrollbar.
				// This causes the libMPSSE to run faster.
				// Sudden changes in timing can causes issues like glitches.
				// To avoid these kind of issues, it is best to not run applications that change the Windows timer resolution.

				for(address=START_ADDRESS_EEPROM; address<END_ADDRESS_EEPROM; address++)
				{
					if (dataOUT[address] == dataIN[address])
					{
						printf("[%d] write %d read %d\n", address, dataOUT[address], dataIN[address]);
					}
					else
					{
						printf("[%d] write %d read %d ERROR!!!\n", address, dataOUT[address], dataIN[address]);
					}
				}
				glitch++;

#if CATCH_GLITCH
				ucGPIOvalue = 0;
				status = FT_WriteGPIO(ftHandle, 255, ucGPIOvalue);
				//printf("FT_WriteGPIO %d %d\n", status, ucGPIOvalue);
				status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
				printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);
#endif // CATCH_GLITCH

				break;
			}

		} while (1);

#if CATCH_GLITCH
		status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
		if (ucGPIOvalue == 0)
		{
			printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);	
			system("pause");
			break;
		}
#endif // CATCH_GLITCH				
	}
}

/*!
 * \brief ADC ADS1015IDGST testing / Entry point of the sample application for testing ADC
 *
 * This function is the entry point to the sample application for ADC. 
 * It opens the channel, writes to the ADC and reads back.
 *
 * \param[in] none
 * \return none
 * \sa
 * \note
 * \warning
 */
void TestDeviceADC()
{
	FT_STATUS status = FT_OK;
	uint32 glitch = 0;
	uint32 count=0;
	uint8 dataOUT[ADC_DATA_LEN] = {0};		


	// Write to config register
	{
		dataOUT[0] = 0x04;	// Value to write to upper byte of config reg
		dataOUT[1] = 0x00;	// Value to write to lower byte of config reg
		status = write_bytes(I2C_DEVICE_ADDRESS_ADC, 0x1, dataOUT, ADC_DATA_LEN);
		APP_CHECK_STATUS(status);
		printf("write_bytes %d\n", status);
	}		


#if CATCH_GLITCH		
	uint8 ucGPIOvalue = 0;
	status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
	printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);
#endif // CATCH_GLITCH

	// Read the data multiple times
	while (++count)
	{
		do
		{
			uint8 dataIN[ADC_DATA_LEN] = {0};
			status = read_bytes(I2C_DEVICE_ADDRESS_ADC, 0, 0, dataIN, ADC_DATA_LEN);
			// Compare the write and read data
			if (memcmp(dataIN, dataOUT, ADC_DATA_LEN) == 0 && status == FT_OK)
			{
#ifdef _WIN32
				SYSTEMTIME st = {0};
				GetLocalTime(&st);
				printf("[%02d-%02d:%02d:%02d] read %d bytes same (glitch = %d) (read = %d ms)\n", 
					st.wDay, st.wHour, st.wMinute, st.wSecond, ADC_DATA_LEN, glitch, timeRead);
#else // _WIN32
				printf("read %d bytes same (glitch = %ud)\n", ADC_DATA_LEN, (unsigned int)glitch);
#endif // _WIN32
				break;
			}
			else
			{
				// NOTES:
				// Google Chrome is changing the Windows timer resolution.
				// http://randomascii.wordpress.com/2013/07/08/windows-timer-resolution-megawatts-wasted/
				// Visual Studio is also changing the Windows timer resolution when mouse cursor is pointed at the scrollbar.
				// This causes the libMPSSE to run faster.
				// Sudden changes in timing can causes issues like glitches.
				// To avoid these kind of issues, it is best to not run applications that change the Windows timer resolution.

				glitch++;
				printf("[status %d] ERROR!!! write %d,%d read %d,%d (glitch %ud)\n", status, dataOUT[0], dataOUT[1], dataIN[0], dataIN[1], (unsigned int)glitch);
				system("pause");

#if CATCH_GLITCH
				ucGPIOvalue = 0;
				status = FT_WriteGPIO(ftHandle, 255, ucGPIOvalue);
				//printf("FT_WriteGPIO %d %d\n", status, ucGPIOvalue);
				status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
				printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);
				break;
#endif // CATCH_GLITCH
			}

		} while (1);

#if CATCH_GLITCH
		status = FT_ReadGPIO(ftHandle, &ucGPIOvalue);
		if (ucGPIOvalue == 0)
		{
			printf("FT_ReadGPIO  %d %d\n", status, ucGPIOvalue);	
			system("pause");
			break;
		}
#endif // CATCH_GLITCH				
	}
}

/*!
 * \brief Main function / Entry point of the sample application
 *
 * This function is the entry point to the sample application. It opens the channel, writes to the
 * EEPROM and reads back.
 *
 * \param[in] none
 * \return Returns 0 for success
 * \sa
 * \note
 * \warning
 */
int main()
{
	FT_STATUS status = FT_OK;
	FT_DEVICE_LIST_INFO_NODE devList = {0};
	ChannelConfig channelConf;
	uint32 channels = 0;
	uint32 i = 0;


	// Initialize libMPSSE library
	if (!initialize_library())
	{
		printf("initialize_library failed!\n");
		return 0;
	}

	// Initialize channel configurations
	memset(&channelConf, 0, sizeof(channelConf));
	channelConf.ClockRate = I2C_CLOCK_FAST_MODE;
#if FAST_TRANSFER
	channelConf.LatencyTimer = 1;
#else // FAST_TRANSFER
	channelConf.LatencyTimer = 255;
#endif // FAST_TRANSFER

	// Get the number of channels
	status = I2C_GetNumChannels(&channels);
	APP_CHECK_STATUS(status);
	printf("Number of available I2C channels = %d\n",(int)channels);
	if(channels>0)
	{
		for(i=0;i<channels;i++)
		{
			// Get the channel information
			status = I2C_GetChannelInfo(i,&devList);
			APP_CHECK_STATUS(status);
			printf("Information on channel number %ud:\n",(unsigned int)i);
			/*print the dev info*/
			printf("		Flags=0x%x\n",devList.Flags);
			printf("		Type=0x%x\n",devList.Type);
			printf("		ID=0x%x\n",devList.ID);
			printf("		LocId=0x%x\n",devList.LocId);
			printf("		SerialNumber=%s\n",devList.SerialNumber);
			printf("		Description=%s\n",devList.Description);
			printf("		ftHandle=0x%x\n",(unsigned int)devList.ftHandle);/* 0 if not open*/
		}

		// Open the first available channel
		status = I2C_OpenChannel(CHANNEL_TO_OPEN,&ftHandle);
		APP_CHECK_STATUS(status);
		printf("\nhandle=0x%x status=%d\n",(unsigned int)ftHandle,(unsigned int)status);
		
		// Initialize the channel
		status = I2C_InitChannel(ftHandle,&channelConf);
		APP_CHECK_STATUS(status);

		// Initialize the timer
		init_time();

#if TEST_EEPROM
		TestDeviceEEPROM();
#else // TEST_EEPROM
		TestDeviceADC();
#endif // TEST_EEPROM

		status = I2C_CloseChannel(ftHandle);
	}

	cleanup_library();

#ifdef _WIN32
	system("pause");
#endif
	return 0;
}

