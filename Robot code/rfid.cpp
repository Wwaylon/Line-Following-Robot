#include <Arduino.h>
#include <MFRC522.h>
#include <iostream>
#include "rfid.h"
#include "globals.h"


long int last_card_read;
MFRC522 rfid(SS, RST_PIN);
MFRC522::MIFARE_Key key;//create a MIFARE_Key struct named 'key', which will hold the card information

int readBlock(int blockNumber, byte arrayAddress[]) 
{
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  //byte PCD_Authenticate(byte command, byte blockAddr, MIFARE_Key *key, Uid *uid);
  //this method is used to authenticate a certain block for writing or reading
  //command: See enumerations above -> PICC_CMD_MF_AUTH_KEY_A	= 0x60 (=1100000),		// this command performs authentication with Key A
  //blockAddr is the number of the block from 0 to 15.
  //MIFARE_Key *key is a pointer to the MIFARE_Key struct defined above, this struct needs to be defined for each block. New cards have all A/B= FF FF FF FF FF FF
  //Uid *uid is a pointer to the UID struct that contains the user ID of the card.
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed (read): ");
         Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;//return "3" as error message
  }
    //it appears the authentication needs to be made before every block read/write within a specific sector.
  //If a different sector is being authenticated access to the previous one is lost.


  /*****************************************reading a block***********************************************************/
        
  byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
  status = rfid.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number
  if (status != MFRC522::STATUS_OK) {
          Serial.print("MIFARE_read() failed: ");
          Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
          return 4;//return "4" as error message
  }
  Serial.println("block was read");
}

void rfid_read()
{
  static byte readbackblock[18];
  if(millis() - last_card_read >= MINIMUM_TIME_BETWEEN_CARDS)
  {
    // Look for new cards (in case you wonder what PICC means: proximity integrated circuit card)
    if ( ! rfid.PICC_IsNewCardPresent()) {//if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
      return;
    }
    if ( ! rfid.PICC_ReadCardSerial()) {//if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
      return;
    }
    readBlock(block, readbackblock);

    Serial.print("read block: ");
    for (int j=0 ; j<16 ; j++)//print the block contents
    {
      Serial.print (readbackblock[j]);//Serial.write() transmits the ASCII numbers as human readable characters to serial monitor
    }
    //convert first 2 bytes of the card UID to a long integer
    long int tablenum = (long int)(readbackblock[0])<<8 | (long int)(readbackblock[1]) ;
    
    Serial.println("");
    //check if tablenum is in tables
    if(target_table == tablenum)
      {
        Serial.print("Table number found w/ value of: ");
        Serial.println(target_table);
        rfid.PICC_HaltA(); 
        rfid.PCD_StopCrypto1();
        command = 0;
        last_card_read = millis();
        return; 
      }else if (search_start == true && tablenum == 0)
      {
        Serial.println("returned back to start");
        command = -1;
        rfid.PICC_HaltA(); 
        rfid.PCD_StopCrypto1();
        last_card_read = millis();
        return;
      }
    
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1();
    return;
  }
}
void rfid_init()
{
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  last_card_read = millis();
  Serial.println("Tap an RFID/NFC tag on the RFID-RC522 reader");
}

int writeBlock(int blockNumber, byte arrayAddress[]) 
{
  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.
  int largestModulo4Number=blockNumber/4*4;
  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector
  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}//block number is a trailer block (modulo 4); quit and send error code 2
  Serial.print(blockNumber);
  Serial.println(" is a data block:");
  
  /*****************************************authentication of the desired block for access***********************************************************/
  byte status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(rfid.uid));
  //byte PCD_Authenticate(byte command, byte blockAddr, MIFARE_Key *key, Uid *uid);
  //this method is used to authenticate a certain block for writing or reading
  //command: See enumerations above -> PICC_CMD_MF_AUTH_KEY_A	= 0x60 (=1100000),		// this command performs authentication with Key A
  //blockAddr is the number of the block from 0 to 15.
  //MIFARE_Key *key is a pointer to the MIFARE_Key struct defined above, this struct needs to be defined for each block. New cards have all A/B= FF FF FF FF FF FF
  //Uid *uid is a pointer to the UID struct that contains the user ID of the card.
  if (status != MFRC522::STATUS_OK) {
         Serial.print("PCD_Authenticate() failed: ");
         Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
         return 3;//return "3" as error message
  }
  //it appears the authentication needs to be made before every block read/write within a specific sector.
  //If a different sector is being authenticated access to the previous one is lost.


  /*****************************************writing the block***********************************************************/
        
  status = rfid.MIFARE_Write(blockNumber, arrayAddress, 16);//valueBlockA is the block number, MIFARE_Write(block number (0-15), byte array containing 16 values, number of bytes in block (=16))
  //status = mfrc522.MIFARE_Write(9, value1Block, 16);
  if (status != MFRC522::STATUS_OK) {
           Serial.print("MIFARE_Write() failed: ");
           Serial.println(rfid.GetStatusCodeName((MFRC522::StatusCode)status));
           return 4;//return "4" as error message
  }
  Serial.println("block was written");
}


int rfid_write()
{
  static byte readbackblock[18];
  byte blockcontent[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  if(millis() - last_card_read >= MINIMUM_TIME_BETWEEN_CARDS)
  {
    // Look for new cards (in case you wonder what PICC means: proximity integrated circuit card)
    if ( ! rfid.PICC_IsNewCardPresent()) {//if PICC_IsNewCardPresent returns 1, a new card has been found and we continue
      return -3;
    }
    if ( ! rfid.PICC_ReadCardSerial()) {//if PICC_ReadCardSerial returns 1, the "uid" struct (see MFRC522.h lines 238-45)) contains the ID of the read card.
      return -3;
    }
    readBlock(block, readbackblock);
    long int tablenum = (long int)(readbackblock[0])<<8 | (long int)(readbackblock[1]) ;    
    Serial.println(tablenum);    
    if(tablenum <= tables)
    {
      Serial.println("entry found");
      rfid.PICC_HaltA(); 
      rfid.PCD_StopCrypto1();
      return -1;        //return do nothing
    }
    
    //if not found write table num into blockcontent
    //size of tables
    //tables.push_back(tables+1); // new table entry with table number
    //convert size to byte array
    Serial.println("entry not found, writing to blockcontent new table num");
    tables++;
    blockcontent[0] = (tables >> 8) & 0xFF;
    blockcontent[1] = tables & 0xFF;
    //write blockcontent to block
    Serial.print(blockcontent[0],HEX);
    Serial.print(blockcontent[1],HEX);
    Serial.println();
    writeBlock(block, blockcontent);//the blockcontent array is written into the card block  
    //tables++;
    rfid.PICC_HaltA(); 
    rfid.PCD_StopCrypto1();
    last_card_read = millis();
    return 1;
  }
}