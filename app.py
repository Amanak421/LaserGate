from xml.etree.ElementTree import Comment
from PyQt5.QtGui import QFont, QIcon
import serial
import time
import sys
from PyQt5 import QtCore, QtWidgets, uic
from PyQt5.QtCore import pyqtSignal

import random


"""while True:

    line = arduino.readline()
    string = line.decode()
    if string != "":
        print(string.strip())"""

class MainWindow(QtWidgets.QWidget):
    def __init__(self):
        super(MainWindow, self).__init__()
        uic.loadUi("ui/app.ui", self)

        self.setWindowIcon(QIcon("img/lasergate_logo.png"))

        self.port = ""
        self.baudrate = 0

        self.arduino  = serial.Serial(port = None, timeout=.1)

        self.com_clock = QtCore.QTimer()
        self.com_clock.setInterval(100)
        self.com_clock.timeout.connect(self.getCommand)

        self.time = ""
        self.timer_run = False

        self.port_open = False

        self.response_values = False

        self.setting = Settings()

        self.current_result = ""
        self.results = []
        self.speed_time = 0
        self.distance = 0
        self.speed = 0

        self.foto_start = 0
        self.foto_stop = 0

        
        self.GAME_TIME = 0
        self.GAME_SPEED = 1
        self.game_mod = self.GAME_TIME
        self.findChild(QtWidgets.QStackedWidget, "game_mod_change").setCurrentIndex(self.game_mod)

        #game guess speed
        self.guess_speed_cur_result = ""
        self.guess_speed_results = []
        self.generated_speed = 0
        self.difference = 0
        self.max_speed = 5

        #self.findChild(QtWidgets.QLabel, "generated_speed").setText( + ' <span style=" vertical-align:super;">-1</span>')

        self.setting.findChild(QtWidgets.QComboBox, "mod").currentIndexChanged.connect(self.changeGameMod)
        self.findChild(QtWidgets.QPushButton, "add_guess").clicked.connect(self.addToGuesResults)
        self.findChild(QtWidgets.QPushButton, "generate_speed_button").clicked.connect(self.generateSpeed)

        self.fillGuessResults()

        self.fillResults()

        self.findChild(QtWidgets.QPushButton, "connect").clicked.connect(self.openPort)
        self.findChild(QtWidgets.QPushButton, "settings").clicked.connect(self.showSettings)
        self.setting.serial_monitor.sendCom.connect(self.sendCommand)
        self.findChild(QtWidgets.QPushButton, "add_time").clicked.connect(self.addToList)
        self.setting.delete_res.connect(self.deleteRes)
        self.findChild(QtWidgets.QPushButton, "button_reset").clicked.connect(self.reset)
        self.findChild(QtWidgets.QPushButton, "button_reset_2").clicked.connect(self.reset)
        self.resizeEvent = self.resizeTimer

    def openPort(self):
        _port = ""
        _port = self.findChild(QtWidgets.QLineEdit, "com").text()
        if _port == "" or _port[:3] != "COM":
            print("ERROR")
            self.showDialog("Naplatný COM port", "Zvolili jste neplatný COM port!", "Zkontrolujte zapojení bran a zápis portu COM (musí být ve formátu COMx)") #show dialog
            return
        
        try: 
            print("COM ", _port)
            self.arduino.setPort(_port)

        except: 
            print("ERROR COM")
            self.showDialog("Naplatný COM port", "Zvolili jste neplatný COM port!", "Zkontrolujte zapojení bran a zápis portu COM (musí být ve formátu COMx)") #show dialog
            return

        _baudrate = ""
        _baudrate = self.findChild(QtWidgets.QLineEdit, "baudrate").text()
        if _baudrate == "":
            print("ERROR")
            self.showDialog("Naplatný BAUDRATE", "Zadali jste neplatný baudrate", "Zkontrolujte zda jste uvedli číselou hodntu (např. 115200)")   #show dialog
            return
        
        try: 
            self.arduino.baudrate = int(_baudrate)


        except: 
            print("ERROR BAUD")
            self.showDialog("Naplatný BAUDRATE", "Zadali jste neplatný baudrate", "Zkontrolujte zda jste uvedli číselou hodntu (např. 115200)")   #show dialog        #show dialog
            return


        try: 
            self.arduino.open()

        except: 
            print("ERROR OPEN")
            self.showDialog("Nepodařilo se otevřít COM port", "Zkontrolujte zapojení bran a zápisu portu COM", "")
            return

        self.findChild(QtWidgets.QPushButton, "connect").setText("Připojeno!")
        self.findChild(QtWidgets.QPushButton, "connect").setEnabled(False)
        self.findChild(QtWidgets.QLineEdit, "com").setEnabled(False)
        self.findChild(QtWidgets.QLineEdit, "baudrate").setEnabled(False)
        self.port_open = True
        self.com_clock.start()
        self.sendCommand("C-CONNECT")       #send command to communicate with computer 

    def getCommand(self):
        line = self.arduino.readline()
        string = line.decode()
        if string == "":
            return
        command = string.strip()

        self.setting.serial_monitor.addLog(command)

        if command[0] == "C":

            if command[2:] == "C-START":
                self.findChild(QtWidgets.QStackedWidget, "change_widget").setCurrentIndex(0)
                self.timer_run = True
                print("TIMER RUN TRUE")
            elif command[2:] == "C-STOP":
                self.timer_run = False
            elif command[2] == "T":
                self.time = command[4:]
                self.updateTime()
                #print("TIME", self.time)
            elif command[2] == "R":
                self.current_result = command[4:]
                self.showResults()
            elif command == "C-CONNECTED":
                print("DEVICE CONNECTED")
            elif command == "C-I-DONE":
                self.sendCommand("C-CONNECT")
            elif command[2] == "A":
                self.distance = command[6:command.index("Z") - 1]
                self.foto_start = command[command.index("Z") + 2 : command.index("K") - 1]
                self.foto_stop = command[command.index("K") + 2:]
                print("VALUES", self.distance, self.foto_start, self.foto_stop)
                self.response_values = True

    def sendCommand(self, _text):
        print("COMMAND", _text)
        self.arduino.write(_text.encode('utf-8'))

    def updateTime(self):
        if self.timer_run == True:
            #print("RAW TIME", self.time)
            minute = self.time[:1]
            second = self.time[self.time.index(":") + 1: self.time.index(".")]
            milisecond = self.time[self.time.index(".")+1:]
            #print("FORMATED TIME", minute, second, milisecond)
            if len(minute) == 1:
                minute = "0" + minute
            self.findChild(QtWidgets.QLabel, "timer_min").setText(minute)
            if len(second) == 1:
                second = "0" + second
            self.findChild(QtWidgets.QLabel, "timer_sec").setText(second)
            self.findChild(QtWidgets.QLabel, "timer_set").setText(milisecond)

    def showDialog(self, _title, _text, _description = ""):
        msg = QtWidgets.QMessageBox()
        msg.setIcon(QtWidgets.QMessageBox.Information)

        msg.setText(_text)
        msg.setInformativeText(_description)
        msg.setWindowTitle(_title)
        msg.setStandardButtons(QtWidgets.QMessageBox.Ok)
            
        msg.exec_()
        #print "value of pressed message box button:", retval
        
    def showSettings(self):
        self.sendCommand("C-ALL")
        while self.response_values != True:
            self.getCommand()
        self.response_values = False

        if self.port_open:
            self.setting.setValues(self.distance, self.foto_start, self.foto_stop)
            self.setting.show()
        else:
            self.showDialog("Zařízení není připojeno", "Zkontrolujte připojení zařízení a akci opakujte")

    def showResults(self):
        self.findChild(QtWidgets.QStackedWidget, "change_widget").setCurrentIndex(1)

        print(self.current_result)

        self.findChild(QtWidgets.QLabel, "results_distance").setText(self.current_result[:self.current_result.index(":")] + ' m')
        self.distance = float(self.current_result[:self.current_result.index(":")])
        self.findChild(QtWidgets.QLabel, "results_time").setText(self.current_result[self.current_result.index(":") + 1: self.current_result.index("/")] + " s")
        self.speed_time = float(self.current_result[self.current_result.index(":") + 1: self.current_result.index("/")])
        self.findChild(QtWidgets.QLabel, "results_speed").setText(self.current_result[self.current_result.index("/") + 1:] + ' ms<span style=" vertical-align:super;">-1</span>')
        self.speed = float(self.current_result[self.current_result.index("/") + 1:])

        if self.game_mod == self.GAME_SPEED:
            self.difference = abs(self.generated_speed - self.speed)
            self.difference = round(self.difference, 1)
            self.findChild(QtWidgets.QLabel, "diff_label").setText(str(self.difference) + ' ms<span style=" vertical-align:super;">-1</span>')
        

    def showTimer(self):
        self.result.setHidden(True)
        self.timer.setHidden(True)

    def addToList(self):
        text = self.findChild(QtWidgets.QLineEdit, "add_time_name").text()

        if text == "":
            self.showDialog("Nezadali jste jméno výsledku", "Přidejte jméno patřící k naměřenému výsledku a akci zopakujte.")
            return
        elif self.findChild(QtWidgets.QStackedWidget, "change_widget").currentIndex() != 1:
            self.showDialog("Měření neproběhlo", "Nejprve proveďte měření a poté přidejte výsledek!")
            return

        _result = {"Name": text, "Time": self.speed_time, "Distance": self.distance, "Speed": self.speed}
        self.results.append(_result)

        self.results = sorted(self.results, key = lambda i: i['Time'])
        #print(self.results)
        self.fillResults()

    def fillResults(self):
        self.findChild(QtWidgets.QListWidget, "best_times_list").clear()
        first_item = QtWidgets.QListWidgetItem()
        top_bar = ResultItem("Jméno", "Čas", "Rychlost", "Pořadí")
        first_item.setSizeHint(top_bar.sizeHint())
        self.findChild(QtWidgets.QListWidget, "best_times_list").addItem(first_item)
        self.findChild(QtWidgets.QListWidget, "best_times_list").setItemWidget(first_item, top_bar)

        for i, _res in enumerate(self.results):
            item = QtWidgets.QListWidgetItem()
            _result = ResultItem(_res["Name"], _res["Time"], _res["Speed"], i + 1)
            item.setSizeHint(_result.sizeHint())
            self.findChild(QtWidgets.QListWidget, "best_times_list").addItem(item)
            self.findChild(QtWidgets.QListWidget, "best_times_list").setItemWidget(item, _result)

    def deleteRes(self):
        self.findChild(QtWidgets.QListWidget, "best_times_list").clear()
        self.results.clear()
        self.fillResults()
        self.guess_speed_results.clear()
        self.fillGuessResults()

    def reset(self):
        if self.port_open:
            self.sendCommand("C-G-RESET")
            self.speed_time = 0
            self.distance = 0
            self.speed = 0
            self.findChild(QtWidgets.QLabel, "timer_min").setText("00")
            self.findChild(QtWidgets.QLabel, "timer_sec").setText("00")
            self.findChild(QtWidgets.QLabel, "timer_set").setText("0")
            self.findChild(QtWidgets.QStackedWidget, "change_widget").setCurrentIndex(0)

            #game mode guess
            self.findChild(QtWidgets.QLabel, "diff_label").setText('0.0 ms<span style=" vertical-align:super;">-1</span>')
        else:
            self.showDialog("Zařízení není připojeno", "Zkontrolujte připojení zařízení a správnost COM portu!")

    def resizeTimer(self, evt):
        
        size = int(self.height() * 0.2)
        font_min = self.findChild(QtWidgets.QLabel, "timer_min").font()
        font_min.setPointSize(size)
        self.findChild(QtWidgets.QLabel, "timer_min").setFont(font_min)
        font_sec = self.findChild(QtWidgets.QLabel, "timer_sec").font()
        font_sec.setPointSize(size)
        self.findChild(QtWidgets.QLabel, "timer_sec").setFont(font_sec)
        font_set = self.findChild(QtWidgets.QLabel, "timer_set").font()
        font_set.setPointSize(size)
        self.findChild(QtWidgets.QLabel, "timer_set").setFont(font_set)
        div_1 = self.findChild(QtWidgets.QLabel, "divider_1").font()
        div_1.setPointSize(size)
        self.findChild(QtWidgets.QLabel, "divider_1").setFont(div_1)
        div_2 = self.findChild(QtWidgets.QLabel, "divider_2").font()
        div_2.setPointSize(size)
        self.findChild(QtWidgets.QLabel, "divider_2").setFont(div_2)

        result_size = int(self.height() * 0.06)
        font_time = self.findChild(QtWidgets.QLabel, "time_label_text").font()
        font_time.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "time_label_text").setFont(font_time)
        font_time_i = self.findChild(QtWidgets.QLabel, "results_time").font()
        font_time_i.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "results_time").setFont(font_time_i)
        font_dist = self.findChild(QtWidgets.QLabel, "label_dis_text").font()
        font_dist.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "label_dis_text").setFont(font_dist)
        font_dist_i = self.findChild(QtWidgets.QLabel, "results_distance").font()
        font_dist_i.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "results_distance").setFont(font_dist_i)
        font_speed = self.findChild(QtWidgets.QLabel, "label_speed_text").font()
        font_speed.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "label_speed_text").setFont(font_speed)
        font_speed_i = self.findChild(QtWidgets.QLabel, "results_speed").font()
        font_speed_i.setPointSize(result_size)
        self.findChild(QtWidgets.QLabel, "results_speed").setFont(font_speed_i)

        if self.game_mod == self.GAME_SPEED:
            size_result = int(self.width() * 0.02)
            font_g_speed = self.findChild(QtWidgets.QLabel, "g_speed_label").font()
            font_g_speed.setPointSize(size_result)
            self.findChild(QtWidgets.QLabel, "g_speed_label").setFont(font_g_speed)
            font_diff = self.findChild(QtWidgets.QLabel, "generated_speed").font()
            font_diff.setPointSize(size_result)
            self.findChild(QtWidgets.QLabel, "generated_speed").setFont(font_diff)
            font_g_speed_i = self.findChild(QtWidgets.QLabel, "diff_label_text").font()
            font_g_speed_i.setPointSize(size_result)
            self.findChild(QtWidgets.QLabel, "diff_label_text").setFont(font_g_speed_i)
            font_diff_i = self.findChild(QtWidgets.QLabel, "diff_label").font()
            font_diff_i.setPointSize(size_result)
            self.findChild(QtWidgets.QLabel, "diff_label").setFont(font_diff_i)
            """font_gen = self.findChild(QtWidgets.QPushButton, "generate_speed_button").font()
            font_gen.setPointSize(size_result)
            self.findChild(QtWidgets.QLabel, "generate_speed_button").setFont(font_gen)"""

    def changeGameMod(self, _mod):
        if _mod == self.GAME_TIME:
            self.game_mod = self.GAME_TIME
        else:
            self.game_mod = self.GAME_SPEED

        self.findChild(QtWidgets.QStackedWidget, "game_mod_change").setCurrentIndex(self.game_mod)

    def addToGuesResults(self):
        text = self.findChild(QtWidgets.QLineEdit, "guess_name").text()

        if text == "":
            self.showDialog("Nezadali jste jméno výsledku", "Přidejte jméno patřící k naměřenému výsledku a akci zopakujte.")
            return
        elif self.findChild(QtWidgets.QStackedWidget, "change_widget").currentIndex() != 1:
            self.showDialog("Měření neproběhlo", "Nejprve proveďte měření a poté přidejte výsledek!")
            return

        _result = {"Name": text, "Speed": self.speed, "G_speed": self.generated_speed, "Diff": self.difference}
        self.guess_speed_results.append(_result)

        self.guess_speed_results = sorted(self.guess_speed_results, key = lambda i: i['Diff'])
        print(self.results)
        self.fillGuessResults()

    def fillGuessResults(self):
        self.findChild(QtWidgets.QListWidget, "guess_results_list").clear()
        first_item = QtWidgets.QListWidgetItem()
        top_bar = ResultItem("Jm.", "G", "N", "P")
        first_item.setSizeHint(top_bar.sizeHint())
        self.findChild(QtWidgets.QListWidget, "guess_results_list").addItem(first_item)
        self.findChild(QtWidgets.QListWidget, "guess_results_list").setItemWidget(first_item, top_bar)

        for i, _res in enumerate(self.guess_speed_results):
            item = QtWidgets.QListWidgetItem()
            _result = ResultItem(_res["Name"], _res["G_speed"], _res["Speed"], i + 1)
            item.setSizeHint(_result.sizeHint())
            self.findChild(QtWidgets.QListWidget, "guess_results_list").addItem(item)
            self.findChild(QtWidgets.QListWidget, "guess_results_list").setItemWidget(item, _result)

    def generateSpeed(self):
        _g_speed = random.randint(1, self.max_speed * 10) / 10
        self.findChild(QtWidgets.QLabel, "generated_speed").setText(str(_g_speed) + ' ms<span style=" vertical-align:super;">-1</span>')
        self.generated_speed = _g_speed
        #game mode guess
        self.findChild(QtWidgets.QLabel, "diff_label").setText('0.0 ms<span style=" vertical-align:super;">-1</span>')

    def getValues(self):
        self.sendCommand("C-ALL")


