#ifndef HEADER_FILE_NAME
#define HEADER_FILE_NAME
#define block 62
#define buttonPin 22
extern byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern byte readbackblock[18];
int readBlock(int blockNumber, byte arrayAddress[]);
#endif