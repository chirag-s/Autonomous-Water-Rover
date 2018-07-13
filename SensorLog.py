import serial
import re
import sqlite3 
import sys
import math
 
dbname='sensorsData.db'

ser = serial.Serial('/dev/ttyUSB0',9600)
temp=0
pH=0
turb=0
rain=0


# log sensor data on database
def logData (temp, pH, turb, rain):	
	conn=sqlite3.connect(dbname)
	curs=conn.cursor()
	curs.execute("INSERT INTO Sensor_data values(datetime('now'), (?), (?), (?), (?))", (temp, pH, turb, rain))
	conn.commit()
	conn.close()

# display database data
def displayData():
	conn=sqlite3.connect(dbname)
	curs=conn.cursor()
	print ("\nEntire database contents:\n")
	for row in curs.execute("SELECT * FROM Sensor_data"):
		print (row)
	conn.close()
	
while True:
        read_serial=ser.readline()
        read_list = read_serial.split()
        read_list_num = [float(i) for i in read_list]
        size1=len(read_list_num)
        if size1==2:
                leftDistance=read_list_num[1];
                rightDistance=read_list_num[0];
        else:
                temp=read_list_num[0]
                pH=read_list_num[1]
                turb=-1120.4*math.pow(read_list_num[2],2)+5742.3*read_list_num[2]-4352.9
                rain=read_list_num[3]
		print('logged')
                logData(temp, pH,turb,rain)
                #c=c+1
                #print(c)
#                displayData()
                
        
        
