/*
 * FPC1020_Communication.c
 *
 *  Created on: Jan 24, 2019
 *      Author: httu
 */
#include "stm32l4xx_hal.h"
#include "FPC1020_Communication.h"
#include "string.h"

typedef bool BOOL;

BYTE m_bySrcDeviceID=0;
BYTE m_byDstDeviceID=0;

int	Run_TestConnection(void)
{
	bool	w_bRet;
	InitCmdPacket(CMD_TEST_CONNECTION, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);
	w_bRet=SendCommand(CMD_TEST_CONNECTION, m_bySrcDeviceID, m_byDstDeviceID);

		if(!w_bRet)
			return ERR_CONNECTION;

		return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int Run_GetDeviceInfo(char* p_szDevInfo)
{
	bool	w_bRet;
	WORD	w_wDevInfoLen;

	w_bRet = Run_Command_NP(CMD_GET_DEVICE_INFO);

	if (!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	w_wDevInfoLen = g_pRcmPacket->m_abyData[0]| g_pRcmPacket->m_abyData[1]<<8; //make WORD

	//RECEIVE_DATAPACKET(CMD_GET_DEVICE_INFO, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);
	w_bRet=ReceiveDataPacket(CMD_GET_DEVICE_INFO, 0, m_bySrcDeviceID, m_byDstDeviceID);
	if(w_bRet == false)
		return ERR_CONNECTION;

	if ( RESPONSE_RET != ERR_SUCCESS )
		return RESPONSE_RET;

	memcpy(p_szDevInfo, g_pRcmPacket->m_abyData, w_wDevInfoLen);

	return ERR_SUCCESS;
}

/***************************************************************************/
/***************************************************************************/
bool Run_Command_NP( WORD p_wCMD )
{
	bool w_bRet;

	//. Assemble command packet
	InitCmdPacket(p_wCMD, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	w_bRet=SendCommand(p_wCMD, m_bySrcDeviceID, m_byDstDeviceID);

	return w_bRet;
}

/************************************************************************/
/************************************************************************/
int	Run_GetImage(void)
{
	bool	w_bRet;
	w_bRet = Run_Command_NP(CMD_GET_IMAGE);
	if(!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
/* httu is still constructed.*/
 #define IMAGE_DATA_UINT	496
int	Run_UpImage(int p_nType, BYTE* p_pFpData, int* p_pnImgWidth, int* p_pnImgHeight)
{
	int		i, n, r, w, h, size;
	BOOL	w_bRet;
	BYTE	w_byData;

	w_byData = p_nType;
	InitCmdPacket(CMD_UP_IMAGE, m_bySrcDeviceID, m_byDstDeviceID, &w_byData, 1);
	SEND_COMMAND(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	w = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);
	h = MAKEWORD(g_pRcmPacket->m_abyData[2], g_pRcmPacket->m_abyData[3]);
	size = w*h;
	n = (size)/IMAGE_DATA_UINT;
	r = (size)%IMAGE_DATA_UINT;
	for (i=0; i<n; i++)
	{
		RECEIVE_DATAPACKET(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

		if(w_bRet == false)
			return ERR_CONNECTION;

		if (RESPONSE_RET != ERR_SUCCESS)
			return RESPONSE_RET;

		memcpy(&p_pFpData[i*IMAGE_DATA_UINT], &g_pRcmPacket->m_abyData[2], IMAGE_DATA_UINT);

		//if (m_hMainWnd)
		//	SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i+1)*IMAGE_DATA_UINT*100/(w*h), 1);
	}

	if (r > 0)
	{
		RECEIVE_DATAPACKET(CMD_UP_IMAGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

		if(w_bRet == false)
			return ERR_CONNECTION;

		if (RESPONSE_RET != ERR_SUCCESS)
			return RESPONSE_RET;

		memcpy(&p_pFpData[i*IMAGE_DATA_UINT], &g_pRcmPacket->m_abyData[2], r);

		//if (m_hMainWnd)
		//	SendMessage(m_hMainWnd, WM_UP_IMAGE_PROGRESS, (i*IMAGE_DATA_UINT+r)*100/(w*h), 1);
	}

	*p_pnImgWidth = w;
	*p_pnImgHeight = h;

	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	Run_Generate(int p_nRamBufferID)
{
	bool	w_bRet;
	BYTE	w_abyData[2];

	w_abyData[0] = p_nRamBufferID; //LOWBYTE
	w_abyData[1] = p_nRamBufferID>>8; //HIBYTE

	InitCmdPacket(CMD_GENERATE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);

	//. Send command packet to target
	w_bRet=SendCommand(CMD_GENERATE,  m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int	Run_DownChar(int p_nRamBufferID, BYTE* p_pbyTemplate)
{
	BOOL	w_bRet = false;
	BYTE	w_abyData[GD_RECORD_SIZE+2];
	WORD	w_wData;

	//. Assemble command packet
	w_wData = GD_RECORD_SIZE + 2;
	InitCmdPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, (BYTE*)&w_wData, 2);

	//. Send command packet to target
	SEND_COMMAND(CMD_DOWN_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(w_bRet == false)
		return ERR_CONNECTION;

	if( RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;



	//. Assemble data packet
	w_abyData[0] = (p_nRamBufferID);//LOWBYTE
	w_abyData[1] = p_nRamBufferID>>8; //highbyte
	memcpy(&w_abyData[2], p_pbyTemplate, GD_RECORD_SIZE);

	InitCmdDataPacket(CMD_DOWN_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, GD_RECORD_SIZE+2);

	//. Send data packet to target
	SEND_DATAPACKET(CMD_DOWN_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (w_bRet == false)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int	Run_LoadChar(int p_nTmplNo, int p_nRamBufferID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];

	w_abyData[0] = (p_nTmplNo);//LOBYTE
	w_abyData[1] = (p_nTmplNo)>>8;//HIBYTE
	w_abyData[2] = (p_nRamBufferID);//LOBYTE
	w_abyData[3] = (p_nRamBufferID)>>8;//HIBYTE

	InitCmdPacket(CMD_LOAD_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);

	//. Send command packet to target
	SEND_COMMAND(CMD_LOAD_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int Run_UpChar(int p_nRamBufferID, BYTE* p_pbyTemplate)
{
	BOOL w_bRet = false;
	BYTE w_abyData[2];
	int	 w_nTemplateNo = 0;
	WORD w_nCmdCks = 0, w_nSize = 0;

	//. Assemble command packet
	w_abyData[0] = (p_nRamBufferID);
	w_abyData[1] = (p_nRamBufferID)>>8;
	InitCmdPacket(CMD_UP_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);

	//. Send command packet to target
	SEND_COMMAND(CMD_UP_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	RECEIVE_DATAPACKET(CMD_UP_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	memcpy(p_pbyTemplate, &g_pRcmPacket->m_abyData[0], GD_RECORD_SIZE);

	return ERR_SUCCESS;
}
/************************************************************************/
/************************************************************************/
int	Run_StoreChar(int p_nTmplNo, int p_nRamBufferID, int* p_pnDupTmplNo)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];

	w_abyData[0] = (uint8_t)(p_nTmplNo); //LOWBYTE
	w_abyData[1] = (uint8_t)(p_nTmplNo>>8); //HIGHBYTE
	w_abyData[2] = (uint8_t)(p_nRamBufferID); //LOWBYTE
	w_abyData[3] = (uint8_t)(p_nRamBufferID>>8); //HIGHBYTE

	InitCmdPacket(CMD_STORE_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);

	//. Send command packet to target
	SEND_COMMAND(CMD_STORE_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
	{
		if (RESPONSE_RET == ERR_DUPLICATION_ID)
			*p_pnDupTmplNo = MAKEWORD(g_pRcmPacket->m_abyData[0], g_pRcmPacket->m_abyData[1]);

		return RESPONSE_RET;
	}

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int Run_DelChar(int p_nSTmplNo, int p_nETmplNo)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];

	w_abyData[0] = p_nSTmplNo; //LOWBYTE
	w_abyData[1] = p_nSTmplNo>>8; //HIGHBYTE
	w_abyData[2] = p_nETmplNo; //LOWBYTE
	w_abyData[3] = p_nETmplNo>>8; //HIGHBYTE

	//. Assemble command packet
	InitCmdPacket(CMD_DEL_CHAR, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);

	SEND_COMMAND(CMD_DEL_CHAR, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}
/************************************************************************/
/************************************************************************/
int	Run_Search(int p_nRamBufferID, int p_nStartID, int p_nSearchCount, int* p_pnTmplNo, int* p_pnLearnResult)
{
	bool	w_bRet;
	BYTE	w_abyData[6];

	w_abyData[0] = (uint8_t)p_nRamBufferID; //LOWBYTE
	w_abyData[1] = (uint8_t)(p_nRamBufferID>>8); //HIBYTE
	w_abyData[2] = (uint8_t)p_nStartID; //LOWBYTE
	w_abyData[3] = p_nStartID>>8; //HIBYTE
	w_abyData[4] = (uint8_t)p_nSearchCount; //LOWBYTE
	w_abyData[5] = p_nSearchCount>>8; //HIGHBYTE

	InitCmdPacket(CMD_SEARCH, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 6);

	//. Send command packet to target
	w_bRet=SendCommand(CMD_SEARCH , m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	if(RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	*p_pnTmplNo			= g_pRcmPacket->m_abyData[0]| g_pRcmPacket->m_abyData[1]<<8; //MAKEWORD
	*p_pnLearnResult	= g_pRcmPacket->m_abyData[2];

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int	Run_Merge(int p_nRamBufferID, int p_nMergeCount)
{
	BOOL	w_bRet;
	BYTE	w_abyData[3];

	w_abyData[0] = (uint8_t)p_nRamBufferID;
	w_abyData[1] = (uint8_t)p_nRamBufferID>>8;
	w_abyData[2] = p_nMergeCount;

	InitCmdPacket(CMD_MERGE, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 3);
	SEND_COMMAND(CMD_MERGE, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (!w_bRet)
		return ERR_CONNECTION;

	return RESPONSE_RET;
}

/************************************************************************/
/************************************************************************/
int	Run_GetStatus(int p_nTmplNo, int* p_pnStatus)
{
	bool	w_bRet;
	BYTE	w_abyData[2];

	w_abyData[0] = (uint8_t)p_nTmplNo;//LOWBYTE
	w_abyData[1] = p_nTmplNo>>8; //HIBYTE

	InitCmdPacket(CMD_GET_STATUS, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 2);

	w_bRet=SendCommand(CMD_GET_STATUS,  m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	if ( RESPONSE_RET != ERR_SUCCESS )
		return RESPONSE_RET;

	*p_pnStatus = g_pRcmPacket->m_abyData[0];

	return ERR_SUCCESS;
}

/************************************************************************/
/************************************************************************/
int	Run_GetEmptyID(int p_nSTmplNo, int p_nETmplNo, int* p_pnEmptyID)
{
	BOOL	w_bRet;
	BYTE	w_abyData[4];

	w_abyData[0] = (p_nSTmplNo);//LOBYTE
	w_abyData[1] =(p_nSTmplNo)>>8;// HIBYTE
	w_abyData[2] = (p_nETmplNo);//LOBYTE
	w_abyData[3] = (p_nETmplNo)>>8;//HIBYTE

	//. Assemble command packet
	InitCmdPacket(CMD_GET_EMPTY_ID, m_bySrcDeviceID, m_byDstDeviceID, w_abyData, 4);

	SEND_COMMAND(CMD_GET_EMPTY_ID, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(!w_bRet)
		return ERR_CONNECTION;

	if ( RESPONSE_RET != ERR_SUCCESS )
		return RESPONSE_RET;


	*p_pnEmptyID =MAKEWORD(g_pRcmPacket->m_abyData[1],g_pRcmPacket->m_abyData[0]);

	return ERR_SUCCESS;
}

/************************************************************************/
 /*
 * int *p_pnCount: contain number of enrolled FP
 * int *p_pnIDs: contain list of enrolled FP
 */
/************************************************************************/

#define VALID_FLAG_BIT_CHECK(A, V)	(A[0 + V/8] & (0x01 << (V & 0x07)))
int Run_GetEnrolledIDList(int* p_pnCount, int* p_pnIDs)
{
	int			i, w_nValidBufSize;
	BOOL		w_bRet = false;
	BYTE*		w_pValidBuf;

	InitCmdPacket(CMD_GET_ENROLLED_ID_LIST, m_bySrcDeviceID, m_byDstDeviceID, NULL, 0);

	//. Send command packet to target
	SEND_COMMAND(CMD_GET_ENROLLED_ID_LIST, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if(w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;
	w_nValidBufSize = MAKEWORD( g_pRcmPacket->m_abyData[1],g_pRcmPacket->m_abyData[0]);

	RECEIVE_DATAPACKET(CMD_GET_ENROLLED_ID_LIST, w_bRet, m_bySrcDeviceID, m_byDstDeviceID);

	if (w_bRet == false)
		return ERR_CONNECTION;

	if (RESPONSE_RET != ERR_SUCCESS)
		return RESPONSE_RET;

	BYTE  Buf[100];
	w_pValidBuf=Buf;

	memcpy(w_pValidBuf, &g_pRcmPacket->m_abyData[0], w_nValidBufSize);

	*p_pnCount = 0;
	for (i=1; i<(w_nValidBufSize*8); i++)
	{
		if (VALID_FLAG_BIT_CHECK(w_pValidBuf, i) != 0)
		{
			p_pnIDs[*p_pnCount] = i;
			*p_pnCount = *p_pnCount + 1;
		}
		int g_nMaxFpCount=500; //max capacity FP
		if (i >= g_nMaxFpCount)
			break;
	}

//	delete[] w_pValidBuf;

	return ERR_SUCCESS;
}

