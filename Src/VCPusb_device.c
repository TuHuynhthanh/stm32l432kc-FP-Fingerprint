
/* Includes ------------------------------------------------------------------*/

#include <VCPusb_device.h>
#include <VCPusbd_desc.h>
#include "usbd_core.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Return USBD_OK if the Battery Charging Detection mode (BCD) is used, else USBD_FAIL. */
extern USBD_StatusTypeDef USBD_LL_BatteryCharging(USBD_HandleTypeDef *pdev);

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB device Library, add supported class and start the library
  * @retval None
  */
void MX_VCP_USB_DEVICE_Init(void)
{
	/* USER CODE BEGIN USB_DEVICE_Init_PreTreatment */

	/* USER CODE END USB_DEVICE_Init_PreTreatment */

	/* Init Device Library, add supported class and start the library. */
	USBD_Init(&hUsbDeviceFS, &VCP_FS_Desc, DEVICE_FS);
	USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
	USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
	/* Verify if the Battery Charging Detection mode (BCD) is used : */
	/* If yes, the USB device is started in the HAL_PCDEx_BCD_Callback */
	/* upon reception of PCD_BCD_DISCOVERY_COMPLETED message. */
	/* If no, the USB device is started now. */
	if (USBD_LL_BatteryCharging(&hUsbDeviceFS) != USBD_OK) {
		USBD_Start(&hUsbDeviceFS);
	}
	/* USER CODE BEGIN USB_DEVICE_Init_PostTreatment */

	/* USER CODE END USB_DEVICE_Init_PostTreatment */
}



/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
