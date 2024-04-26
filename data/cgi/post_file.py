import os
import sys

# Get environment variable
content_length = os.getenv('CONTENT_LENGTH')

# Read content from standard input
content = sys.stdin.read(int(content_length))

# Append content to db.txt
with open('new/db.txt', 'a') as f:
    f.write(content + '\n')