import serial,json,time
import boto3

client = boto3.client('sns')
arn = "arn:aws:sns:us-west-2:524203992316:NotifyMe"

arduinoSerialData = serial.Serial('/dev/cu.usbmodem1411', 9600)
while(1==1):
    if(arduinoSerialData.inWaiting()>0):
        myData = arduinoSerialData.readline()
        jsonData = json.loads(myData)
        print (jsonData)
        
        response = client.publish(TopicArn=arn,Message=jsonData)
        print("Response: {}".format(response))
