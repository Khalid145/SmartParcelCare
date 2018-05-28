# Define the libraries needed for the application.
from __future__ import print_function # Python 2/3 compatibility
from dateutil import parser
from flask import Flask, render_template, flash, redirect, url_for, session, logging, request
from wtforms import Form, StringField, TextAreaField, PasswordField, validators
from datetime import datetime
import boto3
import json
import decimal
from boto3.dynamodb.conditions import Key, Attr
from botocore.exceptions import ClientError
import os

# Create instance of Flask
app = Flask(__name__)

# Create empty array/list for data
datajson = []

# Specify the field for device id
class searchDevice(Form):
     deviceid = StringField('Enter the Parcel ID: ',[validators.Length(min=1, max=50)])

# Specifiy AWS region needed for the application to connect to AWS
os.environ.get('AWS_DEFAULT_REGION')

# This method will handle all request for the index page
@app.route('/', methods=['GET','POST'])
def index():
    form = searchDevice(request.form)
    # Check if the request mode made from the index is POST
    if request.method == 'POST' and form.validate():
        # Obtain value submitted by the user
        id = form.deviceid.data
        # Specfiy the connection to the database at AWS
        dynamodb=boto3.resource('dynamodb',region_name='us-west-2')
        # Specify the table in the database
        table=dynamodb.Table('Message')
        # Specify the key value needed by the database to search for record
        response = table.query(
            KeyConditionExpression=Key('deviceid').eq("device"+id)
        )
        # Temporary placeholder for the JSON
        datalist = []
        # Iterate through each value in item and create a JSON object
        for i in response['Items']:
            deviceid = i['deviceid']
            datano = i['datano']
            timestamp = i['timestamp']
            latitude = i['latitude']
            longitude = i['longitude']
            temperature = i['temperature']
            try:
                if 'humidity' in i:
                    humidity = i['humidity']
                else:
                    print("NOT FOUND")
            except:
                print ('EXCEPTION:')
            try:
                if 'impact' in i:
                    impact = i['impact']
                else:
                    print("NOT FOUND")
            except:
                print ('EXCEPTION:')
            try:
                if 'orientation' in i:
                    orientation = i['orientation']
                else:
                    print("NOT FOUND")
            except:
                print ('EXCEPTION:')
            # Add JSON values to temporary placeholder
            datalist.append(i)
        # Dump the JSON the temporary placeholder to the final JSON placeholder
        datajson = json.dumps(datalist)

        # If record was not found in the database, notify the user
        if not datalist:
            flash('Invalid Parcel ID. Please Try Again.','danger')
            return redirect(url_for('index'))
        else:
            # Direct the user to another page with the obtained JSON
            return redirect(url_for('parcelInfo', datajson=datajson))
    # Create template for the index page and the form in the index page
    return render_template('index.html',form=form)

# This method will handle all request for the parcelInfo page
@app.route('/parcelInfo')
def parcelInfo():
    # Retrieve the JSON from request
    datajson = request.args['datajson']
    # Create template for the parcelInfo page and the JSON needed by the parcelInfo page
    return render_template('parcelInfo.html',datajson=datajson)


# Specify that the code with be run directly from app.py and not imported
if __name__ == '__main__':
    # Required by Flask to maintain session
    app.secret_key="secret123"
    # Run the application in debug mode and on any IP
    app.run(debug=True,host='0.0.0.0')
