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
from boto3.session import Session

app = Flask(__name__)

datajson = []

aws_session_token = ''
aws_access_key_id = 'AKIAIZYGKEGVYHCC375Q'
aws_secret_access_key = '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'
region =  'us-west-2'




class searchDevice(Form):
     deviceid = StringField('Enter the Parcel ID: ',[validators.Length(min=1, max=50)])


@app.route('/', methods=['GET','POST'])
def index():
    form = searchDevice(request.form)
    if request.method == 'POST' and form.validate():
        id = form.deviceid.data

        dynamodb_session = Session(aws_access_key_id= aws_access_key_id,
          aws_secret_access_key= aws_secret_access_key,
          region_name= region)

        dynamodb = dynamodb_session.resource('dynamodb')

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



            datalist.append(i)

        datajson = json.dumps(datalist)

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

    app.run(host='0.0.0.0')
