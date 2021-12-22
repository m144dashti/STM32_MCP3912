#include <Arduino.h>
#include "MCP3912.h"


#define FIR_ORDER   10

uint16_t volt = 0;
uint32_t Sum = 0;
uint16_t Data[FIR_ORDER];
ADC_HandleTypeDef hadc2;
uint16_t ADC_Val = 0;

static void MX_ADC2_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  hadc2.Instance = ADC2;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK) {printf("Error");}
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {printf("Error");}
}


void setup() {
  mcp3912_init();
  Serial.begin(115200);

  for(uint8_t i=0 ; i<FIR_ORDER ; i++){
    Data[i] = 0;
  }

  MX_ADC2_Init();
  HAL_ADCEx_Calibration_Start(&hadc2);
}


void loop() {
  while(mcp_data_ready() < 1);
  ADC = mcp3912_read_reg32(MCP3912_CH3);
  if(ADC > 0){
    volt = (ADC / 1000000) * 2;
  }

  HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 1);
	ADC_Val = HAL_ADC_GetValue(&hadc2);

  Data[FIR_ORDER - 1] = ADC_Val;
  for(uint8_t j=0 ; j<FIR_ORDER ; j++){
    Sum += Data[j];
  }
  for(uint8_t i=0 ; i<(FIR_ORDER-1) ; i++){
    Data[i] = Data[i+1];
  }
  

  Serial.printf("$%d %d;", ((Sum/FIR_ORDER)+100), ADC_Val);
  delay(1);
  Sum = 0;
}


