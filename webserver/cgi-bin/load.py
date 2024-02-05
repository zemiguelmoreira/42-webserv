#!/usr/bin/env python3
import cgi
import os

document_root_variable = 'DOCUMENT_ROOT'
document_root_value = os.environ.get(document_root_variable)

path_info_variable = 'PATH_INFO'
path_info_value = os.environ.get(path_info_variable)

folder_name = os.path.join(document_root_value, path_info_value)

# Create a form object to parse the input
form = cgi.FieldStorage()

# Check if the file is present in the POST request
if "inpFile" in form:
	# Get the file data
	file_item = form["inpFile"]

	# Check if the file was uploaded
	if file_item.filename:
		# Create the folder if it doesn't exist
		save_directory = os.path.join(os.getcwd(), folder_name)
		if not os.path.exists(save_directory):
			os.makedirs(save_directory)

		# Create a new file in the specified folder
		file_path = os.path.join(save_directory, os.path.basename(file_item.filename))
		with open(file_path, 'wb') as file:
			# Write the file data to the new file
			file.write(file_item.file.read())
		print("HTTP/1.1 201 Created\r\nContent-type: text/html\r\n\r\n")
		print("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><style>@import url('https://fonts.googleapis.com/css?family=Montserrat:300');body {background: #171717;color: #fff;font-family: 'Montserrat', sans-serif;font-size: 16px;padding: 0;margin: 0;height: 100vh;display: flex;align-items: center;justify-content: center;}span {text-decoration: none;color: #09d3dae7;}</style>")
		print("<body>")
		print("<h2>File successfully uploaded and saved!")
		print("</body></html>")
	else:
		print("HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\n\r\n")
		print("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><style>@import url('https://fonts.googleapis.com/css?family=Montserrat:300');body {background: #171717;color: #fff;font-family: 'Montserrat', sans-serif;font-size: 16px;padding: 0;margin: 0;height: 100vh;display: flex;align-items: center;justify-content: center;}span {text-decoration: none;color: #09d3dae7;}</style>")
		print("<body>")
		print("<h2>Error: No file uploaded!</h2>")
		print("</body></html>")
else:
		print("HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\n\r\n")
		print("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><style>@import url('https://fonts.googleapis.com/css?family=Montserrat:300');body {background: #171717;color: #fff;font-family: 'Montserrat', sans-serif;font-size: 16px;padding: 0;margin: 0;height: 100vh;display: flex;align-items: center;justify-content: center;}span {text-decoration: none;color: #09d3dae7;}</style>")
		print("<body>")
		print("<h2>Error: No file field in the POST request!</h2>")
		print("</body></html>")
