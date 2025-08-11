#pragma once

#include <Arduino.h>
#include <Wire.h>

//*************** copied verbatim from ES8316 reference code *****************

// State machine confirmation. Read back the value of STATEconfirm to confirm
// the IC's normal working state.
#define STATEconfirm 0x4F

#define NORMAL_I2S 0x00
#define NORMAL_LJ 0x01
#define NORMAL_DSPA 0x03
#define NORMAL_DSPB 0x23
#define Format_Len24 0x00
#define Format_Len20 0x01
#define Format_Len18 0x02
#define Format_Len16 0x03
#define Format_Len32 0x04
#define VDDA_3V3 0x00
#define VDDA_1V8 0x01

// Product master/slave mode selection: default 0 for SlaveMode, set to 1 for
// MasterMode
#define MSMode_MasterSelOn 0
// Actual Ratio = MCLK/LRCK ratio, needs to match the actual clock proportion
#define Ratio 256
#define Format NORMAL_I2S
#define Format_Len Format_Len16
// SCLK division selection: (range 1~18), SCLK = MCLK/SCLK_DIV, specific
// correspondence beyond this range is detailed in the DS
#define SCLK_DIV 4
// Default alignment method is on the falling edge, 1 for rising edge alignment,
// needs to match the actual timing
#define SCLK_INV 0

// Single-channel ADC input channel selection between CH1 (MIC1P/1N) or CH2
// (MIC2P/2N),
#define ADC_MIC1P_MIC1N 0
#define ADC_MIC2P_MIC2N 1
#define ADC_MIC1P_DF2SE 2
#define ADC_MIC2P_DF2SE 3
#define ADC_Mic ADC_MIC1P_DF2SE
#define ADC_AuxIn ADC_MIC2P_DF2SE

#define ADCChannelSel 1
// Analog voltage selection between 3V3 or 1V8, needs to match the actual
// hardware
#define VDDA_VOLTAGE VDDA_3V3
// ADC analog fixed 15dB gain: default off 0, set to 1 to turn on
#define ADC_PGA_DF2SE_15DB 1
// ADC analog gain: (range 0~10), see DS for specific correspondences
#define ADC_PGA_GAIN 0xA
// ADC digital gain: (range 0~192), 0: 0DB, -0.5dB/Step
#define ADC_Volume 0
// DAC digital gain: (range 0~192), 0: 0DB, -0.5dB/Step
#define DAC_Volume 0
// DMIC selection: default off 0, set to 1 for stereo, for mono select 2 for H,
// 3 for L
#define Dmic_Selon 0

#define I2C_ADDR 0x11

//********************************

namespace CodecES8316 {

// // helper functions to read & write to I2C
void i2c_write(uint8_t reg, uint8_t value) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

uint8_t i2c_read(uint8_t reg) {
  Wire.beginTransmission(I2C_ADDR);
  Wire.write(reg);
  size_t howmany = Wire.requestFrom(I2C_ADDR, (uint8_t)1);
  return Wire.read();
}

bool Init(int sda, int scl, int adc_input = ADC_Mic) {
  if (!Wire.begin(sda, scl, 100000)) {
    return false;
  }

  i2c_write(0x00, 0x3F);
  i2c_write(0x00, 0x00);
  i2c_write(0x0C, 0xFF);

  delay(30);

  i2c_write(0x02, 0x08);
  i2c_write(0x03, 0x20);
  i2c_write(0x04, 0x11);
  i2c_write(0x05, 0x00);
  i2c_write(0x06, 0x11);
  i2c_write(0x07, 0x00);
  i2c_write(0x08, 0x00);
  i2c_write(0x09, (MSMode_MasterSelOn << 7) + (SCLK_INV << 5) + SCLK_DIV);
  i2c_write(0x01, 0x7F);

  i2c_write(0x1C, 0x0F);
  i2c_write(0x1E, 0x90);
  i2c_write(0x1F, 0x90);
  i2c_write(0x27, ADC_Volume);
  i2c_write(0x22, (adc_input << 4));
  i2c_write(0x23, (ADC_PGA_GAIN << 4));
  i2c_write(0x24, ADC_PGA_DF2SE_15DB);
  i2c_write(0x25, 0x08 + Dmic_Selon);
  i2c_write(0x31, 0x00);
  i2c_write(0x32, 0x00);
  i2c_write(0x33, DAC_Volume);
  i2c_write(0x34, DAC_Volume);

  i2c_write(0x0A, Format + (Format_Len << 2));
  i2c_write(0x0B, Format + (Format_Len << 2));
  i2c_write(0x10, 0x12 + (0x0C * VDDA_VOLTAGE) - VDDA_VOLTAGE);
  i2c_write(0x11, 0xFC);
  i2c_write(0x12, 0x28);
  i2c_write(0x0E, 0x04);
  i2c_write(0x0F, 0x0C);
  i2c_write(0x0F, 0x00);

  i2c_write(0x2F, 0x00);
  i2c_write(0x13, 0x00);
  i2c_write(0x14, 0x88);
  i2c_write(0x15, 0x44);
  i2c_write(0x16, 0xBB);
  i2c_write(0x1A, 0x10);
  i2c_write(0x1B, 0x30);
  i2c_write(0x19, 0x00);
  i2c_write(0x18, 0x00);
  i2c_write(0x4D, 0x00);
  i2c_write(0x4E, 0x02);
  i2c_write(0x50, 0xA0);
  i2c_write(0x51, 0x00);
  i2c_write(0x52, 0x00);
  i2c_write(0x0D, 0x00);
  i2c_write(0x00, 0xC0);

  delay(30);

  i2c_write(0x17, 0x66);

  Wire.end();

  return true;
}

}; // namespace CodecES8316