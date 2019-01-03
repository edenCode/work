#!/bin/bash

#export LD_LIBRARY_PATH=$(pwd)
#CNF= ./openssl.cnf
rm *.pem
openssl genrsa -out privkey.pem 1024
openssl req -new -x509 -key privkey.pem -out CAcert.pem -days 1095
