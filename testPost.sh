curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 45" \
  --data-binary @- \
  http://localhost:8081/data/normal.txt < Tests/quran.txt