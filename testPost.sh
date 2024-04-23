curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 27" \
  --data-binary @- \
  http://localhost:8081/data/normal.txt <<EOF
Wikipedia without chunks. 
EOF
