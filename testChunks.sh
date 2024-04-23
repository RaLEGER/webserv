curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Transfer-Encoding: chunked" \
  --data-binary @- \
  http://localhost:8081/data/chunked.txt <<EOF
4\r\n
Wiki\r\n
5\r\n
pedia\r\n
E\r\n
 in\r\n\r\nchunks.\r\n
0\r\n
\r\n
EOF
