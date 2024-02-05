/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HandleData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2024/02/05 19:30:23 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HandleData.hpp"

HandleData::HandleData() {}

HandleData::HandleData(const HandleData& other) {
	*this = other;
}

HandleData::HandleData(int& clientSocket, Config *config) : _clientSocket(clientSocket), _config(config) {
	_isDir = false;
	_workDir = "www";
	_mime["html"] = "Content-type: text/html";
	_mime["htm"] = "Content-type: text/html";
	_mime["css"] = "Content-type: text/css";
	_mime["js"] = "Content-type: text/javascript";
	_mime["pdf"] = "Content-type: application/pdf";
	_mime["txt"] = "Content-type: text/plain";
	_mime["jpeg"] = "Content-type: image/jpeg";
	_mime["jpg"] = "Content-type: image/jpeg";
	_mime["gif"] = "Content-type: image/gif";
	_mime["png"] = "Content-type: image/png";
	_mime["ico"] = "Content-type: image/x-icon";
	_readTotal = 0;
	_contentLength = 0;
	_isCgi = 0;
	_boundary = 0;
}

HandleData::~HandleData() {}

HandleData& HandleData::operator=(const HandleData& other) {
	if (this != &other) {
		this->_clientSocket = other._clientSocket;
		this->_readTotal = other._readTotal;
		this->_envs = other._envs;
		this->_contentLength = other._contentLength;
		this->_body = other._body;
		this->_boundary = other._boundary;
		this->_mime = other._mime;
		this->_isCgi = other._isCgi;
		this->_isDir = other._isDir;
		this->_workDir = other._workDir;
		this->_webSender = other._webSender;
		this->_config = other._config;
	}
	return (*this);
}

bool	HandleData::isRedirection()
{
	std::vector<std::string> redirPaths = _config->get_redirectionPaths();
	for (size_t i = 0; i < redirPaths.size(); i++)
	{
		if (redirPaths[i] == _envs["REQUEST_URI"])
			return (true);
	}
	return (false);
}

bool	HandleData::sendRedirection()
{
	std::vector<std::string> 			redirPaths = _config->get_redirectionPaths();


	std::vector<Location>	loc = _config->get_locations();
	if ( loc.size() <= 0 )
	{
		// std::cerr << "No locations found... moving on!" << std::endl;
		return (false);
	}

	if ( _envs["REQUEST_URI"].empty() )
	{
		// std::cerr << "URI not found... moving on!" << std::endl;
		return (false);
	}

	std::vector<std::string>::iterator redir =  std::find( redirPaths.begin(), redirPaths.end(), _envs["REQUEST_URI"] );
	if ( redir == redirPaths.end() )
	{
		// std::cout << "URI: " << _envs["REQUEST_URI"] << " has no redirection... moving on!" << std::endl;
		return (false);
	}
	else
	{
		// std::cout << "URI needs to be redirected: " << *redir << std::endl;
		for ( size_t i = 0; i < loc.size() ; i++ )
		{
			// std::cout << "loc path: " << loc[i].get_path() << std::endl;
			if ( _envs["REQUEST_URI"] == loc[i].get_path() )
			{
				std::string 	newPath = loc[i].get_redirectionPath();
				unsigned int	redirectionCode = loc[i].get_redirectionCode();
				std::string 	redirectionMsg = _config->create_map_return_codes()[redirectionCode];
				// std::cout << "URI: " << _envs["REQUEST_URI"] << " will be redirected to: " << newPath << "!" << std::endl;
				// std::cout << "Code: " << redirectionCode << std::endl;
				// std::cout << "Reditr MEssage: " << redirectionMsg << std::endl;
				std::string 	header = "HTTP/1.1 " + redirectionMsg + "\nLocation: " + newPath;

				send(_clientSocket, header.c_str(), header.size(), 0);
				return ( true );
			}
		}
	}
	return ( false );
}

bool	HandleData::isNotFound()
{
	std::ifstream	file;

	std::string	path = _config->get_root() + _envs["REQUEST_URI"];
	// std::cout << "path: " << path << std::endl;
	file.open( path.c_str() );
	if (file.is_open())
	{
		file.close();
		return ( false );
	}
	return ( true );
}

