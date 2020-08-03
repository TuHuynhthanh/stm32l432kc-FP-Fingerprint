/*
 * FPC1020Application.c
 *
 *  Created on: Mar 18, 2019
 *      Author: tuhuy
 */

#include "FPC1020Application.h"
#include "FPC1020_Communication.h"
extern char st[100];

BYTE m_abyTemplate1[GD_RECORD_SIZE];

/**
 * Enroll new fingerprint
 *
 */
void appEnroll(int IDnumber){
	int	w_nRet, w_nFPNo, w_nEnrollStep = 0, w_nGenCount, w_nStatus, w_nDupFpNo;

	w_nGenCount = 3;
	w_nFPNo=IDnumber;
	w_nRet = Run_GetStatus(w_nFPNo, &w_nStatus);
	if( w_nRet == ERR_SUCCESS && w_nStatus == GD_TEMPLATE_NOT_EMPTY )
	{
		DEBUG("Template is already exist\n");

	}
	else{
		DEBUG("Input your finger please\n");
		while(w_nEnrollStep < w_nGenCount)
		{


			//. Get Image
			while (1)
			{

				w_nRet = Run_GetImage();

				if(w_nRet == ERR_SUCCESS)
					break;
				//
				//			if(w_nRet == ERR_CONNECTION)
				//			{
				//				m_strCmdResult.Format(_T("Error Connection"));
				//				goto l_exit;
				//			}
			}


			//. Create Template From Captured Image
			w_nRet = Run_Generate(w_nEnrollStep);

			if (w_nRet != ERR_SUCCESS)
			{
				if (w_nRet == ERR_BAD_QUALITY)
				{
					DEBUG("Bad Quality. Try Again!\n");
					//				m_strCmdResult = _T("Bad Quality. Try Again!");
					//				UpdateData(FALSE);
					//				DoEvents();
					continue;
				}
				else
				{
					DEBUG("Another error!");
					//				DEBUG(GetErrorMsg(w_nRet));
					//				goto l_exit;
				}
			}
			else
			{

				if(w_nEnrollStep == 0)
				{
					if (w_nGenCount == 3)
						DEBUG("Two More\n");
					else
						DEBUG("One More\n");
				}
				else if(w_nEnrollStep == 1)
					DEBUG("One More\n");
				w_nEnrollStep++;
			}
		}
	}


		if (w_nGenCount != 1)
		{
			//. Merge Template
			w_nRet = Run_Merge(0, w_nGenCount);

			if (w_nRet != ERR_SUCCESS)
			{
				DEBUG("ERROR");
//				m_strCmdResult = GetErrorMsg(w_nRet);
//				goto l_exit;
			}
		}
		//. Store template
			w_nRet = Run_StoreChar(w_nFPNo, 0, &w_nDupFpNo);

			if( w_nRet != ERR_SUCCESS )
			{
				if(w_nRet == ERR_DUPLICATION_ID)
				{
					DEBUG("Duplication ID \n");
				}
				else
				{
					DEBUG("ERROR");
//					m_strCmdResult = GetErrorMsg(w_nRet);
				}
			}
			else
			{
				DEBUG("Enroll Success\n");
			}
}
/*deleteID/////////////////////////////////////////////////////////////////
 *
 * */
void deleteID(int userID){
	int	w_nRet;

		w_nRet = Run_DelChar(userID, userID);

		if (w_nRet != ERR_SUCCESS)
		{
			//m_strCmdResult = GetErrorMsg(w_nRet);
			//UpdateData(FALSE);

			DEBUG("ERROR");
			return;
		}

		DEBUG("Delete Success\n");

}
/*delete all fingerprint that stored/////////////////////////////////////////////////////////////////
 *
 * */
