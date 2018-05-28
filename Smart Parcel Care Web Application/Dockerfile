# Base Image of the OS
FROM ubuntu:14.04

# Update OS
RUN sed -i 's/# \(.*multiverse$\)/\1/g' /etc/apt/sources.list
RUN apt-get update
RUN apt-get -y upgrade

# Install Python
RUN apt-get install -y python-dev python-pip

# Create a folder in the Base OS
RUN mkdir /app
WORKDIR /app

# Copy requirements file into the Base OS
COPY requirements.txt /app/requirements.txt

# Run a series of terminal commands needed by the application
RUN pip install -r requirements.txt
RUN pip install python-dateutil
RUN pip install Flask-WTF
RUN pip install boto3
RUN pip install awscli --upgrade --user
RUN export PATH=~/.local/bin:$PATH

# Copy everything from the host OS to the Dockerfile base OS
COPY . /app

# Open port 5000 for the application
EXPOSE 5000

# Run this command to start the application
CMD python app.py
