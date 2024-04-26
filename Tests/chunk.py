import requests

def chunked_http_request(url, data_chunks, headers=None):
    """
    Sends chunked HTTP requests to the specified URL.
    
    Args:
        url (str): The URL to which the requests will be sent.
        data_chunks (list): A list containing data chunks to be sent in the request body.
        headers (dict, optional): Additional headers to include in the request.

    Returns:
        requests.Response: The response object returned by the server.
    """
    response = None

    # Iterate through the data chunks
    for chunk in data_chunks:
        try:
            # Send the chunked request
            response = requests.post(url, data=chunk, headers=headers, stream=True)
            print("Chunk sent successfully:", response.text)
        except requests.exceptions.RequestException as e:
            print("Error sending chunk:", e)
            break

    return response

if __name__ == "__main__":
    # Example usage
    url = "http://localhost:8081/cgi/post_file.py"
    # data_chunks = ["3c\r\nhwduhwq\r\n"]
    data_chunks = ["3\r\nabc\r\n6\r\ndefghi\r\n0\r\n"]
    headers = {"Content-Type": "application/octet-stream",
               "Transfer-Encoding": "chunked"}

    response = chunked_http_request(url, data_chunks, headers)
    if response:
        print("Response:", response.status_code, response.text)