/*
 * FPC1020_Command.h
 *
 *  Created on: Jan 24, 2019
 *      Author: tuhuy
 */



#ifndef FPC1020_COMMAND_H_
#define FPC1020_COMMAND_H_


#include "stdint.h"
#include "stdbool.h"
#include "Define.h"
#define COMM_TIMEOUT 1000

#define	RESPONSE_RET	(g_pRcmPacket->m_wRetCode)
/////////////////////////////	Function	/////////////////////////////////////////////
#define SEND_COMMAND(cmd, ret, nSrcDeviceID, nDstDeviceID)								\
			ret = SendCommand(cmd, nSrcDeviceID, nDstDeviceID);							\

#define RECEIVE_DATAPACKET(cmd, ret, nSrcDeviceID, nDstDeviceID)						\
		ret = ReceiveDataPacket(cmd, 0, nSrcDeviceID, nDstDeviceID);					\

#define SEND_DATAPACKET(cmd, ret, nSrcDeviceID, nDstDeviceID)								\
		ret = SendDataPacket(cmd, nSrcDeviceID, nDstDeviceID);								\


#define MAKEWORD(highByte,lowByte) lowByte|highByte<<8

typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef uint32_t DWORD;

typedef struct _ST_CMD_PACKET_
{
	WORD	m_wPrefix;
	BYTE	m_bySrcDeviceID;
	BYTE	m_byDstDeviceID;
	WORD	m_wCMDCode;
	WORD	m_wDataLen;
	BYTE	m_abyData[16];
	WORD	m_wCheckSum;
} ST_CMD_PACKET, *PST_CMD_PACKET;

typedef struct _ST_RCM_PACKET_
{
	WORD	m_wPrefix;
	BYTE	m_bySrcDeviceID;
	BYTE	m_byDstDeviceID;
	WORD	m_wCMDCode;
	WORD	m_wDataLen;
	WORD	m_wRetCode;
	BYTE	m_abyData[14];
	WORD	m_wCheckSum;
} ST_RCM_PACKET, *PST_RCM_PACKET;

typedef struct _ST_COMMAND_
{
	char	szCommandName[64];
	WORD	wCode;
} ST_COMMAND, *PST_COMMAND;

extern DWORD g_dwPacketSize;
extern BYTE	g_Packet[10*64];
extern PST_CMD_PACKET	g_pCmdPacket;
extern PST_RCM_PACKET	g_pRcmPacket;


void InitCmdPacket(uint16_t p_wCMDCode, uint8_t p_bySrcDeviceID, uint8_t p_byDstDeviceID, uint8_t* p_pbyData, uint16_t p_wDataLen);
void InitCmdDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID, BYTE* p_pbyData, WORD p_wDataLen);
bool SendCommand(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
bool ReceiveAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
bool ReceiveAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
bool CheckReceive( BYTE* p_pbyPacket, DWORD p_dwPacketLen, WORD p_wPrefix, WORD p_wCMDCode );
bool ReadDataN(BYTE* p_pData, int p_nLen, DWORD p_dwTimeOut);
bool ReceiveDataPacket(WORD	p_wCMDCode, BYTE p_byDataLen, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
bool SendDataPacket(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
bool ReceiveDataAck(WORD p_wCMDCode, BYTE p_bySrcDeviceID, BYTE p_byDstDeviceID);
//string GetErrorMsg(DWORD p_dwErrorCode);

#endif /* FPC1020_COMMAND_H_ */
