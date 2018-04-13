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

app = Flask(__name__)

datajson = []

class searchDevice(Form):
     deviceid = StringField('Enter the Parcel ID: ',[validators.Length(min=1, max=50)])


os.environ.get('AWS_DEFAULT_REGION')


@app.route('/', methods=['GET','POST'])
def index():
    form = searchDevice(request.form)
    if request.method == 'POST' and form.validate():
        id = form.deviceid.data

        dynamodb=boto3.resource('dynamodb',region_name='us-west-2')

        table=dynamodb.Table('Message')
        response = table.query(
            KeyConditionExpression=Key('deviceid').eq("device"+id)
        )
        datalist = []
        for i in response['Items']:
            deviceid = i['deviceid']
            datano = i['datano']
            timestamp = i['timestamp']
            latitude = i['latitude']
            longitude = i['longitude']
            temperature = i['temperature']

            print(longitude)




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

            #print(i)


            datalist.append(i)

            #print(datalist)

        datajson = json.dumps(datalist)
        #datajson = simplejson.dumps([datalist], default=json_encode_decimal

        if not datalist:
            flash('Invalid Parcel ID. Please Try Again.','danger')
            return redirect(url_for('index'))
        else:
            return redirect(url_for('parcelInfo', datajson=datajson))

    return render_template('index.html',form=form)

@app.route('/parcelInfo')
def parcelInfo():
    datajson = request.args['datajson']
    return render_template('parcelInfo.html',datajson=datajson)



if __name__ == '__main__':
    app.secret_key="secret123"
    app.run(debug=True,host='0.0.0.0')
    #app.run(debug='True')
