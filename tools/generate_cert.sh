#!/bin/bash
# Generate self-signed certificate for development TLS
# Requires openssl to be installed

mkdir -p certs
openssl req -x509 -newkey rsa:2048 -keyout certs/server.key -out certs/server.crt \
    -days 365 -nodes \
    -subj "/C=CN/ST=Beijing/L=Beijing/O=WonChat/CN=localhost"

echo "Certificates generated in certs/ directory"
echo "  certs/server.crt - public certificate"
echo "  certs/server.key  - private key (keep secret)"
