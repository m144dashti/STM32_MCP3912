#include <Arduino.h>
#include "MCP3912.h"
#include "ST.h"

#define FIR_ORDER   10
#define ST_ADC      1

int32_t Sum = 0;
extern ADC_HandleTypeDef hadc2;
int16_t ADC_ST = 0;
int32_t ADC_MCP_R = 0;
int32_t ADC_MCP = 0;
int16_t Data[FIR_ORDER];


void setup() {
  mcp3912_init();
  Serial.begin(115200);
  Serial.println("Start");
  MX_TIM2_Init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  #ifdef FIR_ORDER
  for(uint8_t i=0 ; i<FIR_ORDER ; i++){ Data[i] = 0;}
  #endif

  #ifdef ST_ADC
  MX_ADC2_Init();
  HAL_ADCEx_Calibration_Start(&hadc2);
  #endif
}


void loop() {
  while(mcp_data_ready() < 1);
  ADC_MCP_R = mcp3912_read_reg32(MCP3912_CH3);
  if(ADC_MCP_R > 0){
    ADC_MCP = (ADC_MCP_R / 1000000) * 2;
  } else {
    ADC_MCP = 0;
  }

  #ifdef FIR_ORDER
  Data[FIR_ORDER - 1] = ADC_MCP;
  for(uint8_t j=0 ; j<FIR_ORDER ; j++){ Sum += Data[j];}
  for(uint8_t i=0 ; i<(FIR_ORDER-1) ; i++){ Data[i] = Data[i+1];}
  #endif

  #ifdef ST_ADC
  HAL_ADC_Start(&hadc2);
	HAL_ADC_PollForConversion(&hadc2, 1);
	ADC_ST = HAL_ADC_GetValue(&hadc2);
  Serial.printf("$%d %d;", (Sum/FIR_ORDER), ADC_ST);
  #else
  Serial.printf("$%d;", ADC_MCP);
  #endif
  
  delay(1);
  Sum = 0;
}


