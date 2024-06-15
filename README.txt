This project consists of two sets of code: one to be uploaded to the robot and one for the GUI

GUI:
1. Open GUI folder
2. Install pyqt5 and websocket dependencies
3. Run GUI.py 

Robot Code (note that this requires a built robot, please check project report for design)
1. Open Robot code folder
2. Open RTOS.ino with Arduino IDE
3. Install arduino libraries: EasyButton, MFRC5222 v1.4.10 (by githubCommunity), websockets(by markus sattler)
4. Set device to ESP32 Firebettle or whatever esp32 you are using though you might have different pinouts
6. Connect esp32 to your computer via usb
7. Select correct COM port in the IDE
5. Edit code to connect to your wifi using your wifi credentials (check line 16, 17 in RTOS.ino)
6. Upload code via Arduino IDE to your ESP32 with the necessary hardware connected(RFID reader, IR sensors,etc)
7. Restart ESP32