void deleteAllID(){
	int	w_nRet;

			w_nRet = Run_DelChar(1, g_nMaxFpCount);

			if (w_nRet != ERR_SUCCESS)
			{
				//m_strCmdResult = GetErrorMsg(w_nRet);
				//UpdateData(FALSE);
				DEBUG("ERROR\n");
				return;
			}

			DEBUG("Delete all ID Success\n");
}
/*//
	down template from array [m_abyTemplate1]  to finger board

*/
void downTemplate(int m_nUserID)
{
	int		w_nRet, w_nDupTmplNo;

	// Download Template from application buffer to board Buffer
	w_nRet = Run_DownChar(0, m_abyTemplate1);

	if (w_nRet != ERR_SUCCESS)
	{
		//		m_strCmdResult = GetErrorMsg(w_nRet);
		//		UpdateData(FALSE);
		goto l_exit;
	}

	//. Store template with ID (m_nUserID)
	w_nRet = Run_StoreChar(m_nUserID, 0, &w_nDupTmplNo);

	if( w_nRet != ERR_SUCCESS )
	{
		if(w_nRet == ERR_DUPLICATION_ID)
		{

			sprintf(st,"Result : Fail\r\nDuplication ID = %u",w_nDupTmplNo);
			DEBUG(st);
//			m_strCmdResult.Format(_T("Result : Fail\r\nDuplication ID = %u"), w_nDupTmplNo);
		}
		else
		{
			DEBUG("ERROR");
			//m_strCmdResult = GetErrorMsg(w_nRet);
		}
	}
	else
	{
		DEBUG("Result : Success");
		//m_strCmdResult.Format(_T("Result : Success"));
	}

l_exit:

	DEBUG("\nEND DOWN TEMPLATE");
}

/*up template from finger board to array [m_abyTemplate1]
 *
 * */
void upTemplate(int m_nUserID)
{
	int		w_nRet;


	// Load Template to Buffer of board
	w_nRet = Run_LoadChar(m_nUserID, 0);

	if (w_nRet != ERR_SUCCESS)
	{
		//m_strCmdResult = GetErrorMsg(w_nRet);
		//UpdateData(FALSE);
		goto l_exit;
	}

	// Up template from buffer of board to buffer of application
	w_nRet = Run_UpChar(0, m_abyTemplate1);

	if( w_nRet != ERR_SUCCESS )
	{
		//	m_strCmdResult = GetErrorMsg(w_nRet);
		goto l_exit;
	}

	//m_strCmdResult.Format(_T("Result : Success"));
	DEBUG("Result : Success");
	//m_strFpData = ConvertByteToHex(m_abyTemplate1, GD_RECORD_SIZE, 8);


l_exit:

	DEBUG("\nEND UP TEMPLATE");
}
/*
 * identify
 * */

int fingerIdentify(){
	int w_nRet, w_nTmplNo, w_nFpStatus, w_nLearned;
	w_nRet=Run_TestConnection();
	if(w_nRet==ERR_CONNECTION){
		DEBUG("Connection error\n");
	}
	w_nRet=Run_GetStatus(w_nTmplNo, &w_nFpStatus);
	DEBUG("Input your finger\n");
	rx2Flag=false;
	while(1){
		while(1)
		{
			w_nRet=Run_GetImage();
			if(w_nRet == ERR_SUCCESS)
				break;
		}

		w_nRet = Run_Generate(0);


		w_nRet = Run_Search(0, 1, g_nMaxFpCount, &w_nTmplNo, &w_nLearned);
		if (w_nRet == ERR_SUCCESS)
		{
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_Delay(50);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			sprintf(st,"Template number=%d \n",w_nTmplNo);
			HAL_UART_Transmit(debugTerminal,(uint8_t*)st,strlen(st),1000);
			return w_nTmplNo;
		}
		else
		{
			DEBUG("Can't identify this finger\n");
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_SET);
			HAL_Delay(50);
			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3,GPIO_PIN_RESET);
			return -1;
		}
		HAL_Delay(50);
	}

}
/*get next empty id*/

int  getEmptyId()
{
	int		w_nRet, w_nEmptyID;

	w_nRet = Run_GetEmptyID(1, g_nMaxFpCount, &w_nEmptyID);

	if (w_nRet != ERR_SUCCESS)
	{
		//m_strCmdResult = GetErrorMsg(w_nRet);
		//UpdateData(FALSE);
		return -1;
	}

	//m_strCmdResult.Format(_T("Result : Success\r\nEmpty ID = %d"), w_nEmptyID);
	//UpdateData(FALSE);
	return w_nEmptyID;
}
