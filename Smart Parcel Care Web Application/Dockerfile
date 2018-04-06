FROM ubuntu:14.04
 
# Update OS
RUN sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list
RUN apt-get update
RUN apt-get -y upgrade
 
# Install Python
RUN apt-get install -y python-dev python-pip


EXPOSE 5000

RUN mkdir /app
WORKDIR /app

COPY requirements.txt /app/requirements.txt
RUN pip install -r requirements.txt
RUN pip install python-dateutil
RUN pip install Flask-WTF
RUN pip install boto3

RUN pip install awscli --upgrade --user
RUN export PATH=~/.local/bin:$PATH

COPY . /app

EXPOSE 5000

CMD python app.py
