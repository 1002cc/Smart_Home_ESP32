#ifndef _MODULE_BLUE_H_
#define _MODULE_BLUE_H_

#define USE_BLE 0
#if USE_BLE
void initBLE();
void startBLE();
void stopBLE();
void BLELoop();
#endif
#endif