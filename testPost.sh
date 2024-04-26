curl -X POST \
  -H "Content-Type: application/octet-stream" \
  -H "Content-Length: 20000000000000000000000000000" \
  --data-binary @- \
  http://localhost:8081/data/normal.txt < Tests/quran.txt