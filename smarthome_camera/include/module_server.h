#ifndef _MODULE_SERVER_H_
#define _MODULE_SERVER_H_

void printLocalTime(void);
void timeAvailable(struct timeval *t);
void initNtpTime();

void initwedServer();
void cameraserver_task(void *pvParameter);
void startcameraTask(void);

#endif