#include <LightInvensense.h>

#include <Arduino.h>

#ifdef LIGHT_INVENSENSE_BUILD
#include <inv_mpu.h>
#include <inv_mpu_dmp_motion_driver.h>
#endif

#include <InvenSense_defines.h>
#include <I2Cdev.h>

#define MAX_INIT_RESET_RETRY 20

/* original firmware parameters */
#define INV_DMP_CODE_SIZE (3062)
#define INV_DMP_START_ADDRESS (0x0400)
#define DMP_CHUNK_SIZE 16

/* how display result */
#define DMP_CODE_LINE_SIZE 16


#define DMP_FEATURE_SEND_ANY_GYRO   (DMP_FEATURE_SEND_RAW_GYRO | \
                                     DMP_FEATURE_SEND_CAL_GYRO)
#define INT_SRC_TAP             (0x01)
#define INT_SRC_ANDROID_ORIENT  (0x08)

/*----------------------------------*/
/*                                  */
/*      DMP Firmware Builder        */
/*                                  */
/*----------------------------------*/

/**************************************/
/* current custom compressed firmware */
/**************************************/

/*----------------PASTE HERE -------------------*/
const unsigned char compressedFirmware[] PROGMEM = {
   0x00, 0x01, 0x70, 0x00, 0x03, 0x24, 0x00, 0x02, 0x02, 0x00, 0x00, 0x03, 0x00, 0x02, 0x65, 0x00,
   0x00, 0x54, 0xFF, 0xEF, 0x00, 0x01, 0xFA, 0x80, 0x00, 0x00, 0x0B, 0x12, 0x82, 0x00, 0x00, 0x01,
   0x03, 0x0C, 0x30, 0xC3, 0x0E, 0x8C, 0x8C, 0xE9, 0x14, 0xD5, 0x40, 0x02, 0x13, 0x71, 0x0F, 0x8E,
   0x38, 0x83, 0xF8, 0x83, 0x30, 0x00, 0x00, 0xF8, 0x83, 0x25, 0x8E, 0xF8, 0x83, 0x30, 0x00, 0x00,
   0xF8, 0x83, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0xA9, 0xD6, 0x24, 0x00, 0x00, 0x04, 0x00, 0x00,
   0x1A, 0x82, 0x79, 0xA1, 0x00, 0x02, 0x3C, 0xFF, 0xFF, 0x00, 0x02, 0x10, 0x00, 0x01, 0x38, 0x83,
   0x6F, 0xA2, 0x00, 0x00, 0x3E, 0x03, 0x30, 0x40, 0x00, 0x02, 0x02, 0xCA, 0xE3, 0x09, 0x3E, 0x80,
   0x00, 0x01, 0x20, 0x00, 0x06, 0x40, 0x00, 0x02, 0x60, 0x00, 0x03, 0x0C, 0x00, 0x02, 0x0C, 0x18,
   0x6E, 0x00, 0x01, 0x06, 0x92, 0x0A, 0x16, 0xC0, 0xDF, 0xFF, 0xFF, 0x02, 0x56, 0xFD, 0x8C, 0xD3,
   0x77, 0xFF, 0xE1, 0xC4, 0x96, 0xE0, 0xC5, 0xBE, 0xAA, 0x00, 0x03, 0xFF, 0xFF, 0x0B, 0x2B, 0x00,
   0x01, 0x16, 0x57, 0x00, 0x01, 0x03, 0x59, 0x40, 0x00, 0x04, 0x1D, 0xFA, 0x00, 0x00, 0x02, 0x6C,
   0x1D, 0x00, 0x03, 0x3F, 0xFF, 0xDF, 0xEB, 0x00, 0x00, 0x3E, 0xB3, 0xB6, 0x00, 0x00, 0x0D, 0x22,
   0x78, 0x00, 0x01, 0x2F, 0x3C, 0x00, 0x04, 0x19, 0x42, 0xB5, 0x00, 0x01, 0x39, 0xA2, 0x00, 0x01,
   0xB3, 0x65, 0xD9, 0x0E, 0x9F, 0xC9, 0x1D, 0xCF, 0x4C, 0x34, 0x30, 0x00, 0x02, 0x50, 0x00, 0x02,
   0x3B, 0xB6, 0x7A, 0xE8, 0x00, 0x00, 0x64, 0x00, 0x02, 0xC8, 0x00, 0x05, 0x10, 0x00, 0x02, 0x10,
   0x00, 0x00, 0xFA, 0x92, 0x10, 0x00, 0x00, 0x22, 0x5E, 0x00, 0x00, 0x0D, 0x22, 0x9F, 0x00, 0x00,
   0x01, 0x00, 0x02, 0x32, 0x00, 0x01, 0xFF, 0x46, 0x00, 0x01, 0x63, 0xD4, 0x00, 0x01, 0x10, 0x00,
   0x02, 0x24, 0x00, 0x02, 0x24, 0x00, 0x02, 0x24, 0x00, 0x04, 0x10, 0x72, 0x00, 0x02, 0x40, 0x00,
   0x08, 0x06, 0x00, 0x00, 0x02, 0x00, 0x00, 0x05, 0x00, 0x00, 0x07, 0x3F, 0x00, 0x03, 0x64, 0x00,
   0x08, 0x05, 0x00, 0x00, 0x02, 0x00, 0x00, 0x08, 0x00, 0x00, 0x8E, 0xF9, 0x90, 0x00, 0x09, 0x40,
   0x00, 0x02, 0x03, 0x00, 0x03, 0x32, 0xF8, 0x98, 0x00, 0x01, 0xFF, 0x65, 0x00, 0x01, 0x83, 0x0F,
   0x00, 0x01, 0xFF, 0x9B, 0xFC, 0x00, 0x2A, 0x10, 0x00, 0x00, 0x40, 0x00, 0x05, 0x06, 0x00, 0x01,
   0xB2, 0x6A, 0x00, 0x00, 0x02, 0x00, 0x02, 0x01, 0xFB, 0x83, 0x00, 0x00, 0x68, 0x00, 0x02, 0xD9,
   0xFC, 0x00, 0x00, 0x7C, 0xF1, 0xFF, 0x83, 0x00, 0x03, 0x30, 0x00, 0x02, 0x30, 0x00, 0x01, 0x64,
   0x30, 0x00, 0x01, 0x14, 0x00, 0x02, 0x25, 0x00, 0x03, 0x16, 0xA0, 0x00, 0x03, 0x10, 0x00, 0x02,
   0x10, 0x00, 0x01, 0x2F, 0x00, 0x03, 0x01, 0xF4, 0x00, 0x01, 0x10, 0x00, 0x01, 0x28, 0x00, 0x01,
   0xFF, 0xFF, 0x45, 0x81, 0xFF, 0xFF, 0xFA, 0x72, 0x00, 0x08, 0x44, 0x00, 0x00, 0x01, 0x00, 0x00,
   0x05, 0xBA, 0xC6, 0x00, 0x00, 0x47, 0x78, 0xA2, 0x00, 0x02, 0x01, 0x00, 0x05, 0x06, 0x00, 0x03,
   0x14, 0x00, 0x01, 0x25, 0x4D, 0x00, 0x00, 0x2F, 0x70, 0x6D, 0x00, 0x01, 0x05, 0xAE, 0x00, 0x00,
   0x0C, 0x02, 0xD0, 0x00, 0x10, 0x1B, 0x00, 0x0E, 0x64, 0x00, 0x02, 0x08, 0x00, 0x5A, 0x1B, 0x00,
   0x0A, 0x0E, 0x00, 0x00, 0x0E, 0x00, 0x01, 0x0A, 0xC7, 0x00, 0x00, 0x04, 0x00, 0x04, 0x32, 0xFF,
   0xFF, 0xFF, 0x9C, 0x00, 0x01, 0x0B, 0x2B, 0x00, 0x02, 0x02, 0x00, 0x02, 0x01, 0x00, 0x02, 0x64,
   0xFF, 0xE5, 0x00, 0x10, 0x01, 0x00, 0x04, 0x01, 0x00, 0x06, 0x01, 0x80, 0x00, 0x01, 0x01, 0x80,
   0x00, 0x01, 0x01, 0x80, 0x00, 0x01, 0x24, 0x26, 0xD3, 0x00, 0x08, 0x06, 0x00, 0x00, 0x10, 0x00,
   0x00, 0x96, 0x00, 0x00, 0x3C, 0x00, 0x0F, 0x0C, 0x0A, 0x4E, 0x68, 0xCD, 0xCF, 0x77, 0x09, 0x50,
   0x16, 0x67, 0x59, 0xC6, 0x19, 0xCE, 0x82, 0x00, 0x17, 0x17, 0xD7, 0x84, 0x00, 0x00, 0x03, 0x00,
   0x0A, 0xC7, 0x93, 0x8F, 0x9D, 0x1E, 0x1B, 0x1C, 0x19, 0x00, 0x17, 0x02, 0x03, 0x18, 0x85, 0x00,
   0x01, 0x40, 0x00, 0x03, 0x03, 0x00, 0x02, 0x03, 0x00, 0x07, 0x40, 0x00, 0x32, 0x67, 0x7D, 0xDF,
   0x7E, 0x72, 0x90, 0x2E, 0x55, 0x4C, 0xF6, 0xE6, 0x88, 0x00, 0x0F, 0xD8, 0xDC, 0xB4, 0xB8, 0xB0,
   0xD8, 0xB9, 0xAB, 0xF3, 0xF8, 0xFA, 0xB3, 0xB7, 0xBB, 0x8E, 0x9E, 0xAE, 0xF1, 0x32, 0xF5, 0x1B,
   0xF1, 0xB4, 0xB8, 0xB0, 0x80, 0x97, 0xF1, 0xA9, 0xDF, 0xDF, 0xDF, 0xAA, 0xDF, 0xDF, 0xDF, 0xF2,
   0xAA, 0xC5, 0xCD, 0xC7, 0xA9, 0x0C, 0xC9, 0x2C, 0x97, 0xF1, 0xA9, 0x89, 0x26, 0x46, 0x66, 0xB2,
   0x89, 0x99, 0xA9, 0x2D, 0x55, 0x7D, 0xB0, 0xB0, 0x8A, 0xA8, 0x96, 0x36, 0x56, 0x76, 0xF1, 0xBA,
   0xA3, 0xB4, 0xB2, 0x80, 0xC0, 0xB8, 0xA8, 0x97, 0x11, 0xB2, 0x83, 0x98, 0xBA, 0xA3, 0xF0, 0x24,
   0x08, 0x44, 0x10, 0x64, 0x18, 0xB2, 0xB9, 0xB4, 0x98, 0x83, 0xF1, 0xA3, 0x29, 0x55, 0x7D, 0xBA,
   0xB5, 0xB1, 0xA3, 0x83, 0x93, 0xF0, 0x00, 0x00, 0x28, 0x50, 0xF5, 0xB2, 0xB6, 0xAA, 0x83, 0x93,
   0x28, 0x54, 0x7C, 0xF1, 0xB9, 0xA3, 0x82, 0x93, 0x61, 0xBA, 0xA2, 0xDA, 0xDE, 0xDF, 0xDB, 0x81,
   0x9A, 0xB9, 0xAE, 0xF5, 0x60, 0x68, 0x70, 0xF1, 0xDA, 0xBA, 0xA2, 0xDF, 0xD9, 0xBA, 0xA2, 0xFA,
   0xB9, 0xA3, 0x82, 0x92, 0xDB, 0x31, 0xBA, 0xA2, 0xD9, 0xBA, 0xA2, 0xF8, 0xDF, 0x85, 0xA4, 0xD0,
   0xC1, 0xBB, 0xAD, 0x83, 0xC2, 0xC5, 0xC7, 0xB8, 0xA2, 0xDF, 0xDF, 0xDF, 0xBA, 0xA0, 0xDF, 0xDF,
   0xDF, 0xD8, 0xD8, 0xF1, 0xB8, 0xAA, 0xB3, 0x8D, 0xB4, 0x98, 0x0D, 0x35, 0x5D, 0xB2, 0xB6, 0xBA,
   0xAF, 0x8C, 0x96, 0x19, 0x8F, 0x9F, 0xA7, 0x0E, 0x16, 0x1E, 0xB4, 0x9A, 0xB8, 0xAA, 0x87, 0x2C,
   0x54, 0x7C, 0xBA, 0xA4, 0xB0, 0x8A, 0xB6, 0x91, 0x32, 0x56, 0x76, 0xB2, 0x84, 0x94, 0xA4, 0xC8,
   0x08, 0xCD, 0xD8, 0xB8, 0xB4, 0xB0, 0xF1, 0x99, 0x82, 0xA8, 0x2D, 0x55, 0x7D, 0x98, 0xA8, 0x0E,
   0x16, 0x1E, 0xA2, 0x2C, 0x54, 0x7C, 0x92, 0xA4, 0xF0, 0x2C, 0x50, 0x78, 0xF1, 0x84, 0xA8, 0x98,
   0xC4, 0xCD, 0xFC, 0xD8, 0x0D, 0xDB, 0xA8, 0xFC, 0x2D, 0xF3, 0xD9, 0xBA, 0xA6, 0xF8, 0xDA, 0xBA,
   0xA6, 0xDE, 0xD8, 0xBA, 0xB2, 0xB6, 0x86, 0x96, 0xA6, 0xD0, 0xF3, 0xC8, 0x41, 0xDA, 0xA6, 0xC8,
   0xF8, 0xD8, 0xB0, 0xB4, 0xB8, 0x82, 0xA8, 0x92, 0xF5, 0x2C, 0x54, 0x88, 0x98, 0xF1, 0x35, 0xD9,
   0xF4, 0x18, 0xD8, 0xF1, 0xA2, 0xD0, 0xF8, 0xF9, 0xA8, 0x84, 0xD9, 0xC7, 0xDF, 0xF8, 0xF8, 0x83,
   0xC5, 0xDA, 0xDF, 0x69, 0xDF, 0x83, 0xC1, 0xD8, 0xF4, 0x01, 0x14, 0xF1, 0xA8, 0x82, 0x4E, 0xA8,
   0x84, 0xF3, 0x11, 0xD1, 0x82, 0xF5, 0xD9, 0x92, 0x28, 0x97, 0x88, 0xF1, 0x09, 0xF4, 0x1C, 0x1C,
   0xD8, 0x84, 0xA8, 0xF3, 0xC0, 0xF9, 0xD1, 0xD9, 0x97, 0x82, 0xF1, 0x29, 0xF4, 0x0D, 0xD8, 0xF3,
   0xF9, 0xF9, 0xD1, 0xD9, 0x82, 0xF4, 0xC2, 0x03, 0xD8, 0xDE, 0xDF, 0x1A, 0xD8, 0xF1, 0xA2, 0xFA,
   0xF9, 0xA8, 0x84, 0x98, 0xD9, 0xC7, 0xDF, 0xF8, 0xF8, 0xF8, 0x83, 0xC7, 0xDA, 0xDF, 0x69, 0xDF,
   0xF8, 0x83, 0xC3, 0xD8, 0xF4, 0x01, 0x14, 0xF1, 0x98, 0xA8, 0x82, 0x2E, 0xA8, 0x84, 0xF3, 0x11,
   0xD1, 0x82, 0xF5, 0xD9, 0x92, 0x50, 0x97, 0x88, 0xF1, 0x09, 0xF4, 0x1C, 0xD8, 0x84, 0xA8, 0xF3,
   0xC0, 0xF8, 0xF9, 0xD1, 0xD9, 0x97, 0x82, 0xF1, 0x49, 0xF4, 0x0D, 0xD8, 0xF3, 0xF9, 0xF9, 0xD1,
   0xD9, 0x82, 0xF4, 0xC4, 0x03, 0xD8, 0xDE, 0xDF, 0xD8, 0xF1, 0xAD, 0x88, 0x98, 0xCC, 0xA8, 0x09,
   0xF9, 0xD9, 0x82, 0x92, 0xA8, 0xF5, 0x7C, 0xF1, 0x88, 0x3A, 0xCF, 0x94, 0x4A, 0x6E, 0x98, 0xDB,
   0x69, 0x31, 0xDA, 0xAD, 0xF2, 0xDE, 0xF9, 0xD8, 0x87, 0x95, 0xA8, 0xF2, 0x21, 0xD1, 0xDA, 0xA5,
   0xF9, 0xF4, 0x17, 0xD9, 0xF1, 0xAE, 0x8E, 0xD0, 0xC0, 0xC3, 0xAE, 0x82, 0xC6, 0x84, 0xC3, 0xA8,
   0x85, 0x95, 0xC8, 0xA5, 0x88, 0xF2, 0xC0, 0xF1, 0xF4, 0x01, 0x0E, 0xF1, 0x8E, 0x9E, 0xA8, 0xC6,
   0x3E, 0x56, 0xF5, 0x54, 0xF1, 0x88, 0x72, 0xF4, 0x01, 0x15, 0xF1, 0x98, 0x45, 0x85, 0x6E, 0xF5,
   0x8E, 0x9E, 0x04, 0x88, 0xF1, 0x42, 0x98, 0x5A, 0x8E, 0x9E, 0x06, 0x88, 0x69, 0xF4, 0x01, 0x1C,
   0xF1, 0x98, 0x1E, 0x11, 0x08, 0xD0, 0xF5, 0x04, 0xF1, 0x1E, 0x97, 0x02, 0x02, 0x98, 0x36, 0x25,
   0xDB, 0xF9, 0xD9, 0x85, 0xA5, 0xF3, 0xC1, 0xDA, 0x85, 0xA5, 0xF3, 0xDF, 0xD8, 0x85, 0x95, 0xA8,
   0xF3, 0x09, 0xDA, 0xA5, 0xFA, 0xD8, 0x82, 0x92, 0xA8, 0xF5, 0x78, 0xF1, 0x88, 0x1A, 0x84, 0x9F,
   0x26, 0x88, 0x98, 0x21, 0xDA, 0xF4, 0x1D, 0xF3, 0xD8, 0x87, 0x9F, 0x39, 0xD1, 0xAF, 0xD9, 0xDF,
   0xDF, 0xFB, 0xF9, 0xF4, 0x0C, 0xF3, 0xD8, 0xFA, 0xD0, 0xF8, 0xDA, 0xF9, 0xF9, 0xD0, 0xDF, 0xD9,
   0xF9, 0xD8, 0xF4, 0x0B, 0xD8, 0xF3, 0x87, 0x9F, 0x39, 0xD1, 0xAF, 0xD9, 0xDF, 0xDF, 0xF4, 0x1D,
   0xF3, 0xD8, 0xFA, 0xFC, 0xA8, 0x69, 0xF9, 0xF9, 0xAF, 0xD0, 0xDA, 0xDE, 0xFA, 0xD9, 0xF8, 0x8F,
   0x9F, 0xA8, 0xF1, 0xCC, 0xF3, 0x98, 0xDB, 0x45, 0xD9, 0xAF, 0xDF, 0xD0, 0xF8, 0xD8, 0xF1, 0x8F,
   0x9F, 0xA8, 0xCA, 0xF3, 0x88, 0x09, 0xDA, 0xAF, 0x8F, 0xCB, 0xF8, 0xD8, 0xF2, 0xAD, 0x97, 0x8D,
   0x0C, 0xD9, 0xA5, 0xDF, 0xF9, 0xBA, 0xA6, 0xF3, 0xFA, 0xF4, 0x12, 0xF2, 0xD8, 0x95, 0x0D, 0xD1,
   0xD9, 0xBA, 0xA6, 0xF3, 0xFA, 0xDA, 0xA5, 0xF2, 0xC1, 0xBA, 0xA6, 0xF3, 0xDF, 0xD8, 0xF1, 0xBA,
   0xB2, 0xB6, 0x86, 0x96, 0xA6, 0xD0, 0xCA, 0xF3, 0x49, 0xDA, 0xA6, 0xCB, 0xF8, 0xD8, 0xB0, 0xB4,
   0xB8, 0xD8, 0xAD, 0x84, 0xF2, 0xC0, 0xDF, 0xF1, 0x8F, 0xCB, 0xC3, 0xA8, 0xB2, 0xB6, 0x86, 0x96,
   0xC8, 0xC1, 0xCB, 0xC3, 0xF3, 0xB0, 0xB4, 0x88, 0x98, 0xA8, 0x21, 0xDB, 0x71, 0x8D, 0x9D, 0x71,
   0x85, 0x95, 0x21, 0xD9, 0xAD, 0xF2, 0xFA, 0xD8, 0x85, 0x97, 0xA8, 0x28, 0xD9, 0xF4, 0x08, 0xD8,
   0xF2, 0x8D, 0x29, 0xDA, 0xF4, 0x05, 0xD9, 0xF2, 0x85, 0xA4, 0xC2, 0xF2, 0xD8, 0xA8, 0x8D, 0x94,
   0x01, 0xD1, 0xD9, 0xF4, 0x11, 0xF2, 0xD8, 0x87, 0x21, 0xD8, 0xF4, 0x0A, 0xD8, 0xF2, 0x84, 0x98,
   0xA8, 0xC8, 0x01, 0xD1, 0xD9, 0xF4, 0x11, 0xD8, 0xF3, 0xA4, 0xC8, 0xBB, 0xAF, 0xD0, 0xF2, 0xDE,
   0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xD8, 0xF1, 0xB8, 0xF6, 0xB5, 0xB9, 0xB0, 0x8A,
   0x95, 0xA3, 0xDE, 0x3C, 0xA3, 0xD9, 0xF8, 0xD8, 0x5C, 0xA3, 0xD9, 0xF8, 0xD8, 0x7C, 0xA3, 0xD9,
   0xF8, 0xD8, 0xF8, 0xF9, 0xD1, 0xA5, 0xD9, 0xDF, 0xDA, 0xFA, 0xD8, 0xB1, 0x85, 0x30, 0xF7, 0xD9,
   0xDE, 0xD8, 0xF8, 0x30, 0xAD, 0xDA, 0xDE, 0xD8, 0xF2, 0xB4, 0x8C, 0x99, 0xA3, 0x2D, 0x55, 0x7D,
   0xA0, 0x83, 0xDF, 0xDF, 0xDF, 0xB5, 0x91, 0xA0, 0xF6, 0x29, 0xD9, 0xFB, 0xD8, 0xA0, 0xFC, 0x29,
   0xD9, 0xFA, 0xD8, 0xA0, 0xD0, 0x51, 0xD9, 0xF8, 0xD8, 0xFC, 0x51, 0xD9, 0xF9, 0xD8, 0x79, 0xD9,
   0xFB, 0xD8, 0xA0, 0xD0, 0xFC, 0x79, 0xD9, 0xFA, 0xD8, 0xA1, 0xF9, 0xF9, 0xF9, 0xF9, 0xF9, 0xA0,
   0xDA, 0xDF, 0xDF, 0xDF, 0xD8, 0xA1, 0xF8, 0xF8, 0xF8, 0xF8, 0xF8, 0xAC, 0xDE, 0xF8, 0xAD, 0xDE,
   0x83, 0x93, 0xAC, 0x2C, 0x54, 0x7C, 0xF1, 0xA8, 0xDF, 0xDF, 0xDF, 0xF6, 0x9D, 0x2C, 0xDA, 0xA0,
   0xDF, 0xD9, 0xFA, 0xDB, 0x2D, 0xF8, 0xD8, 0xA8, 0x50, 0xDA, 0xA0, 0xD0, 0xDE, 0xD9, 0xD0, 0xF8,
   0xF8, 0xF8, 0xDB, 0x55, 0xF8, 0xD8, 0xA8, 0x78, 0xDA, 0xA0, 0xD0, 0xDF, 0xD9, 0xD0, 0xFA, 0xF8,
   0xF8, 0xF8, 0xF8, 0xDB, 0x7D, 0xF8, 0xD8, 0x9C, 0xA8, 0x8C, 0xF5, 0x30, 0xDB, 0x38, 0xD9, 0xD0,
   0xDE, 0xDF, 0xA0, 0xD0, 0xDE, 0xDF, 0xD8, 0xA8, 0x48, 0xDB, 0x58, 0xD9, 0xDF, 0xD0, 0xDE, 0xA0,
   0xDF, 0xD0, 0xDE, 0xD8, 0xA8, 0x68, 0xDB, 0x70, 0xD9, 0xDF, 0xDF, 0xA0, 0xDF, 0xDF, 0xD8, 0xF1,
   0xA8, 0x88, 0x90, 0x2C, 0x54, 0x7C, 0x98, 0xA8, 0xD0, 0x5C, 0x38, 0xD1, 0xDA, 0xF2, 0xAE, 0x8C,
   0xDF, 0xF9, 0xD8, 0xB0, 0x87, 0xA8, 0xC1, 0xC1, 0xB1, 0x88, 0xA8, 0xC6, 0xF9, 0xF9, 0xDA, 0x36,
   0xD8, 0xA8, 0xF9, 0xDA, 0x36, 0xD8, 0xA8, 0xF9, 0xDA, 0x36, 0xD8, 0xA8, 0xF9, 0xDA, 0x36, 0xD8,
   0xA8, 0xF9, 0xDA, 0x36, 0xD8, 0xF7, 0x8D, 0x9D, 0xAD, 0xF8, 0x18, 0xDA, 0xF2, 0xAE, 0xDF, 0xD8,
   0xF7, 0xAD, 0xFA, 0x30, 0xD9, 0xA4, 0xDE, 0xF9, 0xD8, 0xF2, 0xAE, 0xDE, 0xFA, 0xF9, 0x83, 0xA7,
   0xD9, 0xC3, 0xC5, 0xC7, 0xF1, 0x88, 0x9B, 0xA7, 0x7A, 0xAD, 0xF7, 0xDE, 0xDF, 0xA4, 0xF8, 0x84,
   0x94, 0x08, 0xA7, 0x97, 0xF3, 0x00, 0x00, 0xAE, 0xF2, 0x98, 0x19, 0xA4, 0x88, 0xC6, 0xA3, 0x94,
   0x88, 0xF6, 0x32, 0xDF, 0xF2, 0x83, 0x93, 0xDB, 0x09, 0xD9, 0xF2, 0xAA, 0xDF, 0xF8, 0xD8, 0xAE,
   0xF8, 0xF9, 0xD1, 0xDA, 0xF3, 0xA4, 0xDE, 0xA7, 0xF1, 0x88, 0x9B, 0x7A, 0xD8, 0xF3, 0x84, 0x94,
   0xAE, 0x19, 0xF9, 0xDA, 0xAA, 0xF1, 0xDF, 0xD8, 0xA8, 0x81, 0xC0, 0xC3, 0xC5, 0xC7, 0xA3, 0x92,
   0x83, 0xF6, 0x28, 0xAD, 0xDE, 0xD9, 0xF8, 0xD8, 0xA3, 0x50, 0xAD, 0xD9, 0xF8, 0xD8, 0xA3, 0x78,
   0xAD, 0xD9, 0xF8, 0xD8, 0xF8, 0xF9, 0xD1, 0xA1, 0xDA, 0xDE, 0xC3, 0xC5, 0xC7, 0xD8, 0xA1, 0x81,
   0x94, 0xF8, 0x18, 0xF2, 0xB0, 0x89, 0xAC, 0xC3, 0xC5, 0xC7, 0xF1, 0xD8, 0xB8, 0xB4, 0xB0, 0x97,
   0x86, 0xA8, 0x31, 0x9B, 0x06, 0x99, 0x07, 0xAB, 0x97, 0x28, 0x88, 0x9B, 0xF0, 0x0C, 0x20, 0x14,
   0x40, 0xB0, 0xB4, 0xB8, 0xF0, 0xA8, 0x8A, 0x9A, 0x28, 0x50, 0x78, 0xB7, 0x9B, 0xA8, 0x29, 0x51,
   0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31, 0xF1, 0xBB, 0xAB, 0x88, 0x00, 0x00,
   0x2C, 0x54, 0x7C, 0xF0, 0xB3, 0x8B, 0xB8, 0xA8, 0x04, 0x28, 0x50, 0x78, 0xF1, 0xB0, 0x88, 0xB4,
   0x97, 0x26, 0xA8, 0x59, 0x98, 0xBB, 0xAB, 0xB3, 0x8B, 0x02, 0x26, 0x46, 0x66, 0xB0, 0xB8, 0xF0,
   0x8A, 0x9C, 0xA8, 0x29, 0x51, 0x79, 0x8B, 0x29, 0x51, 0x79, 0x8A, 0x24, 0x70, 0x59, 0x8B, 0x20,
   0x58, 0x71, 0x8A, 0x44, 0x69, 0x38, 0x8B, 0x39, 0x40, 0x68, 0x8A, 0x64, 0x48, 0x31, 0x8B, 0x30,
   0x49, 0x60, 0x88, 0xF1, 0xAC, 0x00, 0x00, 0x2C, 0x54, 0x7C, 0xF0, 0x8C, 0xA8, 0x04, 0x28, 0x50,
   0x78, 0xF1, 0x88, 0x97, 0x26, 0xA8, 0x59, 0x98, 0xAC, 0x8C, 0x02, 0x26, 0x46, 0x66, 0xF0, 0x89,
   0x9C, 0xA8, 0x29, 0x51, 0x79, 0x24, 0x70, 0x59, 0x44, 0x69, 0x38, 0x64, 0x48, 0x31, 0xA9, 0x88,
   0x09, 0x20, 0x59, 0x70, 0xAB, 0x11, 0x38, 0x40, 0x69, 0xA8, 0x19, 0x31, 0x48, 0x60, 0x8C, 0xA8,
   0x3C, 0x41, 0x5C, 0x20, 0x7C, 0x00, 0x00, 0xF1, 0x87, 0x98, 0x19, 0x86, 0xA8, 0x6E, 0x76, 0x7E,
   0xA9, 0x99, 0x88, 0x2D, 0x55, 0x7D, 0xD8, 0xB1, 0xB5, 0xB9, 0xA3, 0xDF, 0xDF, 0xDF, 0xAE, 0xD0,
   0xDF, 0xAA, 0xD0, 0xDE, 0xF2, 0xAB, 0xF8, 0xF9, 0xD9, 0xB0, 0x87, 0xC4, 0xAA, 0xF1, 0xDF, 0xDF,
   0xBB, 0xAF, 0xDF, 0xDF, 0xB9, 0xD8, 0xB1, 0xF1, 0xA3, 0x97, 0x8E, 0x60, 0xDF, 0xB0, 0x84, 0xF2,
   0xC8, 0xF8, 0xF9, 0xD9, 0xDE, 0xD8, 0x93, 0x85, 0xF1, 0x4A, 0xB1, 0x83, 0xA3, 0x08, 0xB5, 0x83,
   0x9A, 0x08, 0x10, 0xB7, 0x9F, 0x10, 0xD8, 0xF1, 0xB0, 0xBA, 0xAE, 0xB0, 0x8A, 0xC2, 0xB2, 0xB6,
   0x8E, 0x9E, 0xF1, 0xFB, 0xD9, 0xF4, 0x1D, 0xD8, 0xF9, 0xD9, 0x0C, 0xF1, 0xD8, 0xF8, 0xF8, 0xAD,
   0x61, 0xD9, 0xAE, 0xFB, 0xD8, 0xF4, 0x0C, 0xF1, 0xD8, 0xF8, 0xF8, 0xAD, 0x19, 0xD9, 0xAE, 0xFB,
   0xDF, 0xD8, 0xF4, 0x16, 0xF1, 0xD8, 0xF8, 0xAD, 0x8D, 0x61, 0xD9, 0xF4, 0xF4, 0xAC, 0xF5, 0x9C,
   0x9C, 0x8D, 0xDF, 0x2B, 0xBA, 0xB6, 0xAE, 0xFA, 0xF8, 0xF4, 0x0B, 0xD8, 0xF1, 0xAE, 0xD0, 0xF8,
   0xAD, 0x51, 0xDA, 0xAE, 0xFA, 0xF8, 0xF1, 0xD8, 0xB9, 0xB1, 0xB6, 0xA3, 0x83, 0x9C, 0x08, 0xB9,
   0xB1, 0x83, 0x9A, 0xB5, 0xAA, 0xC0, 0xFD, 0x30, 0x83, 0xB7, 0x9F, 0x10, 0xB5, 0x8B, 0x93, 0xF2,
   0x02, 0x02, 0xD1, 0xAB, 0xDA, 0xDE, 0xD8, 0xF1, 0xB0, 0x80, 0xBA, 0xAB, 0xC0, 0xC3, 0xB2, 0x84,
   0xC1, 0xC3, 0xD8, 0xB1, 0xB9, 0xF3, 0x8B, 0xA3, 0x91, 0xB6, 0x09, 0xB4, 0xD9, 0xAB, 0xDE, 0xB0,
   0x87, 0x9C, 0xB9, 0xA3, 0xDD, 0xF1, 0xB3, 0x8B, 0x8B, 0x8B, 0x8B, 0x8B, 0xB0, 0x87, 0x20, 0x28,
   0x30, 0x38, 0xB2, 0x8B, 0xB6, 0x9B, 0xF2, 0xA3, 0xC0, 0xC8, 0xC2, 0xA3, 0xA3, 0xA3, 0xA3, 0xA3,
   0xA3, 0xF1, 0xB0, 0x87, 0xB5, 0x9A, 0x20, 0xF3, 0x9B, 0xA3, 0xA3, 0xDC, 0xBA, 0xAC, 0xDF, 0xB9,
   0xA3, 0xFE, 0xF2, 0xAB, 0xC4, 0xAA, 0xF1, 0xDF, 0xDF, 0xBB, 0xAF, 0xDF, 0xDF, 0xA3, 0xA3, 0xA3,
   0xD8, 0xD8, 0xD8, 0xBB, 0xB3, 0xB7, 0xF1, 0xAA, 0xF9, 0xDA, 0xFF, 0xD9, 0x80, 0x9A, 0xAA, 0x28,
   0xB4, 0x80, 0x98, 0xA7, 0x20, 0xB7, 0x97, 0x87, 0xA8, 0x66, 0x88, 0xF0, 0x79, 0x51, 0xF1, 0x90,
   0x2C, 0x87, 0x0C, 0xA7, 0x81, 0x97, 0x62, 0x93, 0xF0, 0x71, 0x71, 0x60, 0x85, 0x94, 0x01, 0x29,
   0x51, 0x79, 0x90, 0xA5, 0xF1, 0x28, 0x4C, 0x6C, 0x87, 0x0C, 0x95, 0x18, 0x85, 0x78, 0xA3, 0x83,
   0x90, 0x28, 0x4C, 0x6C, 0x88, 0x6C, 0xD8, 0xF3, 0xA2, 0x82, 0x00, 0x00, 0xF2, 0x10, 0xA8, 0x92,
   0x19, 0x80, 0xA2, 0xF2, 0xD9, 0x26, 0xD8, 0xF1, 0x88, 0xA8, 0x4D, 0xD9, 0x48, 0xD8, 0x96, 0xA8,
   0x39, 0x80, 0xD9, 0x3C, 0xD8, 0x95, 0x80, 0xA8, 0x39, 0xA6, 0x86, 0x98, 0xD9, 0x2C, 0xDA, 0x87,
   0xA7, 0x2C, 0xD8, 0xA8, 0x89, 0x95, 0x19, 0xA9, 0x80, 0xD9, 0x38, 0xD8, 0xA8, 0x89, 0x39, 0xA9,
   0x80, 0xDA, 0x3C, 0xD8, 0xA8, 0x2E, 0xA8, 0x39, 0x90, 0xD9, 0x0C, 0xD8, 0xA8, 0x95, 0x31, 0x98,
   0xD9, 0x0C, 0xD8, 0xA8, 0x09, 0xD9, 0xFF, 0xD8, 0x01, 0xDA, 0xFF, 0xD8, 0x95, 0x39, 0xA9, 0xDA,
   0x26, 0xFF, 0xD8, 0x90, 0xA8, 0x0D, 0x89, 0x99, 0xA8, 0x10, 0x80, 0x98, 0x21, 0xDA, 0x2E, 0xD8,
   0x89, 0x99, 0xA8, 0x31, 0x80, 0xDA, 0x2E, 0xD8, 0xA8, 0x86, 0x96, 0x31, 0x80, 0xDA, 0x2E, 0xD8,
   0xA8, 0x87, 0x31, 0x80, 0xDA, 0x2E, 0xD8, 0xA8, 0x82, 0x92, 0xF3, 0x41, 0x80, 0xF1, 0xD9, 0x2E,
   0xD8, 0xA8, 0x82, 0xF3, 0x19, 0x80, 0xF1, 0xD9, 0x2E, 0xD8, 0x82, 0xAC, 0xF3, 0xC0, 0xA2, 0x80,
   0x22, 0xF1, 0xA6, 0x2E, 0xA7, 0x2E, 0xA9, 0x22, 0x98, 0xA8, 0x29, 0xDA, 0xAC, 0xDE, 0xFF, 0xD8,
   0xA2, 0xF2, 0x2A, 0xF1, 0xA9, 0x2E, 0x82, 0x92, 0xA8, 0xF2, 0x31, 0x80, 0xA6, 0x96, 0xF1, 0xD9,
   0x00, 0x00, 0xAC, 0x8C, 0x9C, 0x0C, 0x30, 0xAC, 0xDE, 0xD0, 0xDE, 0xFF, 0xD8, 0x8C, 0x9C, 0xAC,
   0xD0, 0x10, 0xAC, 0xDE, 0x80, 0x92, 0xA2, 0xF2, 0x4C, 0x82, 0xA8, 0xF1, 0xCA, 0xF2, 0x35, 0xF1,
   0x96, 0x88, 0xA6, 0xD9, 0x00, 0x00, 0xD8, 0xF1, 0xFF };
