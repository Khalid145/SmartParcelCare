from __future__ import print_function
import boto3
import json
from dateutil import parser

def lambda_handler(event, context):
    messageid = event['Records'][0]['Sns']['MessageId']
    timestamp = event['Records'][0]['Sns']['Timestamp']
    message = event['Records'][0]['Sns']['Message']
    
    dynamodb = boto3.resource('dynamodb')
    table = dynamodb.Table('Message')
    
    j = json.loads(message)
    
    deviceid = j['deviceid']
    datano = j['datano']
    latitude = j['latitude']
    longitude = j['longitude']
    
    
    
    table.put_item(
        Item={
            'deviceid' : deviceid,
            'timestamp': timestamp,
            'datano' : datano,
            'latitude': latitude,
            'longitude': longitude
            
        }
        )

    
   