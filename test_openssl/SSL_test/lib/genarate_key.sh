#!/bin/bash

export LD_LIBRARY_PATH=$(pwd)
CNF= ./openssl.cnf
./openssl genrsa -out privkey.pem 2048
./openssl req -config $CNF -new -x509 -key privkey.pem -out CAcert.pem -days 1095
