/*
 * FPC1020_Communication.h
 *
 *  Created on: Jan 24, 2019
 *      Author: HTTU
 */

#ifndef FPC1020_COMMUNICATION_H_
#define FPC1020_COMMUNICATION_H_
#include "FPC1020_Command.h"
#include "stdbool.h"
#include "Define.h"

int	Run_TestConnection(void); //kiem tra ket noi
int Run_GetDeviceInfo(char* p_szDevInfo);//lay thong tin board finger
bool Run_Command_NP( WORD p_wCMD );

int	Run_UpImage(int p_nType, BYTE* p_pData, int* p_pnImgWidth, int* p_pnImgHeight); //
int	Run_GetImage(void);

int	Run_Generate(int p_nRamBufferID);

int	Run_GetStatus(int p_nTmplNo, int* p_pnStatus);
int	Run_GetEmptyID(int p_nSTmplNo, int p_nETmplNo, int* p_pnEmptyID);
int Run_GetEnrolledIDList(int* p_pnCount, int* p_pnIDs);

int	Run_StoreChar(int p_nTmplNo, int p_nRamBufferID, int* p_pnDupTmplNo);
int Run_DelChar(int p_nSTmplNo, int p_nETmplNo);
int	Run_UpChar(int p_nRamBufferID, BYTE* p_pbyTemplate);
int	Run_DownChar(int p_nRamBufferID, BYTE* p_pbyTemplate);
int Run_LoadChar(int p_nTmplNo, int p_nRamBufferID);

int	Run_Merge(int p_nRamBufferID, int p_nMergeCount);
int	Run_Search(int p_nRamBufferID, int p_nStartID, int p_nSearchCount, int* p_pnTmplNo, int* p_pnLearnResult);

#endif /* FPC1020_COMMUNICATION_H_ */