#define COMPRESSED_DMP_CODE_SIZE 2665
#define COMPRESSED_DMP_LPF_CFG (0x2)
#define COMPRESSED_DMP_RATE_DIV_CFG (0x4)
#define COMPRESSED_DMP_PAQUET_RAW_ACCEL
#define COMPRESSED_DMP_PAQUET_QUAT
#define COMPRESSED_DMP_PAQUET_GESTURE
#define COMPRESSED_DMP_PAQUET_TAP
#define COMPRESSED_DMP_PAQUET_LENGTH 26
/*----------------END HERE -------------------*/

static unsigned char dmpChunk[DMP_CHUNK_SIZE];

#ifdef LIGHT_INVENSENSE_BUILD
/************************/
/* firmware compression */
/************************/

  
/* print a c++ formated buffer */ 
static unsigned hexValueCount = 0;

void print_hex_value(uint8_t value) {

  /* separator */ 
  if(hexValueCount % DMP_CODE_LINE_SIZE != 0) {
    Serial.print(", ");
  }
  
  /* print value */
  Serial.print("0x");
  if( value < 0x10 ) {
    Serial.print("0");
  }
  Serial.print(value, HEX);
  hexValueCount++;
  
  /* new line ? */
  if( hexValueCount % DMP_CODE_LINE_SIZE == 0 ) {
    Serial.print(",\n   ");
  }
}