void	HandleData::dataRouter()
{
	string	requestURI;
	string	mimeType;
	int		status = 42;



	if (isRedirection())
	{
		// std::cout << "Send to redirection" << std:: endl;
		if (sendRedirection())
		{
			// std::cout << "Redirection Done!" << std::endl;
			return ;
		}
	}
	if (_body.find("_method=delete") != string::npos)
		_envs["REQUEST_METHOD"] = "DELETE";
	if ( has_forbidden(_envs["REQUEST_METHOD"], _envs["REQUEST_URI"], _config->get_locations()))
	{
		_config->sendError(_clientSocket, 405);
		return ;
	}
	if ( _contentLength > _config->get_max_client_body_size() )
	{
		cerr << "Error: Body too large\n";
		_config->sendError(_clientSocket, 413);
		return ;
	}
	if ((requestURI = getRequestURI()) != "EOF") {
		if ((mimeType = getMimeType(requestURI)) != "EOF") {
			// std::cout << "Mimetype: " << mimeType << std::endl;
		}
	} else {
		std::cerr << "RequestURI not found" << std::endl;
	}
	if ( !_isCgi && _envs["REQUEST_METHOD"] == "GET" && isNotFound() )
	{
		// std::cout << "404 config\n";
		_config->sendError(_clientSocket, 404);
		return ;
	}
	if (_isDir) {
		_webSender.sendDirData(_clientSocket, mimeType, requestURI, _workDir);
		// CGI
	} else if (_isCgi){
		HandleCgi	webCgi(_envs, _body, _config->get_root());

		if (!(status = webCgi.executeCgi())) {
			if (_webSender.sendData(webCgi.getCgi(), _clientSocket)) {
				cerr << "Error: CGI\n";
				_config->sendError(_clientSocket, 500);
			}
		} else {
			std::cerr << "Error: CGI failed\n";
			_config->sendError(_clientSocket, status);
		}
	} else {
		// GET
		if (_envs["REQUEST_METHOD"] == "GET") {
			_webSender.sendBinaryData(_clientSocket, mimeType, requestURI, _workDir);
		// DELETE
		} else if (_envs["REQUEST_METHOD"] == "DELETE") {
			Response	httpResponse(_envs, _body, _config->get_root());

			if (!(status = httpResponse.handle())) {
				if (_webSender.sendData(httpResponse.getResponse(), _clientSocket)) {
					cerr << "Error: DELETE\n";
					_config->sendError(_clientSocket, 500);
				}
			} else {
				cerr << "Error: DELETE failed\n";
				_config->sendError(_clientSocket, status);
			}
		// POST
		} else if (_envs["REQUEST_METHOD"] == "POST") {
			Response	httpResponse(_envs, _body, _config->get_root());
			// POST UPLOAD
			if (_boundary && !(status = httpResponse.upload())) {
				if (_webSender.sendData(httpResponse.getResponse(), _clientSocket)) {
					cerr << "Error: POST upload\n";
					_config->sendError(_clientSocket, 500);
				}
			// POST FORM
			} else if (!_boundary && !(status = httpResponse.handle())) {
				if (_webSender.sendData(httpResponse.getResponse(), _clientSocket)) {
						cerr << "Error: POST form\n";
						_config->sendError(_clientSocket, 500);
				}
			} else {
					cerr << "Error: POST failed\n";
					_config->sendError(_clientSocket, status);
			}
		} else {
			std::cerr << "Error: Unknown request method\n";//
			_config->sendError(_clientSocket, 400);
		}
	}
}


bool	HandleData::parseRequest(string& requestData) {
	vector<string>	request = splitRequest(requestData, "\r\n\r\n");
	vector<string>	requestHeaders = splitRequest(request[0], "\r\n");

	// Checking request header compatibility with HTTP 1.1
	if (headersToEnvs(requestHeaders)) {
		cerr << "Error: Invalid HTTP request\n";
		return (1);
	}
	// cout << "--REQUEST DATA--\n" << requestData << "\n";
	// cout << "--ENVS--\n";
	// for (map<string, string>::iterator it = _envs.begin(); it != _envs.end(); ++it) {
	//     cout << it->first << " = " << it->second << "\n";
	// }
	// Checking if request has CL and boundary
	if (!_envs["CONTENT_LENGTH"].empty()) {
		if (_envs["CONTENT_TYPE"].find("boundary=") != string::npos) {
			_boundary = 1;
		}
		_body.assign(requestData.substr(request[0].length() + 4));
		_contentLength = std::strtoul(_envs["CONTENT_LENGTH"].c_str(), NULL, 10);
	}
	return (0);
}

