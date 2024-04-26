curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 17" \
  --data-binary @- \
  http://localhost:8081/cgi/post_file.py<< EOF
Body of the file
EOF