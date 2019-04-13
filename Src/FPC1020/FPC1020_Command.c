/*
 * FPC1020_Command.c
 *
 *  Created on: Jan 24, 2019
 *      Author: tuhuy
 */

#include "FPC1020_Command.h"
#include "stm32l4xx_hal.h"
#include "string.h"
#include "Define.h"

DWORD g_dwPacketSize = 0;
BYTE g_Packet[10*64];

PST_CMD_PACKET	g_pCmdPacket = (PST_CMD_PACKET)g_Packet;
PST_RCM_PACKET	g_pRcmPacket = (PST_RCM_PACKET)g_Packet;
extern UART_HandleTypeDef huart1;
UART_HandleTypeDef* uartFinger=&huart1;
void InitCmdPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID, BYTE* p_pbyData, WORD p_wDataLen)
{
	int		i;
	WORD	w_wCheckSum;

	memset( g_Packet, 0, sizeof(g_Packet));
//	for(int j=0;j<sizeof(g_Packet);j++)
//	{
//		g_Packet[j]=0;
//	}

	g_pCmdPacket->m_wPrefix = CMD_PREFIX_CODE;
	g_pCmdPacket->m_bySrcDeviceID = p_bySrcDeviceID;
	g_pCmdPacket->m_byDstDeviceID = p_byDstDeviceID;
	g_pCmdPacket->m_wCMDCode = p_wCMDCode;
	g_pCmdPacket->m_wDataLen = p_wDataLen;

	if (p_wDataLen)
		memcpy(g_pCmdPacket->m_abyData, p_pbyData, p_wDataLen);

	w_wCheckSum = 0;

	for (i=0; i<sizeof(ST_CMD_PACKET)-2; i++)
	{
		w_wCheckSum = w_wCheckSum + g_Packet[i];
	}

	g_pCmdPacket->m_wCheckSum = w_wCheckSum;

	g_dwPacketSize = sizeof(ST_CMD_PACKET);
}

/***************************************************************************/
/***************************************************************************/
void InitCmdDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID, BYTE* p_pbyData, WORD p_wDataLen)
{
	int		i;
	WORD	w_wCheckSum;

	g_pCmdPacket->m_wPrefix = CMD_DATA_PREFIX_CODE;
	g_pCmdPacket->m_bySrcDeviceID = p_bySrcDeviceID;
	g_pCmdPacket->m_byDstDeviceID = p_byDstDeviceID;
	g_pCmdPacket->m_wCMDCode = p_wCMDCode;
	g_pCmdPacket->m_wDataLen = p_wDataLen;

	memcpy(&g_pCmdPacket->m_abyData[0], p_pbyData, p_wDataLen);

	//. Set checksum
	w_wCheckSum = 0;

	for (i=0; i<(p_wDataLen + 8); i++)
	{
		w_wCheckSum = w_wCheckSum + g_Packet[i];
	}

	g_Packet[p_wDataLen+8] = (w_wCheckSum);
	g_Packet[p_wDataLen+9] = (w_wCheckSum)>>8;

	g_dwPacketSize = p_wDataLen + 10;
}
/***************************************************************************/
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
bool SendCommand(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{

	HAL_StatusTypeDef	w_nResult ;



	//w_nResult = g_Serial.Write(g_Packet, g_dwPacketSize , &w_nSendCnt, NULL, COMM_TIMEOUT);
//char stt[10];
//sprintf(stt,"Hello");
//while(1)
//{
//w_nResult=HAL_UART_Transmit(&huart6,(uint8_t*)stt,5,COMM_TIMEOUT);
//HAL_Delay(500);
//}

	w_nResult=HAL_UART_Transmit(uartFinger,g_Packet,g_dwPacketSize,COMM_TIMEOUT);
	if( w_nResult==HAL_ERROR)
	{
		return false;
	}

	return ReceiveAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
}



/***************************************************************************/
/***************************************************************************/
bool CheckReceive( BYTE* p_pbyPacket, DWORD p_dwPacketLen, WORD p_wPrefix, WORD p_wCMDCode )
{
	int				i;
	WORD			w_wCalcCheckSum, w_wCheckSum;
	ST_RCM_PACKET*	w_pstRcmPacket;

	w_pstRcmPacket = (ST_RCM_PACKET*)p_pbyPacket;

	//. Check prefix code
 	if (p_wPrefix != w_pstRcmPacket->m_wPrefix)
 		return false;

	//. Check checksum
	w_wCheckSum = (uint16_t)p_pbyPacket[p_dwPacketLen-2]| p_pbyPacket[p_dwPacketLen-1]<<8;
	w_wCalcCheckSum = 0;
	for (i=0; i<p_dwPacketLen-2; i++)
	{
		w_wCalcCheckSum = w_wCalcCheckSum + p_pbyPacket[i];
	}

	if (w_wCheckSum != w_wCalcCheckSum)
		return false;

	if (p_wCMDCode != w_pstRcmPacket->m_wCMDCode)
	{
		return false;
	}

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool ReceiveAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{

	DWORD	w_dwTimeOut = COMM_TIMEOUT;

	if (p_wCMDCode == CMD_TEST_CONNECTION)
		w_dwTimeOut = 2000;
	else if (p_wCMDCode == CMD_ADJUST_SENSOR)
		w_dwTimeOut = 30000;
WORD packetLength=sizeof(ST_RCM_PACKET);
	bool ketqua;

	ketqua=ReadDataN(g_Packet, packetLength, w_dwTimeOut);
	if (!ketqua) //doc cac packet vao mang g_Packet
	{
		return false;
	}

	g_dwPacketSize = sizeof(ST_RCM_PACKET);

	if (!CheckReceive(g_Packet, sizeof(ST_RCM_PACKET), RCM_PREFIX_CODE, p_wCMDCode))
		return false;

	return true;
}

/***************************************************************************/
/***************************************************************************/
bool SendDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nSendCnt = 0;
	long	w_nResult = 0;


	w_nResult=HAL_UART_Transmit(uartFinger,g_Packet,g_dwPacketSize,COMM_TIMEOUT);
	//w_nResult = g_Serial.Write(g_Packet, g_dwPacketSize , &w_nSendCnt, NULL, COMM_TIMEOUT);

	if(ERR_SUCCESS != w_nResult)
	{
//		DEBUG("ERROR");
		return false;
	}

	return ReceiveDataAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
}
/***************************************************************************/
/***************************************************************************/
bool ReceiveDataPacket(WORD	p_wCMDCode, BYTE p_byDataLen, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	return ReceiveDataAck(p_wCMDCode, p_bySrcDeviceID, p_byDstDeviceID);
}

/***************************************************************************/
/***************************************************************************/
bool ReceiveDataAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID)
{
	DWORD	w_nAckCnt = 0;
	long	w_nResult = 0;
	DWORD	w_dwTimeOut = COMM_TIMEOUT;


	if (!ReadDataN(g_Packet, 8, w_dwTimeOut))
	{
		//::MessageBox(NULL, _T("Please check connection with device"), _T("Err"), MB_ICONWARNING );
		//DEBUG("Please check connection with device");
		return false;
	}


	if (!ReadDataN(g_Packet+8, g_pRcmPacket->m_wDataLen+2, w_dwTimeOut))
	{
		//::MessageBox(NULL, _T("Please check connection with device"), _T("Err"), MB_ICONWARNING );
	//	DEBUG("Please check connection with device");
		return false;
	}

	g_dwPacketSize = g_pRcmPacket->m_wDataLen + 10;

	//::SendMessage(g_hMainWnd, WM_RCM_PACKET_HOOK, 0, 0);

	return CheckReceive(g_Packet, 10 + g_pRcmPacket->m_wDataLen, RCM_DATA_PREFIX_CODE, p_wCMDCode);
}

