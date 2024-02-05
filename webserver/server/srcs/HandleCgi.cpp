#include "../includes/HandleCgi.hpp"

HandleCgi::HandleCgi() {}

HandleCgi::HandleCgi(HandleCgi const& other) {
	*this = other;
	return ;
}

HandleCgi::HandleCgi(map<string, string>& envs, string& body, string root) : _envs(envs), _body(body), _root(root){
	_timeout = std::time(NULL);
	envsToCgi();
	_cgiEnvsChar = mapToCArray();
	_cgiOutput.clear();
	// map<string, string>::iterator	it;
	// for ( it = _envs.begin(); it != _envs.end(); ++it)
	// 	cout << "env: " << it->first << "=" << it->second << "\n";
	// for ( it = _cgiEnvs.begin(); it != _cgiEnvs.end(); ++it)
	// 	cout << "cgiEnv: " << it->first << "=" << it->second << "\n";
	// for ( int i = 0; _cgiEnvsChar[i]; i++ )
	// 	cout << "cgiEnv: " << _cgiEnvsChar[i] << std::endl;
}

HandleCgi::~HandleCgi() {}

HandleCgi&	HandleCgi::operator=(HandleCgi const& other) {
	if (this != &other) {
		this->_timeout = other._timeout;
		this->_cgiPid = other._cgiPid;
		this->_toCgi[0] = other._toCgi[0];
		this->_toCgi[1] = other._toCgi[1];
		this->_fromCgi[0] = other._fromCgi[0];
		this->_fromCgi[1] = other._fromCgi[1];
		this->_cgiEnvsChar = other._cgiEnvsChar;
		this->_cgiEnvs = other._cgiEnvs;
		this->_cgiOutput = other._cgiOutput;
		this->_scriptPath = other._scriptPath;
		this->_envs = other._envs;
		this->_body = other._body;
		this->_root = other._root;
	}
	return *this;
}

void	HandleCgi::envsToCgi() {
	string		uri = _envs["REQUEST_URI"];
	size_t		pos = 0;
	size_t		cgiPos = uri.find("/cgi-bin/") + 9;
	size_t		pathPos = uri.find("/", cgiPos);
	size_t		queryPos = uri.find("?", cgiPos);

	if (_envs["REQUEST_METHOD"] == "GET") {
		// GET with QUERY STRING Check if there's a query after "?"
		if (queryPos != string::npos && uri.size() > queryPos + 1) {
			_scriptPath = uri.substr(0, queryPos);
			pos = queryPos;
			_cgiEnvs["QUERY_STRING"] = decode(uri.substr(queryPos + 1));
		}
		// GET with PATH INFO Check if there's a path after "/""
		if (pathPos != string::npos && uri.size() > pathPos + 1) {
			_scriptPath = uri.substr(0, pathPos);
			pos = pathPos;
			if (queryPos == string::npos) {
				_cgiEnvs["PATH_INFO"] = decode(uri.substr(pathPos + 1));
			} else {
				_cgiEnvs["PATH_INFO"] = decode(uri.substr(pathPos, queryPos - pathPos));
			}
			// GET
		} else if (uri.size() > cgiPos + 1 && !pos) {
			_scriptPath = uri;
			pos = _scriptPath.length() - cgiPos;
		} else if (!pos) {
			cerr << "Error: Invalid CGI request\n";
			return ;
		}
	} else if (_envs["REQUEST_METHOD"] == "POST") {
		if (pathPos != string::npos && uri.size() > pathPos + 1) {
			_cgiEnvs["PATH_INFO"] = decode(uri.substr(pathPos + 1));
		}
		_scriptPath = uri.substr(0, uri.find(".py") + 3);
		pos = _scriptPath.length() - cgiPos;
	}
	_cgiEnvs["SCRIPT_NAME"] = _scriptPath;
	_cgiEnvs["SCRIPT_FILENAME"] = _scriptPath.substr(cgiPos, pos);
	_cgiEnvs["REQUEST_METHOD"] = _envs["REQUEST_METHOD"];
	_cgiEnvs["HTTP_HOST"] = _envs["HTTP_HOST"];
	_cgiEnvs["HTTP_USER_AGENT"] = _envs["HTTP_USER_AGENT"];
	_cgiEnvs["HTTP_REFERER"] = _envs["HTTP_REFERER"];
	_cgiEnvs["CONTENT_LENGTH"] = _envs["CONTENT_LENGTH"];
	_cgiEnvs["CONTENT_TYPE"] = _envs["CONTENT_TYPE"];
	_cgiEnvs["REQUEST_URI"] = _envs["REQUEST_URI"];
	_cgiEnvs["SERVER_PROTOCOL"] = _envs["SERVER_PROTOCOL"];
	_cgiEnvs["GATEWAY_INTERFACE"] = "CGI/1.1";
	_cgiEnvs["SERVER_PORT"] = _envs["HTTP_HOST"].substr(_envs["HTTP_HOST"].find(":") + 1);
	_cgiEnvs["SERVER_SOFTWARE"] = "42-WEBSERV";
	_cgiEnvs["DOCUMENT_ROOT"] = _root;
}