bool	HandleData::headersToEnvs(vector<string>& headers) {
	vector<string>	firstLine = splitRequest(headers[0], " ");
	string			key, val;

	// Checking request's first line
	if (firstLine.size() != 3) {
		return (1);
	}
	_envs.insert(make_pair("REQUEST_METHOD", firstLine[0]));
	_envs.insert(make_pair("REQUEST_URI", firstLine[1]));
	_envs.insert(make_pair("SERVER_PROTOCOL", firstLine[2]));
	// Removing extra "/" from URI and if URI is just "/" replacing it with index
	if (_envs["REQUEST_URI"].find("/") != string::npos) {
		if (_envs["REQUEST_URI"].size() > 1) {
			while (_envs["REQUEST_URI"][_envs["REQUEST_URI"].size() - 1] == '/' && _envs["REQUEST_URI"].size() > 1) {
				_envs["REQUEST_URI"] = _envs["REQUEST_URI"].substr(0, _envs["REQUEST_URI"].size() - 1);
			}
		} else {
			_envs["REQUEST_URI"] = "/" + _config->get_index();
		}
	}
	// Getting remaining headers to envs
	for (size_t i = 1; i < headers.size(); i++) {
		if (addToEnvs(splitRequest(headers[i], ": "))) {
			return (1);
		}
	}
	return (0);
}

void	HandleData::addToBody(std::string& requestData) {
		_body.append(requestData);
}

bool	HandleData::addToEnvs(vector<string> strVec) {
	string	key, val;

	// Host is needed for a valid HTTP 1.1 request
	if (strVec.size() != 2) {
		return(1);
	}
	key.assign(strVec[0]);
	val.assign(strVec[1]);
	// Request to Env name "-" to "_", "abc" to "ABC", and "HTTP_" prefix on most
	for (size_t i = 0; i < key.size(); i++) {
		if (key[i] == '-') {
			key[i] = '_';
		} else {
			key[i] = toupper(key[i]);
		}
	}
	if (key != "CONTENT_LENGTH" && key != "CONTENT_TYPE") {
			key.insert(0, "HTTP_");
	}
	_envs.insert(make_pair(key, val));
	return (0);
}

string	HandleData::getMimeType(string& requestURI) {
	size_t posDot = requestURI.find_last_of(".");
	string ext = requestURI.substr(posDot + 1);
	// std::cout << "ext: " << ext << std::endl;
	std::map<string, string>::iterator mimeIt = _mime.find(ext);

	if (mimeIt != _mime.end()) {
		return (mimeIt->second);
	} else {
		return ("Content-type: text/html");//
	}
	return ("DIR");
}

string	HandleData::getRequestURI() {
	std::map<string, string>::iterator reqIt = _envs.find("REQUEST_URI");

	if (reqIt != _envs.end()) {
		if (_webSender.checkIsDir(reqIt->second, _workDir) && reqIt->second != "/") {
			_isDir = true;
		} else {
			_isDir = false;
		}
		if (reqIt->second.find("/cgi-bin/") != string::npos) {
			_isCgi = true;
		} else {
			_isCgi = false;
		}
		if (reqIt !=_envs.end()) {
			return (reqIt->second);
		}
	}
	return ("EOF");
}

void	HandleData::clearObj() {
	_envs.clear();
	_readTotal = 0;
	_contentLength = 0;
	_body.clear();
	_isCgi = 0;
	_isDir = 0;
	_workDir = _config->get_root();
}

bool	HandleData::readFromSocket() {
	vector<char>	buffer(REQUEST_BUFFER_SIZE);
	string			requestData;
	ssize_t bytesRead = recv(_clientSocket, buffer.data(), REQUEST_BUFFER_SIZE, 0);
	if (!bytesRead) {
		// cout << "Read 0\n";
		clearObj();
		return (1);
	} else if (bytesRead < 0) {
		// cout << "Read -1\n";
		clearObj();
		return (1);
	} else {
		// cout << "Read > 0\n";
		requestData.assign(buffer.begin(), buffer.begin() + bytesRead);
		_readTotal += bytesRead;
		// cout << "Read " << bytesRead << " Total read " << _readTotal << "\n";
		if (_readTotal <= REQUEST_BUFFER_SIZE && _envs.empty()) {
			if (parseRequest(requestData)) {
				clearObj();
				return (1);
			} else {
				return(readyToSend());
			}
		} else if (_contentLength && _body.size() + bytesRead <= _contentLength){
			addToBody(requestData);
			return(readyToSend());
		} else {
			cerr << "Error: recv: Unknown read\n";
			clearObj();
			return (1);
		}
	}
}

bool	HandleData::readyToSend() {
	// cout << "ready to send\nbody.size=" << _body.size() << " contentL=" << _contentLength << "\n";
	if (!_contentLength || _body.size() == _contentLength ) {
		dataRouter();
		clearObj();
		return (1);
	} else {
		return (0);
	}
}

vector<string>	HandleData::splitRequest(const string& str, string delimiter) {
	vector<string>	ret;
	size_t			start = 0;
	size_t			end = str.find(delimiter);

	while (end != string::npos) {
		ret.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}
	ret.push_back(str.substr(start));
	return (ret);
}

size_t	HandleData::get_contentLength() {
	return ( this->_contentLength );
}
