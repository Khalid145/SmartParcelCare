import serial,json,time
#import boto3
import requests



#client = boto3.client('sns')
arn = "arn:aws:sns:us-west-2:524203992316:NotifyMe"

arduinoSerialData = serial.Serial('/dev/cu.usbmodem1421', 9600)
def test():
    while(1==1):
        if(arduinoSerialData.inWaiting()>0):
            myData = arduinoSerialData.readline()
            try:
                response_json = response_json.decode('utf-8').replace('\0', '')
                jsonData = json.loads(myData)
            except:
                print('bad json: ', response_json)
            return jsonData

    
        #jsonData = json.loads(myData)
        #print (jsonData)
        
        #response = client.publish(TopicArn=arn,Message=jsonData)
        #print("Response: {}".format(response))
