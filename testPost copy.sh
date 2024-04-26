curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 20000000000000000000000000000000" \
  --data-binary @- \
  http://localhost:8081/data/normal.txt << EOF
  Hello World 0
  Hello World 1
  Hello World 2
  Hello World 3
  Hello World 4
  Hello World 5
  Hello World 6
  Hello World 7
  Hello World 8
  Hello World 9
  EOF