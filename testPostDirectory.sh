curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 17" \
  --data-binary @- \
  http://localhost:8081/data/ << EOF
Body of the file
EOF