class Results(QtWidgets.QWidget):
    def __init__(self, _dis, _time, _speed):
        super(Results, self).__init__()
        uic.loadUi("ui/results.ui", self)
        self.setTime(_time)
        self.setDistance(_dis)
        self.setSpeed(_speed)
    
    def setTime(self, _time):
        self.findChild(QtWidgets.QLabel, "results_time").setText(str(_time))

    def setDistance(self, _dis):
        self.findChild(QtWidgets.QLabel, "results_distance").setText(str(_dis))

    def setSpeed(self, _speed):
        self.findChild(QtWidgets.QLabel, "results_speed").setText(str(_speed))

        
class Settings(QtWidgets.QWidget):

    delete_res = pyqtSignal()

    def __init__(self):
        super(Settings, self).__init__()
        uic.loadUi("ui/settings.ui", self)

        self.setWindowIcon(QIcon("img/lasergate_logo.png"))

        self.serial_monitor = SerialMonitor()
        self.setObjectName("serial_monitor_class")

        self.findChild(QtWidgets.QPushButton, "show_monitor").clicked.connect(self.showSerialMonitor)
        self.findChild(QtWidgets.QPushButton, "delete_results").clicked.connect(lambda: self.delete_res.emit())

    def showSerialMonitor(self):
        self.serial_monitor.show()

    def setValues(self, _dis, _f_start, _f_stop):
        self.findChild(QtWidgets.QLineEdit, "distance").setText(str(_dis))
        self.findChild(QtWidgets.QLineEdit, "start_value").setText(str(_f_start))
        self.findChild(QtWidgets.QLineEdit, "stop_value").setText(str(_f_stop))
    
    def updateField(self, _value, _field):
        if _field == "dis":
            self.findChild(QtWidgets.QLineEdit, "distance").setText(str(_value))
        elif _field == "start":
            self.findChild(QtWidgets.QLineEdit, "start_value").setText(str(_value))
        elif _field == "stop":
            self.findChild(QtWidgets.QLineEdit, "stop_value").setText(str(_value))

    

