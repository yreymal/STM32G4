/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int ClockSetUp(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  ClockSetUp();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
int ClockSetUp(void){

 /*straightforward type int specification, using volatile to prevent SW optimization,
  like taking the value from a cash structure */
	volatile uint32_t startCounter;

	/* setting HSE oscillator */
	RCC->CR|=(1<<RCC_CR_HSEON_Pos);

	/* wait until HSE will be set */
   for(startCounter=0;;++startCounter){

	   /*we don't carry about all bits in CR register, we just need to compare with 1, bit that's HSERDY_Pos far from beginning of the register  */
	   if(RCC->CR & (1<<RCC_CR_HSERDY_Pos)){
		   break;
	   }
	   /* if it isn't set within a timeout, turn HSE off */
	   if(startCounter>0x1000){
		   RCC->CR&= ~(1<<RCC_CR_HSEON_Pos);
		   return -1;
	   }
   }

    /* turning PLL of, before tune it */
	RCC->CR&= ~ (1<<RCC_CR_PLLON_Pos);

    /*clear PLLN(multiplier) bits field  */
    RCC->PLLCFGR&= ~RCC_PLLCFGR_PLLN_Msk;
    /* set multiplier to 16 */
    RCC->PLLCFGR|= (0x016<<RCC_PLLCFGR_PLLN_Pos);

    /* clear PLLM (divider) value bits field */
    RCC->PLLCFGR&= ~RCC_PLLCFGR_PLLM_Msk;
    /* set PLLM divider to 3. The smallest value PLLM could have it's 1, so when we write 0 to this field, PLLM will be set to 1,
     * so when we want to achieve 3 value, we should add 2 to its bits field*/
    RCC->PLLCFGR|= (0x02 <<RCC_PLLCFGR_PLLM_Pos);
    /*if we use PLL for system clock it's needed to enable and use PLLR divider (minimum value is 2)     */
    RCC->PLLCFGR&= ~RCC_PLLCFGR_PLLR_Msk;
    RCC->PLLCFGR|=(0<<RCC_PLLCFGR_PLLR_Pos); /* SYSCLK = PLL/PLLR(2)*/
    /* enable PLL(R) divider */
    RCC->PLLCFGR|=(1<<RCC_PLLCFGR_PLLREN_Pos);


    /* clear PLL SRC value bits field */
    RCC->PLLCFGR&= ~RCC_PLLCFGR_PLLSRC_Msk;
    /* 11: HSE clock selected as PLL clock entry */
    RCC->PLLCFGR|= (0b11<<RCC_PLLCFGR_PLLSRC_Pos);
    /* wait PLL SRC switching */
    while((RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_Msk)!=(0b11<<RCC_PLLCFGR_PLLSRC_Pos)) {}

     /* clean flash cycles bits field */
     FLASH->ACR&= ~FLASH_ACR_LATENCY_Msk;
     /* 60 <=Fout(64MHz) <=68 * --> 3 reading cycles*/
     FLASH->ACR|= (0b0011<<FLASH_ACR_LATENCY_Pos);

     /* for stm32G4 - no prescalers is needed, while SYSCLK < 170MHz, so set prescalers to 1 */
     /* By clearing correspond bits fields, we write there 0, what means /1 prescaler */
     RCC->CFGR&= ~RCC_CFGR_PPRE1_Msk; /* APB1 prescaler */
     RCC->CFGR&= ~RCC_CFGR_PPRE2_Msk; /* APB2 prescaler */
     RCC->CFGR&= ~RCC_CFGR_HPRE_Msk;  /* AHB prescaler */

     /* turn on the PLL after configuration */
     RCC->CR|=(1<<RCC_CR_PLLON_Pos);
     for(startCounter=0;;++startCounter){
     	if(RCC->CR & (1<<RCC_CR_PLLRDY_Pos))
     		break;

     	if(startCounter>0x1000){
     		/*tuning off HSE and PLL */
     		RCC->CR&= ~(1<<RCC_CR_HSEON_Pos);
     		RCC->CR&= ~(1<<RCC_CR_PLLON_Pos);

     		break;
     		}
     	}
     /* clear system clock bits field  */
     RCC->CFGR&= ~RCC_CFGR_SW_Msk;
     /* 11: PLL selected as system clock */
     RCC->CFGR|= (0b11<<RCC_CFGR_SW_Pos);
     /* wait until SYSCLK switch to PLL*/
     while(
 		   (RCC->CFGR & RCC_CFGR_SWS_Msk) /* select only SWS bits from an entire register * ( 0bxxxx10xxx - CFGR register,
 		    							after mask we keep only _____10__ value)*/
 		   !=
 		   (0b11<<RCC_CFGR_SWS_Pos) /* compare with value 3 that we set, shifted to position where those bits are,
 		    											_____10__
 		    										!=
 		    										  2<<Pos (__)	*/
 		   ){}

     /* After we switched to HSE for PLL, we can turn off HSI for power saving */
     RCC->CR&= ~(1<<RCC_CR_HSION_Pos);
     while((RCC->CR & (1<<RCC_CR_HSIRDY_Pos)!=0)){}

     return 0;


    }




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
