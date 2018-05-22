import serial,json,time
import boto3
import requests
import re
import simplejson

client = boto3.client('sns')
arn = "arn:aws:sns:us-west-2:524203992316:NotifyMe"

arduinoSerialData = serial.Serial('/dev/ttyACM0', 9600)

while(1==1):
    if(arduinoSerialData.inWaiting()>0):
        myData = arduinoSerialData.readline()

        
        print(myData)
        #jsonData = json.loads(myData)
        #print("JSON")
        
        #jsonData = json.dumps(myData)
        print (myData)
        
        response = client.publish(TopicArn=arn,Message=myData)
        print("Response: {}".format(response))