/* read dmp memory byte per byte */
static unsigned short memPos = (unsigned short)(-1);
static unsigned short chunkPos = 0;

unsigned char readDmpMemory(void) {

  memPos++;

  /* need to load a new chunk ? */
  if( memPos % DMP_CHUNK_SIZE == 0) {
    mpu_read_mem(memPos, DMP_CHUNK_SIZE, dmpChunk);
    chunkPos = 0;
  }

  /* get the code char */
  unsigned char c = dmpChunk[chunkPos];
  chunkPos++;
  
  return c;
}


/* compress dmp memory */
int createCompressedFirmware(void) {
  
  /*****************/
  /* init settings */
  /*****************/
  
  /* setting imu */
  if( mpu_init(NULL) < 0 )
    return -1;
  
  if( mpu_set_sensors(INV_XYZ_GYRO|INV_XYZ_ACCEL) < 0 )
    return -1;
  
  /* set fsrs */
  unsigned short fsr;
  
  switch( LIGHT_INVENSENSE_GYRO_FSR ) {
  case  INV_FSR_250DPS: fsr = 250; break;
  case  INV_FSR_500DPS: fsr = 500; break;
  case  INV_FSR_1000DPS: fsr = 1000; break;
  case  INV_FSR_2000DPS: fsr = 2000; break;
  default: fsr = 0;
  }
  if( mpu_set_gyro_fsr(fsr) < 0 )
    return -1;

  switch( LIGHT_INVENSENSE_ACCEL_FSR ) {
  case INV_FSR_2G: fsr = 2; break;
  case INV_FSR_4G: fsr = 4; break;
  case INV_FSR_8G: fsr = 8; break;
  case INV_FSR_16G: fsr = 16; break;
  default: fsr = 0;
  }
  if( mpu_set_accel_fsr(fsr) < 0 )
    return -1;

  /* setting dmp */
  if( dmp_load_motion_driver_firmware() < 0 )
    return -1;
  if( dmp_set_fifo_rate(LIGHT_INVENSENSE_DMP_SAMPLE_RATE) < 0 )
    return -1;
  if( dmp_enable_feature(LIGHT_INVENSENSE_DMP_FEATURES) < 0 )
    return -1;

  /* setting tap if enabled */
  if( LIGHT_INVENSENSE_DMP_FEATURES & DMP_FEATURE_TAP ) {
    if(dmp_set_tap_axes(LIGHT_INVENSENSE_TAP_AXES) < 0)
      return -1;
    if(dmp_set_tap_thresh(LIGHT_INVENSENSE_TAP_AXES, LIGHT_INVENSENSE_TAP_THRESH) < 0)
      return -1;
    if(dmp_set_tap_count(LIGHT_INVENSENSE_TAP_COUNT) < 0)
      return -1;
    if(dmp_set_tap_time(LIGHT_INVENSENSE_TAP_TIME) < 0)
      return -1;
    if( dmp_set_tap_time_multi(LIGHT_INVENSENSE_TAP_TIME_MULTI) < 0)
      return -1;
  }
  
  /*********************/
  /* Compress firmware */
  /*********************/
  
  unsigned readPos = 0;
  bool zeroFound = false;
  unsigned zeroCount;

  Serial.print("const unsigned char compressedFirmware[] PROGMEM = {\n   ");
  
  /* read all the code */
  while( readPos < INV_DMP_CODE_SIZE ) {

    /* get current char */
    uint8_t ic = readDmpMemory();

    /* not counting zero */
    if( ! zeroFound ) {

      /* start counting zero ? */
      if( ic == 0x00 ) {
	zeroFound = true;
	zeroCount = 0;
      }

      print_hex_value(ic);
    }

    /* counting zero */
    else {

      /* zero again ? */
      if( ic == 0x00 ) {
	zeroCount++;
      }

      /* else print zero count */
      else {
	print_hex_value(zeroCount);
	print_hex_value(ic);
	zeroFound = false;
      }
    }

    /* next */
    readPos++;
  }

  Serial.print(" };\n");
  Serial.print("#define COMPRESSED_DMP_CODE_SIZE ");
  Serial.print(hexValueCount, DEC);
  Serial.print("\n");

  /******************/
  /* get dmp config */
  /******************/
  unsigned char data;

  /* start dmp */
  mpu_set_dmp_state(1);

  /* read LPF config (depend on sample rate) */
  I2Cdev::readBytes(INV_HW_ADDR, INV_REG_LPF, 1, &data);
  Serial.print("#define COMPRESSED_DMP_LPF_CFG (0x");
  Serial.print(data, HEX);
  Serial.print(")\n");

  /* read sample rate (depend on DMP firmware) */
  I2Cdev::readBytes(INV_HW_ADDR, INV_REG_RATE_DIV, 1, &data);
  Serial.print("#define COMPRESSED_DMP_RATE_DIV_CFG (0x");
  Serial.print(data, HEX);
  Serial.print(")\n");

  /******************/
  /* paquet length  */
  /******************/
  unsigned packet_length = 0;
  if (LIGHT_INVENSENSE_DMP_FEATURES & DMP_FEATURE_SEND_RAW_ACCEL) {
    Serial.print("#define COMPRESSED_DMP_PAQUET_RAW_ACCEL\n");
    packet_length += 6;
  }
  
  if (LIGHT_INVENSENSE_DMP_FEATURES & DMP_FEATURE_SEND_ANY_GYRO) {
    Serial.print("#define COMPRESSED_DMP_PAQUET_GYRO\n");
    packet_length += 6;
  }
      
  if (LIGHT_INVENSENSE_DMP_FEATURES & (DMP_FEATURE_LP_QUAT | DMP_FEATURE_6X_LP_QUAT)) {
    Serial.print("#define COMPRESSED_DMP_PAQUET_QUAT\n");
    packet_length += 16;
  }
  
  if (LIGHT_INVENSENSE_DMP_FEATURES & (DMP_FEATURE_TAP | DMP_FEATURE_ANDROID_ORIENT)) {
    Serial.print("#define COMPRESSED_DMP_PAQUET_GESTURE\n");
    packet_length += 4;
    if(LIGHT_INVENSENSE_DMP_FEATURES & DMP_FEATURE_TAP) {
      Serial.print("#define COMPRESSED_DMP_PAQUET_TAP\n");
    }
    if(LIGHT_INVENSENSE_DMP_FEATURES & DMP_FEATURE_ANDROID_ORIENT) {
      Serial.print("#define COMPRESSED_DMP_PAQUET_ANDROID_ORIENT\n");
    }
  }

  Serial.print("#define COMPRESSED_DMP_PAQUET_LENGTH ");
  Serial.print(packet_length, DEC);
  Serial.print("\n");

  return 0;
  
}

