#include "../includes/Response.hpp"

Response::Response() {}

Response::Response(Response const& other) {
	*this = other;
	return ;
}

Response::Response(map<string, string>& envs, string& body, string root) : _envs(envs), _body(body), _root(root){
	_respOutput.clear();
}

Response::~Response() {
}

Response&	Response::operator=(Response const& other) {
	if (this != &other) {
		this->_envs = other._envs;
		this->_respOutput = other._respOutput;
		this->_body = other._body;
		this->_root = other._root;
	}
	return *this;
}

string&	Response::getResponse() {
	return (_respOutput);
}

int Response::handle() {
	if (_body.empty()) {
		cerr << "Error: Body is empty\n";
		return (400);
	}
	if (_envs["REQUEST_METHOD"] == "POST") {
		if (_body.empty()) {
			cerr << "Error: Body is empty\n";
			return (400);
		}
		ofstream	outputFile("./form.txt", std::ios::app);
		if (outputFile.is_open()) {
			outputFile << decode(_body);
			outputFile << "\n";
			outputFile.close();
			_respOutput = "HTTP/1.1 201 Created\r\n" + styleStr("Form data added!");
		} else {
			cerr << "Error: Opening file\n";
			return (500);
		}
	} else if (_envs["REQUEST_METHOD"] == "DELETE") {
		int		posDel = _body.find("resourceId=") + 11;
		string	toDel = "./" + _root + "/assets/" + decode(_body.substr(posDel));
		char	*fileName = strdup(toDel.c_str());
		if (!fileExists(fileName)) {
			cerr << "Error: File does not exist\n";
			free (fileName);
			return (404);
		}
		else if (remove(fileName) == 0) {
			cout << "File '" << toDel << "' deleted successfully\n";
			_respOutput = "HTTP/1.1 200 OK\r\n" + styleStr("File deleted!");
		} else {
			cerr << "Error: Deleting file\n";
			free (fileName);
			return (500);
		}
		free (fileName);
	} else {
		cerr << "Error: Invalid POST request\n";
		return (400);
	}
	return (0);
}

int	Response::upload() {
	if (_envs["CONTENT_TYPE"].find("multipart/form-data") == string::npos)
		return (400);
	size_t	start = _body.find("filename=") + 10;
	size_t	end = _body.find("\nContent-Type:") - start - 2;
	string	fileName = _root + "/assets/" + _body.substr(start, end);
	if (!isDir((_root + "/assets").c_str())) {
		cerr << "Error: Directory " << _root << "/assets/ missing!\n";
		return (500);
	} else if (isDir(fileName.c_str())) {
		cerr << "Error: Empty file field\n";
		return (400);
	} else if (fileExists(fileName.c_str())) {
		cerr << "Error: A file with that name already exists\n";
		size_t	pos;
		if ((pos = fileName.find_last_of(".")) == string::npos || fileName.size() - pos > 5) {
			pos = fileName.size();
		}
		fileName.insert(pos, "(1)");
		size_t i = 1;
		while (fileExists(fileName.c_str())) {
			i++;
			string	firstPart = fileName.substr(0, fileName.find_last_of("(") + 1);
			string	lastPart = fileName.substr(fileName.find_last_of(")"));
			fileName = firstPart + toString(i) + lastPart;
		}
	}
	ofstream	outputFile(fileName.c_str());
	if (outputFile.is_open()) {
		outputFile << removeLines(_body);
		outputFile.close();
		_respOutput = "HTTP/1.1 201 Created\r\n" + styleStr("File uploaded!\n");
	} else {
		cerr << "Error: Opening file\n";
		return (500);
	}
	return (0);
}

// Removes boundaries and header from POST body for non cgi upload
string	Response::removeLines(string& input) {
	std::istringstream iss(input);
	std::vector<std::string> lines;

	std::string line;
	while (std::getline(iss, line)) {
		lines.push_back(line);
	}
	if (lines.size() >= 5) {
		lines.erase(lines.begin(), lines.begin() + 4);

		lines.erase(lines.end() - 1);
	} else {
		std::cerr << "Error: Not enough lines to remove\n";
	}
	std::string result;
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		result += *it + "\n";
	}
	return (result);
}

// Adds html and css to str or if template file doesn't exist, adds simple html
string	Response::styleStr(string str) {
	string	ret = "Content-type: text/html\n\n<html><body>\n<h2>" + str + "</h2>\n</body></html>";
	string	ret2;
	std::ifstream file((_root + "/template.html").c_str());

	if (!file.is_open()) {
		return ret;
	}
	std::ostringstream buffer;
	buffer << file.rdbuf();
	ret2 = buffer.str() + "<body>\n<h2>" + str + "</h2></body></html>";
	return (ret2);
}