char**	HandleCgi::mapToCArray()
{
	map<string, string>::iterator	it;
	char							**tmp = new char*[_cgiEnvs.size() + 1];
	int								i = 0;

	for ( it = _cgiEnvs.begin(); it != _cgiEnvs.end(); it ++ ) {
		string	var = it->first + "=" + it->second;
		char	*toCStr = new char[var.size() + 1];
		strcpy(toCStr, var.c_str());
		tmp[i++] = toCStr;
	}
	tmp[i] = 0;
	return (tmp);
}

int	HandleCgi::executeCgi() {
	// Envs check
	if (_cgiEnvs.size() < 14 || !_cgiEnvsChar) {
		cerr << "Error: CGI Envs check failed\n";
		return (500);
	}
	// "./ for execution"
	string	tmp = "." + _scriptPath;
	char**	av = new char*[2];
	av[0] = strdup(tmp.c_str());
	av[1] = NULL;
	// Script checks
	if (!fileExists(av[0])) {
		cerr << "Error: Script file does not exist\n";
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
		return (400);
	} else if (isDir(av[0])) {
		cerr << "Error: Script path is a directory\n";
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
		return (400);
	} else if (!isExe(av[0])) {
		cerr << "Error: Script doesn't have permission to execute\n";
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
		return (403);
	} else 	if (!(ifstream(av[0]).good())) {
		cerr << "Error: Script can't be opened\n";
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
	 	return (500);
	}
	if (pipe(_toCgi) == -1 || pipe(_fromCgi) == -1) {
		cerr << "Error: Pipe: " << strerror(errno) << "\n";
		return (500);
	}
	_cgiPid = fork();
	if (_cgiPid == -1) {
		cerr << "Error: Fork: " << strerror(errno) << "\n";
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
		close(_toCgi[READ]);
		close(_toCgi[WRITE]);
		close(_fromCgi[READ]);
		close(_fromCgi[WRITE]);
		return (500);
	// Child process
	} else if (!_cgiPid) {
		close(_fromCgi[READ]);
		close(_toCgi[WRITE]);
		dup2(_toCgi[READ], STDIN_FILENO);
		dup2(_fromCgi[WRITE], STDOUT_FILENO);
		if (execve(av[0], av, _cgiEnvsChar) == -1) {
			cerr << "Error: Execve: " << strerror(errno) << "\n";
			close(_toCgi[READ]);
			close(_fromCgi[WRITE]);
			std::exit (EXIT_FAILURE);
		}
	// Parent process
	} else {
		int	status;
		std::vector<char>	_cgiBuffer(CGI_BUFFER_SIZE);

		close(_fromCgi[WRITE]);
		close(_toCgi[READ]);
		if (write(_toCgi[WRITE], _body.c_str(), _body.size()) == -1) {
			close(_fromCgi[READ]);
			close(_toCgi[WRITE]);
			deleteCArray(_cgiEnvsChar);
			free (av[0]);
			delete[] av;
			return (500);
		}
		_cgiOutput.clear();
		while (1) {
			_cgiBuffer.clear();
			ssize_t	readBytes = read(_fromCgi[READ], _cgiBuffer.data(), CGI_BUFFER_SIZE);
			// Timeout
			if (std::time(NULL) - _timeout >= CGI_TIMEOUT) {
				kill(_cgiPid, SIGTERM);
				cerr << "CGI timed out\n";
				_cgiOutput.clear();
				break;
			}
			if (readBytes > 0) {
				_cgiOutput.append(_cgiBuffer.data(), readBytes);
			} else if (!readBytes) {
				break;
			} else {
				cerr << "Error: Read from CGI\n";
				_cgiOutput.clear();
				break;
			}
		}
		if (waitpid(_cgiPid, &status, 0) == -1) {
			cerr << "Error: waitpid" << "\n";
			_cgiOutput.clear();
			deleteCArray(_cgiEnvsChar);
			free (av[0]);
			delete[] av;
			close(_fromCgi[READ]);
			close(_toCgi[WRITE]);
			return (500);
		}
		close(_fromCgi[READ]);
		close(_toCgi[WRITE]);
		if (WIFEXITED(status)) {
			;
		} else if (WIFSIGNALED(status)) {
			cerr << "Error: CGI Script was terminated\n";
			_cgiOutput.clear();
			deleteCArray(_cgiEnvsChar);
			free (av[0]);
			delete[] av;
			return (508);
		} else {
			cerr << "Error: CGI Script did not exit normally\n";
			_cgiOutput.clear();
			deleteCArray(_cgiEnvsChar);
			free (av[0]);
			delete[] av;
			return (500);
		}
		deleteCArray(_cgiEnvsChar);
		free (av[0]);
		delete[] av;
	}
	return (0);
}

string&	HandleCgi::getCgi(){
	return _cgiOutput;
}

void	HandleCgi::deleteCArray(char **arr) {
	for ( int i = 0; arr[i]; i++ )
		delete[] arr[i];
	delete[] arr;
}