#endif //LIGHT_INVENSENSE_BUILD

/**************************/
/* Firmware decompression */
/**************************/

static unsigned decReadPos = 0;
static unsigned decZeroCount = 0;
 
/* return 1 byte */
uint8_t decodeCompressedFirmware(void) {

  /* already finish ? */
  if( decReadPos >= COMPRESSED_DMP_CODE_SIZE) {
    return 0x00;  //return 0x00 even if zeroCount != 0
  }

  /********************/
  /* output next char */
  /********************/
  
  /* output remaining zeros */
  if( decZeroCount != 0 ) {
    decZeroCount--;
    return 0x00;
  }

  /* else decompress */
  uint8_t c; 
  c = pgm_read_byte(compressedFirmware + decReadPos);
  decReadPos++;

  /* check for zero */
  if( c != 0x00 ) {
    return c;
  } else {
    decZeroCount = pgm_read_byte(compressedFirmware + decReadPos);
    decReadPos++;
  }
  
  return 0x00;
}
  
int fastMpuWriteMem(unsigned short memAddr, unsigned char *data);

int loadCompressedFirmware(void) {
  
  unsigned short writePos = 0;
  while( writePos < INV_DMP_CODE_SIZE ) {

    /* decompress chunk */
    for(int i = 0; i<DMP_CHUNK_SIZE; i++) {
      dmpChunk[i] = decodeCompressedFirmware();
    }

    /* write chunk */
    if( fastMpuWriteMem(writePos, dmpChunk) < 0 ) 
      return -1;

    /* next */
    writePos += DMP_CHUNK_SIZE;
  }

  return 0;
}
  


