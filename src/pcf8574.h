#ifndef __PCF8574_H__
#define __PCF8574_H__ 1

i2c_drv* open_pcf8574_drv();

void close_pcf8574_drv(i2c_drv* pHandle);

#endif
