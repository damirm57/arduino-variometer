#include "vertaccel.h"

#include <Arduino.h>

#include <LightInvensense.h>
#include <EEPROM.h>

/******************/
/* data variables */
/******************/

/* calibration */
static double accelCal[3];

static boolean newData;

/* vertical acceleration */
static double va;


/*************************/
/* calibration functions */
/*************************/

/* read calibration from EEPROM */
void vertaccel_readCalibration(void) {

  /* check tag */
  uint16_t eepromTag;
  eepromTag = EEPROM.read(VERTACCEL_EPROM_ADDR);
  eepromTag <<= 8;
  eepromTag += EEPROM.read(VERTACCEL_EPROM_ADDR + 0x01);
  
  if( eepromTag != VERTACCEL_EPROM_TAG ) {
    accelCal[0] = 0.0;
    accelCal[1] = 0.0;
    accelCal[2] = 0.0;
  } else {
    /* read calibration settings */
    uint8_t* datap = (uint8_t*)accelCal;
    for( unsigned i = 0; i<sizeof(accelCal); i++ ) {
      datap[i] =  EEPROM.read(VERTACCEL_EPROM_ADDR + 0x02 + i);
    }
  }
}

/* save calibration to EEPROM */
void vertaccel_saveCalibration(double* cal) {

  /* write tag */
  uint16_t eepromTag = VERTACCEL_EPROM_TAG;
  EEPROM.write(VERTACCEL_EPROM_ADDR, (eepromTag>>8) & 0xff);
  EEPROM.write(VERTACCEL_EPROM_ADDR + 0x01, eepromTag & 0xff);

  /* save calibration settings */
  uint8_t* datap = (uint8_t*)cal;
  for( unsigned i = 0; i<3*sizeof(double); i++ ) {
    EEPROM.write(VERTACCEL_EPROM_ADDR + 0x02 + i, datap[i]);
  }

  /* save in global var */
  accelCal[0] = cal[0];
  accelCal[1] = cal[1];
  accelCal[2] = cal[2];
}

/* give calibration coefficients */
double* vertaccel_getCalibration(void) {

  return accelCal;
}  
 


/********************/
/* public functions */
/********************/

/* init vertaccel */
void vertaccel_init(void) {

  /* init */
  fastMPUInit();

  /* init calibration settings */
  vertaccel_readCalibration();
  
  /* init data variables */
  newData = false;
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*   must be run as often as possible   */
/* check if data ready and threat data  */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
boolean vertaccel_dataReady() {
  
  short iaccel[3];
  long iquat[4];
   
  /* check if we have new data from imu */
  while( fastMPUReadFIFO(NULL, iaccel, iquat) >= 0 ) {
    newData = true;
  }

  /* if new data compute vertical acceleration */
  if( newData ) {

    /***************************/
    /* normalize and calibrate */
    /***************************/
    double accel[3]; 
    double quat[4]; 
    
    accel[0] = ((double)iaccel[0])/LIGHT_INVENSENSE_ACCEL_SCALE + accelCal[0];
    accel[1] = ((double)iaccel[1])/LIGHT_INVENSENSE_ACCEL_SCALE + accelCal[1];
    accel[2] = ((double)iaccel[2])/LIGHT_INVENSENSE_ACCEL_SCALE + accelCal[2];
        
    quat[0] = ((double)iquat[0])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[1] = ((double)iquat[1])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[2] = ((double)iquat[2])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[3] = ((double)iquat[3])/LIGHT_INVENSENSE_QUAT_SCALE;
    

    /******************************/
    /* real and vert acceleration */
    /******************************/
  
    /* compute upper direction from quaternions */
    double ux, uy, uz;
    ux = 2*(quat[1]*quat[3]-quat[0]*quat[2]);
    uy = 2*(quat[2]*quat[3]+quat[0]*quat[1]);
    uz = 2*(quat[0]*quat[0]+quat[3]*quat[3])-1;
        
    /* compute real acceleration (without gravity) */
    double rax, ray, raz;
    rax = accel[0] - ux;
    ray = accel[1] - uy;
    raz = accel[2] - uz;
    
        
    /* compute vertical acceleration */
    va = (ux*rax + uy*ray + uz*raz);
  }
 
  return newData;
}


/* send raw data for debugging or calibration */ 
boolean vertaccel_rawReady(double* accel, double* upVector, double* vertAccel) {
  
  short iaccel[3];
  long iquat[4];
   
  /* check if we have new data from imu */
  while( fastMPUReadFIFO(NULL, iaccel, iquat) >= 0 ) {
    newData = true;
  }

  /* if new data compute vertical acceleration */
  if( newData ) {
    
    /*************/
    /* normalize */
    /*************/ 
    double quat[4]; 
    
    accel[0] = ((double)iaccel[0])/LIGHT_INVENSENSE_ACCEL_SCALE;
    accel[1] = ((double)iaccel[1])/LIGHT_INVENSENSE_ACCEL_SCALE;
    accel[2] = ((double)iaccel[2])/LIGHT_INVENSENSE_ACCEL_SCALE;
        
    quat[0] = ((double)iquat[0])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[1] = ((double)iquat[1])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[2] = ((double)iquat[2])/LIGHT_INVENSENSE_QUAT_SCALE;
    quat[3] = ((double)iquat[3])/LIGHT_INVENSENSE_QUAT_SCALE;
    

    /******************************/
    /* real and vert acceleration */
    /******************************/
  
    /* compute upper direction from quaternions */
    double ux, uy, uz;
    ux = 2*(quat[1]*quat[3]-quat[0]*quat[2]);
    uy = 2*(quat[2]*quat[3]+quat[0]*quat[1]);
    uz = 2*(quat[0]*quat[0]+quat[3]*quat[3])-1;
        
    /* compute real acceleration (without gravity) */
    double rax, ray, raz;
    rax = accel[0] - ux;
    ray = accel[1] - uy;
    raz = accel[2] - uz;
    
        
    /* compute vertical acceleration */
    upVector[0] = ux;
    upVector[1] = uy;
    upVector[2] = uz;
    *vertAccel = (ux*rax + uy*ray + uz*raz);
  }

  boolean result = newData;
  newData = false;
  
  return result;
}


/* must be run before reading values */
void vertaccel_updateData() {

  /* start a new acceleration computation */
  newData = false;
}


/* get vertical acceleration */
double vertaccel_getValue() {
  return va * VERTACCEL_G_TO_MS;
}
