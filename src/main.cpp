#include <Arduino.h>
#include "MCP3912.h"

#define FIR_ORDER   20

uint16_t volt = 0;
uint32_t Sum = 0;
uint16_t Data[FIR_ORDER];

void setup() {
  mcp3912_init();
  Serial.begin(115200);

  for(uint8_t i=0 ; i<FIR_ORDER ; i++){
    Data[i] = 0;
  }

}


void loop() {
  while(mcp_data_ready() < 1);
  adc_ch3 = mcp3912_read_reg32(MCP3912_CH3);
  if(adc_ch3 > 0){
    volt = (adc_ch3 / 1000000) * 2;
  }

  Data[FIR_ORDER - 1] = volt;

  for(uint8_t j=0 ; j<FIR_ORDER ; j++){
    Sum += Data[j];
  }

  Serial.printf("$%d;", (Sum/FIR_ORDER));
  delayMicroseconds(500);
  
  for(uint8_t i=0 ; i<(FIR_ORDER-1) ; i++){
    Data[i] = Data[i+1];
  }
  Sum = 0;

  
}


