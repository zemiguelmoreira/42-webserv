#!/bin/bash

if [ -n "$PATH_INFO" ]; then
	file_path=$(pwd)/$DOCUMENT_ROOT/$PATH_INFO
	if [ -n "$QUERY_STRING" ]; then
		file_name="${QUERY_STRING#*=}"
		full_path="$file_path/$file_name"
	else
		full_path="$file_path"
	fi

	if [ -f "$full_path" ]; then
		file_size=$(wc -c < "$full_path")
		content_type=$(file -b --mime-type "$full_path")
		echo "HTTP/1.1 200 OK"
		echo "Content-type: $content_type"
		echo "Content-Disposition: inline; filename=\"$file_name\""
		echo "Content-length: $file_size"
		echo ""
		cat "$full_path"
	else
		echo "HTTP/1.1 404 Not Found"
		echo "Content-type: text/html"
		echo ""
		echo "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><style>"
		echo ""
		echo "@import url('https://fonts.googleapis.com/css?family=Montserrat:300');body {background: #171717;color: #fff;font-family: 'Montserrat', sans-serif;font-size: 16px;padding: 0;margin: 0;height: 100vh;display: flex;align-items: center;justify-content: center;}span {text-decoration: none;color: #09d3dae7;}</style>"
		echo "<body>"
		echo "<h2>404 Not Found</h2>"
		echo "</body></html>"
	fi
else
	echo "HTTP/1.1 400 Bad Request"
	echo "Content-type: text/html"
	echo ""
	echo "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Document</title></head><style>"
	echo ""
	echo "@import url('https://fonts.googleapis.com/css?family=Montserrat:300');body {background: #171717;color: #fff;font-family: 'Montserrat', sans-serif;font-size: 16px;padding: 0;margin: 0;height: 100vh;display: flex;align-items: center;justify-content: center;}span {text-decoration: none;color: #09d3dae7;}</style>"
	echo "<body>"
	echo "<h2>400 Bad Request</h2>"
	echo "</body></html>"
fi
