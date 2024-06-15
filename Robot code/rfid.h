#ifndef RFID_H
#define RFID_H
#define block 62
#define RST_PIN 27
#define MINIMUM_TIME_BETWEEN_CARDS  1000

int readBlock(int blockNumber, byte arrayAddress[]);
int writeBlock(int blockNumber, byte arrayAddress[]);
void rfid_read();
void rfid_init();
int rfid_write();
#endif