/*----------------------------------*/
/*                                  */
/*      Light MPU library           */
/*                                  */
/*----------------------------------*/

int fastMpuWriteMem(unsigned short memAddr, unsigned char *data) {

  unsigned char com[2];

  /* set bank */
  com[0] = (unsigned char)(memAddr >> 8);
  com[1] = (unsigned char)(memAddr & 0xFF);
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_BANK_SEL, 2, com);

  /* write data */
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_MEM_R_W, DMP_CHUNK_SIZE, data);
  
  return 0;
}


int fastFIFOReset(void) {
  
  unsigned char data;

  /* clear */
  data = 0;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_INT_ENABLE, 1, &data);
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_FIFO_EN, 1, &data);
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_USER_CTRL, 1, &data);
  
  /* reset DMP FIFO */
  data = BIT_FIFO_RST | BIT_DMP_RST;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_USER_CTRL, 1, &data);
  delay(50);

  /* enable DMP FIFO */
  data = BIT_DMP_EN | BIT_FIFO_EN;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_USER_CTRL, 1, &data);
  
  /* reset STD FIFO */
  data = 0;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_INT_ENABLE, 1, &data);
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_FIFO_EN, 1, &data);

  return 0;
}


int fastMPUInit(void) {

  unsigned char data[2];

  /*****************/
  /* Reset device  */
  /*****************/
  bool resettingError = true;
  unsigned resetCountStep = 0;

  while( resettingError && resetCountStep < MAX_INIT_RESET_RETRY ) {

    resettingError = false;

    /* send reset command */
    data[0] = BIT_RESET;
    if( !I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_PWR_MGMT_1, 1, data) ) {
      resettingError = true;
    }      
    delay(100);

    /* read reset result */
    if( !resettingError ) {
      if( I2Cdev::readBytes(INV_HW_ADDR, INV_REG_PWR_MGMT_1, 1, data) != 1 )
	resettingError = true;
    }

    /* check reset result */
    if( !resettingError ) {
      if( data[0] & BIT_RESET )
	resettingError = true;
    }

    /* next try if needed */
    resetCountStep++;
  }

  /* check reset */
  if( resettingError ) {
    return -1;
  }

  /********************/
  /* start the device */
  /********************/
  
  /* Wake up chip. */
  data[0] = 0x00;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_PWR_MGMT_1, 1, data);

