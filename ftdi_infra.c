/*!
 * \file infra.c
 *
 * \author FTDI
 * \date 20110317
 *
 * Copyright (C) 2000-2014 Future Technology Devices International Limited
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
 * Module: Infra
 *
 * Rivision History:
 * 0.1  - initial version
 * 0.2  - 20110708 - exported Init_libMPSSE & Cleanup_libMPSSE for Microsoft toolchain support
 * 0.3  - 20111103 - commented & cleaned up
 * 0.41 - 20140903 - fixed compile warnings 
 */


/******************************************************************************/
/*								Include files					  			  */
/******************************************************************************/
#include "ftdi_infra.h"		/*portable infrastructure(datatypes, libraries, etc)*/

/******************************************************************************/
/*								Global variables							  */
/******************************************************************************/
#ifdef INFRA_DEBUG_ENABLE
	//int currentDebugLevel = MSG_INFO;
	int currentDebugLevel = MSG_DEBUG;
#endif

/******************************************************************************/
/*						Global function definitions						  */
/******************************************************************************/

/*!
 * \brief Print function return status
 *
 * All the functions return a status code. This function prints a text to the debug terminal
 * that provides the meaning of the status code.
 *
 * \param[in] status Status code returned by functions
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa
 * \note
 * \warning
 */
FT_STATUS Infra_DbgPrintStatus(FT_STATUS status)
{
	FN_ENTER;

	switch(status)
	{
		case FT_OK:
			DBG(MSG_ERR, "Status: FT_OK\n");
		break;

		case FT_INVALID_HANDLE:
			DBG(MSG_ERR, "Status: FT_INVALID_HANDLE\n");
		break;

		case FT_DEVICE_NOT_FOUND:
			DBG(MSG_ERR, "Status: FT_DEVICE_NOT_FOUND\n");
		break;

		case FT_DEVICE_NOT_OPENED:
			DBG(MSG_ERR, "Status: FT_DEVICE_NOT_OPENED\n");
		break;

		case FT_IO_ERROR:
			DBG(MSG_ERR, "Status: FT_IO_ERROR\n");
		break;

		case FT_INSUFFICIENT_RESOURCES:
			DBG(MSG_ERR, "Status: FT_INSUFFICIENT_RESOURCES\n");
		break;

		case FT_INVALID_PARAMETER:
			DBG(MSG_ERR, "Status: FT_INVALID_PARAMETER\n");
		break;

		case FT_INVALID_BAUD_RATE:
			DBG(MSG_ERR, "Status: FT_INVALID_BAUD_RATE\n");
		break;

		case FT_DEVICE_NOT_OPENED_FOR_ERASE:
			DBG(MSG_ERR, "Status: FT_DEVICE_NOT_OPENED_FOR_ERASE\n");
		break;

		case FT_DEVICE_NOT_OPENED_FOR_WRITE:
			DBG(MSG_ERR, "Status: FT_DEVICE_NOT_OPENED_FOR_WRITE\n");
		break;

		case FT_FAILED_TO_WRITE_DEVICE:
			DBG(MSG_ERR, "Status: FT_FAILED_TO_WRITE_DEVICE\n");
		break;

		case FT_EEPROM_READ_FAILED:
			DBG(MSG_ERR, "Status: FT_EEPROM_READ_FAILED\n");
		break;

		case FT_EEPROM_WRITE_FAILED:
			DBG(MSG_ERR, "Status: FT_EEPROM_WRITE_FAILED\n");
		break;

		case FT_EEPROM_ERASE_FAILED:
			DBG(MSG_ERR, "Status: FT_EEPROM_ERASE_FAILED\n");
		break;

		case FT_EEPROM_NOT_PRESENT:
			DBG(MSG_ERR, "Status: FT_EEPROM_NOT_PRESENT\n");
		break;

		case FT_EEPROM_NOT_PROGRAMMED:
			DBG(MSG_ERR, "Status: FT_EEPROM_NOT_PROGRAMMED\n");
		break;

		case FT_INVALID_ARGS:
			DBG(MSG_ERR, "Status: FT_INVALID_ARGS\n");
		break;

		case FT_NOT_SUPPORTED:
			DBG(MSG_ERR, "Status: FT_NOT_SUPPORTED\n");
		break;

		case FT_OTHER_ERROR:
			DBG(MSG_ERR, "Status: FT_OTHER_ERROR\n");
		break;

#ifndef __linux
/* gives compilation error in linux - not defined in D2XX for linux */
		case FT_DEVICE_LIST_NOT_READY:
			DBG(MSG_ERR, "Status: FT_DEVICE_LIST_NOT_READY\n");
#endif
		break;
			DBG(MSG_ERR, "Status: Unknown Error!\n");
		default:

			;

	}
	FN_EXIT;
	return FT_OK;
}


