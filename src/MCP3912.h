#include <Arduino.h>
#include <SPI.h>

uint8_t mcp3912_cs = PA4;
uint8_t mcp3912_dr = PA3;

#define MCP3912_CH0     0x00
#define MCP3912_CH1     0x01
#define MCP3912_CH2     0x02
#define MCP3912_CH3     0x03
#define MCP3912_MOD     0x08
#define MCP3912_PHASE   0x0A
#define MCP3912_GAIN    0x0B
#define MCP3912_STATCOM 0x0C
#define MCP3912_CONFIG0 0x0D
#define MCP3912_CONFIG1 0x0E
#define MCP3912_OFF0    0x0F
#define MCP3912_GC0     0x10
#define MCP3912_OFF1    0x11
#define MCP3912_GC1     0x12
#define MCP3912_OFF2    0x13
#define MCP3912_GC2     0x14
#define MCP3912_OFF3    0x15
#define MCP3912_GC3     0x16
#define MCP3912_LOCK    0x1F


uint8_t b1 = 0;
uint8_t b2 = 0;
int32_t st = 0;
uint8_t str = 0;
uint8_t treg = 0;
uint8_t readReg[3] = {0, 0, 0};
uint8_t readReg32[4] = {0, 0, 0, 0};
uint32_t adc_ch0 = 0, adc_ch1 = 0, adc_ch2 = 0, adc_ch3 = 0;
uint32_t ADC = 0;

void mcp3912_write_reg(uint8_t reg, uint32_t val)
{
  digitalWrite(mcp3912_cs, 0);
  treg = ((0b01) << 6) | (reg << 1) | 0;
  SPI.transfer(treg);
  SPI.transfer((val >> 16) & 0xFF);
  SPI.transfer((val >> 8) & 0xFF);
  SPI.transfer(val & 0xFF);
  digitalWrite(mcp3912_cs, 1);
}

uint32_t mcp3912_read_reg(uint8_t reg)
{
  digitalWrite(mcp3912_cs, 0);
  treg = ((0b01) << 6) | (reg << 1) | 1;
  SPI.transfer(treg);
  readReg[0] = SPI.transfer(0);
  readReg[1] = SPI.transfer(0);
  readReg[2] = SPI.transfer(0);
  digitalWrite(mcp3912_cs, 1);
  return (0 << 24) | (readReg[0] << 16) | (readReg[1] << 8) | readReg[2];
}

uint32_t mcp3912_read_reg32(uint8_t reg)
{
  digitalWrite(mcp3912_cs, 0);
  treg = ((0b01)<<6) | (reg<<1) | 1;
  SPI.transfer(treg);
  readReg32[0] = SPI.transfer(0);
  readReg32[1] = SPI.transfer(0);
  readReg32[2] = SPI.transfer(0);
  readReg32[3] = SPI.transfer(0);
  digitalWrite(mcp3912_cs, 1);
  return (readReg32[0] << 24) | (readReg32[1] << 16) | (readReg32[2] << 8) | readReg32[3];
}

void get_mcp_data()
{
  adc_ch0 = mcp3912_read_reg32(MCP3912_CH0);
  adc_ch1 = mcp3912_read_reg32(MCP3912_CH1);
  adc_ch2 = mcp3912_read_reg32(MCP3912_CH2);
  adc_ch3 = mcp3912_read_reg32(MCP3912_CH3);
}


void get_mcp_data_t()
{
  digitalWrite(mcp3912_cs, 0);
  uint8_t treg = ((0b01)<<6) | (MCP3912_CH0<<1) | 1;
  SPI.transfer(treg);

  b1 = SPI.transfer(0);
  b2 = SPI.transfer(0);
  adc_ch0 = (b1<<8) | b2;

  b1 = SPI.transfer(0);
  b2 = SPI.transfer(0);
  adc_ch1 = (b1<<8) | b2;

  b1 = SPI.transfer(0);
  b2 = SPI.transfer(0);
  adc_ch2 = (b1<<8) | b2;

  b1 = SPI.transfer(0);
  b2 = SPI.transfer(0);
  adc_ch3 = (b1<<8) | b2;

  digitalWrite(mcp3912_cs, 1);
  return ;  
}


uint8_t mcp_data_ready()
{
  st = mcp3912_read_reg(MCP3912_STATCOM);
  str = st & 0b01111;
  return (str == 0);
}


void mcp3912_init()
{
  SPI.begin();
  SPI.beginTransaction(SPISettings(18000000, MSBFIRST, SPI_MODE0));
  pinMode(mcp3912_cs, OUTPUT);
  pinMode(mcp3912_dr, INPUT);
  digitalWrite(mcp3912_cs, 1);

  uint32_t statcom, conf0, conf1, lock;
  statcom = (0b10111001<<16) | (0b0<<8) | 0b0; // statcom : 10111001 00000000 00000000    types group, auto inc write, dr high, dr linked, 16crc, 16 bit resolution
  conf0 = (0b00111100<<16) | (0b10100000<<8) | 0x50; //976 sps
  conf1 = 0;
  lock = 0xA5<<16;
  
  mcp3912_write_reg(MCP3912_STATCOM, statcom);
  mcp3912_write_reg(MCP3912_CONFIG0, conf0);
  mcp3912_write_reg(MCP3912_CONFIG1, conf1);
}