#ifdef MPU6500
  /* MPU6500 shares 4kB of memory between the DMP and the FIFO. Since the
   * first 3kB are needed by the DMP, we'll use the last 1kB for the FIFO.
   */
  data[0] = BIT_FIFO_SIZE_1024;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_ACCEL_CFG2, 1, data);
#endif

  /****************/
  /* set settings */
  /****************/

  /* set gyro fsr */
  data[0] = LIGHT_INVENSENSE_GYRO_FSR << 3;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_GYRO_CFG, 1, data);

  /* set accel fsr */
  data[0] = LIGHT_INVENSENSE_ACCEL_FSR << 3;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_ACCEL_CFG, 1, data);

  /* set low pass filter */
  data[0] = COMPRESSED_DMP_LPF_CFG;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_LPF, 1, data);

#ifdef MPU6500 //MPU6500 accel/gyro dlpf separately
  data[0] = BIT_FIFO_SIZE_1024 | data[0];
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_ACCEL_CFG2, 1, data);
#endif

  /********************/
  /* power on sensors */
  /********************/

  /* power on gyro and accel */
  data[0] = 0; //all power on
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_PWR_MGMT_2, 1, data);
  delay(50);
  
  /****************/
  /* set firmware */
  /****************/

  /* load */
  loadCompressedFirmware();

  /* set start address */
  data[0] = INV_DMP_START_ADDRESS >> 8;
  data[1] = INV_DMP_START_ADDRESS & 0xFF;
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_PRGM_START_H, 2, data);

  
  /**************/
  /* enable dmp */
  /**************/

  /* set sample rate */
  data[0] = COMPRESSED_DMP_RATE_DIV_CFG; 
  I2Cdev::writeBytes(INV_HW_ADDR, INV_REG_RATE_DIV, 1, data);

  /**************/
  /* reset fifo */
  /**************/
  fastFIFOReset();
 
  
  return 0;
}