/******************************************************************************/
/*						Local function definitions						  */
/*!
 * \brief Delay the execution of the thread
 *
 * Delay the execution of the thread
 *
 * \param[in] delay Value of the delay in milliseconds
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa
 * \note The macro INFRA_SLEEP has a resolution of 1 second
 * \warning
 */
FT_STATUS Infra_Delay(uint64 delay)
{
	FT_STATUS status=FT_OTHER_ERROR;
	FN_ENTER;
#ifdef _WIN32
/*TBD*/
	/*status = FT_OK;*/
#endif

#ifdef _LINUX
/*TBD*/
	/*status = FT_OK;*/
#endif

	FN_EXIT;
	return status;
}

/******************************************************************************/
/*						Local function definitions						  */
/******************************************************************************/


#ifdef _WIN32

/*!
 * \brief Module entry point for Windows DLL
 *
 * This function is called by Windows OS when an application loads/unloads libMPSSE as a DLL
 *
 * \param[in] hModule			Handle
 * \param[in] reason_for_call	Reason for being called
 * \param[in] lpReserved		Reserved
 * \return none
 * \sa
 * \note
 * \warning
 */
BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD reason_for_call,LPVOID lpReserved)
{
	//FT_STATUS status=FT_OK;
	FN_ENTER;

    switch (reason_for_call)
  	{
		case DLL_PROCESS_ATTACH:
			DBG(MSG_DEBUG,"reason_for_call = DLL_PROCESS_ATTACH\n");
		break;
		case DLL_THREAD_ATTACH:
			DBG(MSG_DEBUG,"reason_for_call = DLL_THREAD_ATTACH\n");

      	break;

		case DLL_THREAD_DETACH:
			DBG(MSG_DEBUG,"reason_for_call = DLL_THREAD_DETACH\n");

		break;
		case DLL_PROCESS_DETACH:
			DBG(MSG_DEBUG,"reason_for_call = DLL_PROCESS_DETACH\n");
		break;

		default:
			DBG(MSG_WARN,"DllMain was called with an unknown reason\n");
    }

	FN_EXIT;
    return TRUE;
}
#endif /*_WIN32*/

/******************************************************************************/
/*						Public function definations						  */
/******************************************************************************/

/*!
 * \brief Test Function
 *
 * Returns D2XX version number
 *
 * \param[in]	i	dummy input
 * \param[out] *j	D2XX version number is returned here
 * \return Returns status code of type FT_STATUS(see D2XX Programmer's Guide)
 * \sa
 * \warning
 */
FTDI_API FT_STATUS CAL_CONV Infra_TestFunction(unsigned long i, unsigned long *j)
{
	FT_STATUS status=FT_OK;
	FN_ENTER;
	*j = i+100;
	//CHECK_NULL_RET(varFunctionPtrLst.p_FT_GetLibraryVersion);
	//varFunctionPtrLst.p_FT_GetLibraryVersion(j);

	FN_EXIT;
	return status;
}



