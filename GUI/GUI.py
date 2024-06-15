from PyQt5.QtWidgets import *
from PyQt5 import uic
import pyobject, websocket
from PyQt5.QtCore import QThread, QObject, pyqtSignal, QTimer
import asyncio
from concurrent.futures import TimeoutError as ConnectionTimeoutError
from time import sleep
import select

ws = websocket.WebSocket()
flag = False
class tables():
    def __init__(self):
        self.tables = 0
        self.target = 0

class Worker(QObject):
    finished = pyqtSignal()
    done = pyqtSignal(bool)
    def run(self):
        global ws
        global flag
        while True:
            sleep(1)
            if (flag):
                ready = select.select([ws], [], [], 5)
                if ready[0]:
                    data = ws.recv()
                    if(data == "done"):
                        break
        self.finished.emit()

class MyGui(QMainWindow):
    def __init__(self):
        super(MyGui, self).__init__()
        uic.loadUi('mygui.ui', self)
        self.show()
        self.t1 = tables()
        self.total_tables = 0
        with open("data.txt", "r") as f:
            self.total_tables = int(f.readline())
        #close
        f.close()
        self.update_tables()
        self.deploy.setEnabled(False)

        self.connect.clicked.connect(self.on_connect)
        self.addtable.clicked.connect(self.on_addtable)
        self.actionquit.triggered.connect(self.close)
        self.deploy.clicked.connect(self.on_deploy)  
        self.resettable.clicked.connect(self.on_resettable) 
    def check_connection(self):
        try:
            ws.send("sync")
            result = ws.recv()
            if(result.isdigit() & (int(result) != self.total_tables)):
                self.total_tables = int(result)
                with open("data.txt", "w") as f:
                    f.write(str(self.total_tables))
                    f.close()
                self.update_tables()
        except ConnectionTimeoutError:
            self.t1.connection = False
            self.connectionlabel.setText("Status: Disconnected")
            self.deploy.setEnabled(False)
            self.select.setEnabled(False)
            self.addtable.setEnabled(False)
            self.resettable.setEnabled(False)
            self.connect.setEnabled(True)
            self.timer.stop()
        except ConnectionResetError:
            self.t1.connection = False
            self.connectionlabel.setText("Status: Disconnected")
            self.deploy.setEnabled(False)
            self.select.setEnabled(False)
            self.addtable.setEnabled(False)
            self.resettable.setEnabled(False)
            self.connect.setEnabled(True)
            self.timer.stop()
                
    def update_tables(self):
        self.select.clear()
        for i in range(1, self.total_tables+1):
            self.select.addItem(str(i))
        if(self.total_tables > 0):
            self.deploy.setEnabled(True)
        else:
            self.deploy.setEnabled(False)
    def on_addtable(self):
        ws.send("sync")
        result = ws.recv()
        self.total_tables = int(result)
        with open("data.txt", "w") as f:
            f.write(str(self.total_tables))
        f.close()
        self.update_tables()
    def on_connect(self):
        try: 
            
            ws.connect("ws://192.168.0.14")
            self.timer = QTimer()
            self.timer.timeout.connect(self.check_connection)
            self.timer.start(5000)
            self.update_tables()
            ws.send(str(self.total_tables))
            result = ws.recv()
            self.t1.connection = True
            self.connectionlabel.setText("Status: Connected")
            self.addtable.setEnabled(True)
            self.select.setEnabled(True)
            self.connect.setEnabled(False)
            self.resettable.setEnabled(True)
            self.update_tables()
        except TimeoutError:
            self.connectionlabel.setText("Status: Connection Timeout")
        
    def on_resettable(self):
        self.total_tables =0
        self.update_tables()
        with open("data.txt", "w") as f:
            f.write(str(self.total_tables))
        f.close()
        ws.send("reset")
        result = ws.recv()

    def returnfromDeploy(self):
        self.connectionlabel.setText("Status: Ready for Deployment")
        self.deploy.setEnabled(True)
        self.select.setEnabled(True)
        self.addtable.setEnabled(True)
        self.resettable.setEnabled(True)
        self.timer.start(5000)

    def on_deploy(self):
        self.timer.stop()
        target = (self.select.currentText())
        ws.send(target)
        result = ws.recv()
        self.deploy.setEnabled(False)
        self.select.setEnabled(False)
        self.addtable.setEnabled(False)
        self.connectionlabel.setText("Status: Out for Delivery")
        self.resettable.setEnabled(False)
        self.thread = QThread()
        self.worker = Worker()
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.run)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.worker.finished.connect(self.returnfromDeploy)
        self.thread.finished.connect(self.thread.deleteLater)
        self.thread.start()
        global flag
        flag= True


def main():
    app = QApplication([])
    window = MyGui()
    app.exec_()
if __name__ == '__main__':
    main()