/* tap call back */
void (*tapCallBack)(unsigned char, unsigned char) = NULL;

void fastMPUSetTapCallback(void (*callback)(unsigned char, unsigned char)) {

  tapCallBack = callback;
}



int fastMPUReadFIFO(short *gyro, short *accel, long *quat) {

  unsigned char data[2];

  /*******************/
  /* read one paquet */
  /*******************/
  unsigned char dmpPaquet[COMPRESSED_DMP_PAQUET_LENGTH];
  
  /* read fifo count for one paquet */
  unsigned short fifoCount;

  I2Cdev::readBytes(INV_HW_ADDR, INV_REG_FIFO_COUNT_H, 2, data);
  fifoCount = (data[0] << 8) | data[1];

  if( fifoCount < COMPRESSED_DMP_PAQUET_LENGTH )
    return -1;

  /* if fifo is at least 50%, check overflow */
  if( fifoCount > (INV_HW_MAX_FIFO >> 1) ) {
    I2Cdev::readBytes(INV_HW_ADDR, INV_REG_INT_STATUS, 1, data);
    if( data[0] & BIT_FIFO_OVERFLOW ) {
      fastFIFOReset();
      return -1;
    }
  }

  /* read paquet */
  I2Cdev::readBytes(INV_HW_ADDR, INV_REG_FIFO_R_W, COMPRESSED_DMP_PAQUET_LENGTH, dmpPaquet);

  /********************/
  /* parse DMP paquet */
  /********************/
  unsigned char* dmpPaquetP = dmpPaquet;
  
#ifdef COMPRESSED_DMP_PAQUET_QUAT
  for(int i = 0; i<4; i++ ) {
    quat[i] = 0;
    for(int j = 0; j<4; j++) {
      quat[i] <<= 8;
      quat[i] += (long)(*dmpPaquetP);
      dmpPaquetP++;
    }
  }

  /* 
  quat[0] = ((long)fifo_data[0] << 24) | ((long)fifo_data[1] << 16) | ((long)fifo_data[2] << 8) | fifo_data[3];
  quat[1] = ((long)fifo_data[4] << 24) | ((long)fifo_data[5] << 16) | ((long)fifo_data[6] << 8) | fifo_data[7];
  quat[2] = ((long)fifo_data[8] << 24) | ((long)fifo_data[9] << 16) | ((long)fifo_data[10] << 8) | fifo_data[11];
  quat[3] = ((long)fifo_data[12] << 24) | ((long)fifo_data[13] << 16) | ((long)fifo_data[14] << 8) | fifo_data[15];
  */
#endif

#ifdef COMPRESSED_DMP_PAQUET_RAW_ACCEL
  for(int i = 0; i<3; i++ ) {
    accel[i] = ((short)(dmpPaquetP[0]) << 8) | (short)(dmpPaquetP[1]);
    dmpPaquetP += 2;
  }

  /*
  accel[0] = ((short)fifo_data[ii+0] << 8) | fifo_data[ii+1];
  accel[1] = ((short)fifo_data[ii+2] << 8) | fifo_data[ii+3];
  accel[2] = ((short)fifo_data[ii+4] << 8) | fifo_data[ii+5];  
  */
#endif

#ifdef COMPRESSED_DMP_PAQUET_GYRO
  for(int i = 0; i<3; i++ ) {
    gyro[i] = ((short)(dmpPaquetP[0]) << 8) | (short)(dmpPaquetP[1]);
    dmpPaquetP += 2;
  }

  /*
  gyro[0] = ((short)fifo_data[ii+0] << 8) | fifo_data[ii+1];
  gyro[1] = ((short)fifo_data[ii+2] << 8) | fifo_data[ii+3];
  gyro[2] = ((short)fifo_data[ii+4] << 8) | fifo_data[ii+5];
  */
#endif


#ifdef COMPRESSED_DMP_PAQUET_GESTURE
#ifdef COMPRESSED_DMP_PAQUET_TAP
  if( (dmpPaquetP[1] & INT_SRC_TAP) && tapCallBack ) {
    unsigned char tap = 0x3F & dmpPaquetP[3];
    unsigned char direction = tap >> 3;
    unsigned char count = (tap % 8) + 1;
    tapCallBack(direction, count);
  }
#endif //COMPRESSED_DMP_PAQUET_TAP
#endif //COMPRESSED_DMP_PAQUET_GESTURE

  return 0;
}


  
  

  



