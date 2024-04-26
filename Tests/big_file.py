import requests

# URL of your C++ HTTP server
url = 'http://localhost:8081/data/result.txt'  # Change this to your server's URL

# Path to the file you want to upload
file_path = 'Tests/quran.txt'  # Change this to the path of your file

def test_upload(file_path):
    try:
        # Open the file in binary mode
        with open(file_path, 'rb') as file:
            # Prepare the data to be sent
            files = {'file': file}

            # Send the POST request
            response = requests.post(url, files=files)

            # Check the response status code
            if response.status_code == 200 or response.status_code == 201:
                print("File uploaded successfully!")
                print(response.text)  # Assuming your server returns some confirmation
            else:
                print("Failed to upload file. Status code:", response.status_code)

    except FileNotFoundError:
        print("File not found at path:", file_path)
    except Exception as e:
        print("An error occurred:", str(e))

if __name__ == "__main__":
    test_upload(file_path)