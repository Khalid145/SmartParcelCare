#Define the libraries needed for the class.
import serial,json,time
import boto3
import requests
import re
import simplejson

#Specify the SNS connection.
client = boto3.client('sns')
arn = "arn:aws:sns:us-west-2:524203992316:NotifyMe"

#Specify the serial port and baud for the arduino.
arduinoSerialData = serial.Serial('/dev/ttyACM0', 9600)

#This condition will run forever.
while(1==1):
    #Check if the serial port has data sent through it.
    if(arduinoSerialData.inWaiting()>0):
        #Place sent serial data into a place holder.
        myData = arduinoSerialData.readline()
        
        #Prints the contents of the placeholder. This will be the JSON.
        print (myData)
        
        #Send the contents of the placeholder(JSON) to SNS.
        response = client.publish(TopicArn=arn,Message=myData)
        
        #Print response of the SNS.
        print("Response: {}".format(response))