/***************************************************************************/
/* Doc p_nLen du lieu vao bo dem duoc chi ra boi con tro p_pData*/
/***************************************************************************/
bool ReadDataN(BYTE* p_pData, int p_nLen, DWORD p_dwTimeOut)
{

	HAL_StatusTypeDef	w_nResult ;
	w_nResult= HAL_UART_Receive(uartFinger,p_pData,p_nLen,COMM_TIMEOUT);
		if ( w_nResult==HAL_ERROR )
		{
			return false;
		}
	return true;
}


/***************************************************************************/
/***************************************************************************/
/*string GetErrorMsg(DWORD p_dwErrorCode)
{
	String w_ErrMsg;

	switch((uint8_t)(p_dwErrorCode))
	{
	case ERROR_SUCCESS:
		w_ErrMsg = "Succcess";
		break;
	case ERR_VERIFY:
		w_ErrMsg = "Verify NG";
		break;
	case ERR_IDENTIFY:
		w_ErrMsg = "Identify NG";
		break;
	case ERR_EMPTY_ID_NOEXIST:
		w_ErrMsg = "Empty Template no Exist";
		break;
	case ERR_BROKEN_ID_NOEXIST:
		w_ErrMsg = "Broken Template no Exist";
		break;
	case ERR_TMPL_NOT_EMPTY:
		w_ErrMsg = "Template of this ID Already Exist";
		break;
	case ERR_TMPL_EMPTY:
		w_ErrMsg = "This Template is Already Empty";
		break;
	case ERR_INVALID_TMPL_NO:
		w_ErrMsg = "Invalid Template No";
		break;
	case ERR_ALL_TMPL_EMPTY:
		w_ErrMsg = "All Templates are Empty";
		break;
	case ERR_INVALID_TMPL_DATA:
		w_ErrMsg = "Invalid Template Data";
		break;
	case ERR_DUPLICATION_ID:
		w_ErrMsg = "Duplicated ID : ";
		break;
	case ERR_BAD_QUALITY:
		w_ErrMsg = "Bad Quality Image";
		break;
	case ERR_MERGE_FAIL:
		w_ErrMsg = "Merge failed";
		break;
	case ERR_NOT_AUTHORIZED:
		w_ErrMsg = "Device not authorized.";
		break;
	case ERR_MEMORY:
		w_ErrMsg = "Memory Error ";
		break;
	case ERR_INVALID_PARAM:
		w_ErrMsg = "Invalid Parameter";
		break;
	case ERR_GEN_COUNT:
		w_ErrMsg = "Generation Count is invalid";
		break;
	case ERR_INVALID_BUFFER_ID:
		w_ErrMsg = "Ram Buffer ID is invalid.";
		break;
	case ERR_INVALID_OPERATION_MODE:
		w_ErrMsg = "Invalid Operation Mode!";
		break;
	case ERR_FP_NOT_DETECTED:
		w_ErrMsg = "Finger is not detected.";
		break;
	default:
		w_ErrMsg="Fail, error code=%d";
		break;
	}

	return w_ErrMsg;
}*/
