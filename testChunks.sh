curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Transfer-Encoding: chunked" \
  --data-binary @- \
  http://localhost:8081/data/chunked.txt <<EOF
"3\r\n
abc\r\n
6\r\n
defghi\r\n
0\r\n"
EOF