class SerialMonitor(QtWidgets.QWidget):

    sendCom = pyqtSignal(str)

    def __init__(self):
        super(SerialMonitor, self).__init__()
        uic.loadUi("ui/serial.ui", self)

        self.setWindowIcon(QIcon("img/lasergate_logo.png"))

        self.log = QtWidgets.QWidget()
        self.serial_content = QtWidgets.QVBoxLayout()
        self.log.setLayout(self.serial_content)
        self.findChild(QtWidgets.QScrollArea, "serial_monitor").setWidget(self.log)
        self.findChild(QtWidgets.QPushButton, "send").clicked.connect(self.sendCommand)

    def addLog(self, _log):
        log = QtWidgets.QLabel(_log)
        log.setStyleSheet("margin: 1px; padding: 1px;")
        self.serial_content.addWidget(log)
        vbar = self.findChild(QtWidgets.QScrollArea, "serial_monitor").verticalScrollBar()
        vbar.setValue(vbar.maximum())

    def sendCommand(self):
        text = self.findChild(QtWidgets.QLineEdit, "console_text").text()
        print("COMMAND_RAW", text)
        self.sendCom.emit(text)

class ResultItem(QtWidgets.QWidget):
    def __init__(self, _name, _time, _speed, _pos):
        super(ResultItem, self).__init__()
        uic.loadUi("ui/result_list_item.ui", self)

        self.setWindowIcon(QIcon("img/lasergate_logo.png"))

        self.setTime(_time)
        self.setName(_name)
        self.setSpeed(_speed)
        self.setPosition(_pos)
    
    def setTime(self, _time):
        self.findChild(QtWidgets.QLabel, "time").setText(str(_time))

    def setName(self, _name):
        self.findChild(QtWidgets.QLabel, "name").setText(str(_name))

    def setSpeed(self, _speed):
        self.findChild(QtWidgets.QLabel, "speed").setText(str(_speed))

    def setPosition(self, _pos):
        self.findChild(QtWidgets.QLabel, "place").setText(str(_pos))



app = QtWidgets.QApplication(sys.argv)
window = MainWindow()
window.show()

sys.exit(app.exec_())