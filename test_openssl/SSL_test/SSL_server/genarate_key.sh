#!/bin/bash

#export LD_LIBRARY_PATH=$(pwd)
#CNF= ./openssl.cnf
rm *.pem
openssl genrsa -out privkey.pem 2048
openssl req -new -x509 -key privkey.pem -out CAcert.pem -days 1095
openssl rsa -in privkey.pem -pubout -out pubkey.pem
