#!/bin/bash

# Check if port number is provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <port>"
    exit 1
fi

PORT=$1

# Incorrect HTTP request: Non-standard HTTP method
request1="FOO / HTTP/1.1"

# Incorrect HTTP request: Invalid HTTP version
request2="GET / HTTP/1.9"

# Incorrect HTTP request: Invalid path // THIS ONE IS A VALID REQUEST
request3="GET /nonexistentpath HTTP/1.1"

# Incorrect HTTP request: Too many tokens on the first line
request4="GET / HTTP/1.1 extra/token" 

# Incorrect HTTP request: Not enough tokens on the first line
request5="GET HTTP/1.1"

# Incorrect HTTP request: Duplicate headers
request6="GET / HTTP/1.1\r\nHost: localhost\r\nHost: localhost"

# Incorrect HTTP request: No ':' delimiter in header
request7="GET / HTTP/1.1\r\nHost localhost"

# Incorrect HTTP request: No double line-break to separate from body
request8="GET / HTTP/1.1\r\nHost: localhost\r\n\r\nBody"

# Correct HTTP request: trap the header parser with a body that contains a colon
request9="GET / HTTP/1.1\r\nHost: localhost\r\nHost: localhist\r\n\r\nthis is the body: hello trap\r\n"

# generated random characters request
# request9= 

# Array of incorrect requests
requests=("$request1" "$request2" "$request3" "$request4" "$request5" "$request6" "$request7" "$request8")

# Send each request to localhost at the given port
for request in "${requests[@]}"
do
    echo "Sending request: $request"
    echo -e $request | nc localhost $PORT
done