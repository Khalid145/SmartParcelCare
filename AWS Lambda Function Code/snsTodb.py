#Define the libraries needed for the class.
from __future__ import print_function
import boto3
import json
from dateutil import parser

#This method is trigger when this lambda is called.
def lambda_handler(event, context):
#Obtain all of the required values from the SNS message.
messageid = event['Records'][0]['Sns']['MessageId']
timestamp = event['Records'][0]['Sns']['Timestamp']
message = event['Records'][0]['Sns']['Message']

#Specify the database connection.
dynamodb = boto3.resource('dynamodb')
table = dynamodb.Table('Message')

#Obtain the JSON from the message placeholder.
j = json.loads(message)s
deviceid = j['deviceid']
datano = j['datano']
latitude = j['latitude']
longitude = j['longitude']

#Place the data obtained from the JSON to the database.
table.put_item(
    Item={
        'deviceid' : deviceid,
        'timestamp': timestamp,
        'datano' : datano,
        'latitude': latitude,
        'longitude': longitude
    }
)