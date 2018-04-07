import boto3

client = boto3.client('sns')

message = '{"deviceid":"device1","datano":"5","humidity":"43","temperature":"21","latitude":"0","longitude":"0","impact":"123","orientation":"321"}'
arn = "arn:aws:sns:us-west-2:524203992316:NotifyMe"

response = client.publish(
    TopicArn=arn,
    Message=message
)
print("Response: {}".format(response))
