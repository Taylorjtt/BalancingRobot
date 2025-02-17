/***************************************************************************
  This is a library for the BMP085 pressure sensor

  Designed specifically to work with the Adafruit BMP085 or BMP180 Breakout 
  ----> http://www.adafruit.com/products/391
  ----> http://www.adafruit.com/products/1603
 
  These displays use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/


#include "../rpi.h"
#include <math.h>
#include <limits.h>

#include "Adafruit_BMP085_U.h"

static bmp085_calib_data _bmp085_coeffs;   // Last read accelerometer data will be available here
static uint8_t           _bmp085Mode;

#define BMP085_USE_DATASHEET_VALS (0) /* Set to 1 for sanity check */
#define BMP085_DEBUG (0)

/**
 * We cannot use the native wiring pi _readReg16BigEndian function because
 * it calls i2c_smbus_read_word_data() under the hood, which interprets
 * the word on the bus as little endian. Since we need data in big endian
 * format, we should instead be using i2c_smbus_read_word_data_swapped().
 * See https://lkml.org/lkmk/2015/3/13/221 for more details.
 */
static int32_t _readReg16BigEndian(int fd, int reg) {

    uint8_t buff[1];
    uint8_t first;
    uint8_t second;

    i2c_read(BMP085_ADDRESS,reg,1,buff);
    first = buff[0];
    i2c_read(BMP085_ADDRESS,reg+1,1,buff);
    second = buff[0];
    return (first << 8) | second;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
static void _readCoefficients(struct bmp_t *bmp)
{
  int fd = bmp->fd;
  #if BMP085_USE_DATASHEET_VALS
    _bmp085_coeffs.ac1 = 408;
    _bmp085_coeffs.ac2 = -72;
    _bmp085_coeffs.ac3 = -14383;
    _bmp085_coeffs.ac4 = 32741;
    _bmp085_coeffs.ac5 = 32757;
    _bmp085_coeffs.ac6 = 23153;
    _bmp085_coeffs.b1  = 6190;
    _bmp085_coeffs.b2  = 4;
    _bmp085_coeffs.mb  = -32768;
    _bmp085_coeffs.mc  = -8711;
    _bmp085_coeffs.md  = 2868;
    _bmp085Mode        = 0;
  #else

    _bmp085_coeffs.ac1 = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC1);
    _bmp085_coeffs.ac2 = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC2);
    _bmp085_coeffs.ac3 = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC3);
    _bmp085_coeffs.ac4 = (uint16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC4);
    _bmp085_coeffs.ac5 = (uint16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC5);
    _bmp085_coeffs.ac6 = (uint16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_AC6);
    _bmp085_coeffs.b1 = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_B1);
    _bmp085_coeffs.b2 = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_B2);
    _bmp085_coeffs.mb = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_MB);
    _bmp085_coeffs.mc = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_MC);
    _bmp085_coeffs.md = (int16_t)_readReg16BigEndian(fd,BMP085_REGISTER_CAL_MD);
    #if BMP085_DEBUG 
        printf("%i\n",_bmp085_coeffs.ac1);
        printf("%i\n",_bmp085_coeffs.ac2);
        printf("%i\n",_bmp085_coeffs.ac3);
        printf("%i\n",_bmp085_coeffs.ac4);
        printf("%i\n",_bmp085_coeffs.ac5);
        printf("%i\n",_bmp085_coeffs.ac6);
        printf("%i\n",_bmp085_coeffs.b1);
        printf("%i\n",_bmp085_coeffs.b2);
        printf("%i\n",_bmp085_coeffs.mb);
        printf("%i\n",_bmp085_coeffs.mc);
        printf("%i\n",_bmp085_coeffs.md);
    #endif
  #endif
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
static int32_t _readRawTemperature(struct bmp_t *bmp)
{
  #if BMP085_USE_DATASHEET_VALS
    return 27898;
  #else
    i2c_write(BMP085_ADDRESS,BMP085_REGISTER_CONTROL, BMP085_REGISTER_READTEMPCMD);
    usleep(5000);
    return (uint16_t)_readReg16BigEndian(BMP085_ADDRESS,BMP085_REGISTER_TEMPDATA);
  #endif
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
static int32_t _readRawPressure(struct bmp_t *bmp)
{
  #if BMP085_USE_DATASHEET_VALS
    return 23843;
  #else
    uint8_t  p8;
    uint16_t p16;
    int32_t  p32;

    i2c_write(BMP085_ADDRESS,BMP085_REGISTER_CONTROL,BMP085_REGISTER_READPRESSURECMD + (_bmp085Mode << 6));

    switch(_bmp085Mode)
    {
      case BMP085_MODE_ULTRALOWPOWER:
        usleep(5000);
        break;
      case BMP085_MODE_STANDARD:
        usleep(8000);
        break;
      case BMP085_MODE_HIGHRES:
        usleep(14000);
        break;
      case BMP085_MODE_ULTRAHIGHRES:
      default:
        usleep(26000);
        break;
    }

    p16 = (uint16_t)_readReg16BigEndian(bmp->fd,BMP085_REGISTER_PRESSUREDATA);
    p32 = (uint32_t)p16 << 8;
    uint8_t buff[1];
    i2c_read(BMP085_ADDRESS,BMP085_REGISTER_PRESSUREDATA+2,1,buff);
    p8 = buff[0];
    p32 += p8;
    p32 >>= (8 - _bmp085Mode);
    
    return p32;
  #endif
}

/**************************************************************************/
/*!
    @brief  Compute B5 coefficient used in temperature & pressure calcs.
*/
/**************************************************************************/
int32_t _computeB5(int32_t ut){
  int32_t X1 = (ut - (int32_t)_bmp085_coeffs.ac6) * ((int32_t)_bmp085_coeffs.ac5) >> 15;
  int32_t X2 = ((int32_t)_bmp085_coeffs.mc << 11) / (X1+(int32_t)_bmp085_coeffs.md);
  return X1 + X2;
}


/***************************************************************************
 PUBLIC FUNCTIONS
 ***************************************************************************/
 
/**************************************************************************/
/*!
    @brief  Setups the HW
*/
/**************************************************************************/
bool bmp_create(struct bmp_t **bmp_ret, int32_t sensorID) 
{
  struct bmp_t *bmp = malloc (sizeof(struct bmp_t));

  /* Enable I2C */
  int fd = i2c_open(1);
  if (fd == -1) {
	  return false;
  }

  //save the i2c handle for later
  bmp->fd = fd;

  /* Mode boundary check */
  /*(if ((mode > BMP085_MODE_ULTRAHIGHRES) || (mode < 0))
  {
    mode = BMP085_MODE_ULTRAHIGHRES;
  }*/

  /* Make sure we have the right device */
  uint8_t id[1];
  i2c_read(BMP085_ADDRESS,BMP085_REGISTER_CHIPID,1,id);
  if(id[0] != 0x55)
  {
    return false;
  }

  //todo method to set mode
  /* Set the mode indicator */
  _bmp085Mode = BMP085_MODE_ULTRAHIGHRES;

  /* Coefficients need to be read once */
  _readCoefficients(bmp);
  
  *bmp_ret = bmp;  
  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the compensated pressure level in kPa
*/
/**************************************************************************/
void bmp_getPressure(struct bmp_t *bmp, float *pressure)
{
  int32_t  ut = 0, up = 0, compp = 0;
  int32_t  x1, x2, b5, b6, x3, b3, p;
  uint32_t b4, b7;

  /* Get the raw pressure and temperature values */
  ut = _readRawTemperature(bmp);
  up = _readRawPressure(bmp);

  /* Temperature compensation */
  b5 = _computeB5(ut);

  /* Pressure compensation */
  b6 = b5 - 4000;
  x1 = (_bmp085_coeffs.b2 * ((b6 * b6) >> 12)) >> 11;
  x2 = (_bmp085_coeffs.ac2 * b6) >> 11;
  x3 = x1 + x2;
  b3 = (((((int32_t) _bmp085_coeffs.ac1) * 4 + x3) << _bmp085Mode) + 2) >> 2;
  x1 = (_bmp085_coeffs.ac3 * b6) >> 13;
  x2 = (_bmp085_coeffs.b1 * ((b6 * b6) >> 12)) >> 16;
  x3 = ((x1 + x2) + 2) >> 2;
  b4 = (_bmp085_coeffs.ac4 * (uint32_t) (x3 + 32768)) >> 15;
  b7 = ((uint32_t) (up - b3) * (50000 >> _bmp085Mode));

  if (b7 < 0x80000000)
  {
    p = (b7 << 1) / b4;
  }
  else
  {
    p = (b7 / b4) << 1;
  }

  x1 = (p >> 8) * (p >> 8);
  x1 = (x1 * 3038) >> 16;
  x2 = (-7357 * p) >> 16;
  compp = p + ((x1 + x2 + 3791) >> 4);

  /* Assign compensated pressure value */
  *pressure = compp;
}

/**************************************************************************/
/*!
    @brief  Reads the temperatures in degrees Celsius
*/
/**************************************************************************/
void bmp_getTemperature(struct bmp_t *bmp, float *temp)
{
  int32_t UT, B5;     // following ds convention
  float t;

  UT = _readRawTemperature(bmp);

  #if BMP085_USE_DATASHEET_VALS
    // use datasheet numbers!
    UT = 27898;
    _bmp085_coeffs.ac6 = 23153;
    _bmp085_coeffs.ac5 = 32757;
    _bmp085_coeffs.mc = -8711;
    _bmp085_coeffs.md = 2868;
  #endif

  B5 = _computeB5(UT);
  t = (B5+8) >> 4;
  t /= 10;

  *temp = t;
}

/**************************************************************************/
/*!
    Converts temperature in celsius to temperature in fahrenheit.

    @param  degC          Degrees celsius
*/
/**************************************************************************/
float bmp_celsiusToFahrenheit(float degC)
{
  return degC * 9/5 + 32;
}

/**************************************************************************/
/*!
    Calculates the altitude (in meters) from the specified atmospheric
    pressure (in hPa), and sea-level pressure (in hPa).

    @param  seaLevel      Sea-level pressure in hPa
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float bmp_pressureToAltitude(float seaLevel, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 16):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  
  return 44330.0 * (1.0 - pow(atmospheric / seaLevel, 0.1903));
}

/**************************************************************************/
/*!
    Calculates the pressure at sea level (in hPa) from the specified altitude 
    (in meters), and atmospheric pressure (in hPa).  

    @param  altitude      Altitude in meters
    @param  atmospheric   Atmospheric pressure in hPa
*/
/**************************************************************************/
float seaLevelForAltitude(float altitude, float atmospheric)
{
  // Equation taken from BMP180 datasheet (page 17):
  //  http://www.adafruit.com/datasheets/BST-BMP180-DS000-09.pdf

  // Note that using the equation from wikipedia can give bad results
  // at high altitude.  See this thread for more information:
  //  http://forums.adafruit.com/viewtopic.php?f=22&t=58064
  
  return atmospheric / pow(1.0 - (altitude/44330.0), 5.255);
}


/**************************************************************************/
/*!
    @brief  Provides the sensor_t data for this sensor
*/
/**************************************************************************/
void bmp_getSensor(struct bmp_t *bmp, sensor_t *sensor)
{
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy (sensor->name, "BMP085", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name)- 1] = 0;
  sensor->version     = 1;
  sensor->sensor_id   = bmp->sensorID;
  sensor->type        = SENSOR_TYPE_PRESSURE;
  sensor->min_delay   = 0;
  sensor->max_value   = 1100.0F;               // 300..1100 hPa
  sensor->min_value   = 300.0F;
  sensor->resolution  = 0.01F;                // Datasheet states 0.01 hPa resolution
}

/**************************************************************************/
/*!
    @brief  Reads the sensor and returns the data as a sensors_event_t
*/
/**************************************************************************/
bool bmp_getEvent(struct bmp_t *bmp, sensors_event_t *event)
{
  float pressure_kPa;

  /* Clear the event */
  memset(event, 0, sizeof(sensors_event_t));

  event->version   = sizeof(sensors_event_t);
  event->sensor_id = bmp->sensorID;
  event->type      = SENSOR_TYPE_PRESSURE;
  event->timestamp = 0;
  bmp_getPressure(bmp, &pressure_kPa);
  event->pressure = pressure_kPa / 100.0F;
  
  return true;
}
