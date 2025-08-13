# 42 WebServ

A high-performance HTTP/1.1 web server implementation in C++, featuring configuration management, CGI support, request/response handling, and modern web server capabilities.

## 🌐 Overview

WebServ is a complete HTTP server implementation that handles HTTP/1.1 requests, supports multiple server configurations, CGI execution, and provides comprehensive error handling. The project demonstrates advanced C++ programming, network programming, and web server architecture principles.

## ✨ Features

### Core HTTP Server
- **HTTP/1.1 Protocol**: Full HTTP/1.1 specification support
- **Multiple Servers**: Support for multiple server blocks on different ports
- **Request Parsing**: Complete HTTP request parsing and validation
- **Response Generation**: Dynamic HTTP response generation
- **Error Handling**: Comprehensive error pages and status codes

### Configuration Management
- **Configuration Files**: Custom `.conf` file format
- **Server Blocks**: Multiple server configurations
- **Location Blocks**: URL-based routing and configuration
- **Directive Support**: Various server directives and options

### Advanced Features
- **CGI Support**: Common Gateway Interface execution
- **File Serving**: Static file serving with MIME types
- **Directory Listing**: Auto-index functionality
- **Redirection**: HTTP redirect support
- **Upload Support**: File upload handling

### Performance & Reliability
- **Epoll-based I/O**: High-performance event-driven architecture
- **Connection Management**: Efficient client connection handling
- **Timeout Management**: Connection timeout handling
- **Memory Management**: Proper resource allocation and cleanup

## 🏗️ Architecture

### Core Components

#### 1. WebServer Class (`WebServer.hpp/cpp`)
- Main server class managing multiple server instances
- Epoll event loop for high-performance I/O
- Client connection management
- Request routing and handling

#### 2. Config Class (`Config.hpp/cpp`)
- Configuration file parsing and management
- Server and location block handling
- Directive processing and validation
- Configuration inheritance and merging

#### 3. HandleData Class (`HandleData.hpp/cpp`)
- HTTP request parsing and validation
- Request method handling (GET, POST, DELETE)
- Request body processing
- Header management

#### 4. Response Class (`Response.hpp/cpp`)
- HTTP response generation
- Status code handling
- Header construction
- Response body management

#### 5. HandleCgi Class (`HandleCgi.hpp/cpp`)
- CGI script execution
- Environment variable setup
- Process management
- Output handling

### Data Flow

```
Client Request → WebServer → HandleData → Response/CGI → Client
     ↓              ↓           ↓           ↓
  Socket        Epoll      Parsing    Processing
  Accept       Events     Request    Response
```

## 🚀 Building and Running

### Prerequisites
- C++98 compatible compiler (GCC/G++)
- Make
- Linux environment (for epoll)
- Root privileges (for ports < 1024)

### Compilation
```bash
# Clone the repository
git clone <repository-url>
cd 42-webserv

# Build the project
make

# Clean build artifacts
make clean

# Full clean
make fclean

# Rebuild
make re
```

### Usage
```bash
# Run with configuration file
./webserv config/webserv.conf

# Run with default configuration
./webserv default.conf

# Run with custom configuration
./webserv /path/to/your/config.conf
```

## 📋 Configuration

### Configuration File Format
```nginx
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    index index.html index.htm;

    location / {
        allow_methods GET POST DELETE;
        autoindex on;
    }

    location /cgi-bin {
        allow_methods GET POST;
        cgi_path /usr/bin/python3 /usr/bin/php;
        cgi_ext .py .php;
    }

    error_page 404 /404.html;
    client_max_body_size 1000000;
}
```

### Server Directives
- **`listen`**: Port number to listen on
- **`server_name`**: Server name for virtual hosting
- **`root`**: Document root directory
- **`index`**: Default index files
- **`error_page`**: Custom error pages
- **`client_max_body_size`**: Maximum request body size

### Location Directives
- **`allow_methods`**: Allowed HTTP methods
- **`autoindex`**: Enable directory listing
- **`cgi_path`**: CGI interpreter paths
- **`cgi_ext`**: CGI file extensions
- **`return`**: HTTP redirects

## 🔧 Implementation Details

### HTTP Request Handling
```cpp
// Request parsing example
class HandleData {
    std::string method;
    std::string uri;
    std::string version;
    std::map<std::string, std::string> headers;
    std::string body;

    bool parseRequest(const std::string& raw_request);
    bool validateRequest();
};
```

### Response Generation
```cpp
// Response generation example
class Response {
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string generateResponse();
    void setStatus(int code);
    void addHeader(const std::string& key, const std::string& value);
};
```

### CGI Execution
```cpp
// CGI handling example
class HandleCgi {
    std::string script_path;
    std::map<std::string, std::string> env_vars;

    bool executeCgi(const std::string& script, const std::string& input);
    void setupEnvironment();
    std::string getCgiOutput();
};
```

## 📁 Project Structure

```
42-webserv/
├── webserver/              # Main server implementation
│   ├── main.cpp           # Entry point
│   └── server/            # Server core
│       ├── includes/      # Header files
│       │   ├── WebServer.hpp
│       │   ├── Config.hpp
│       │   ├── HandleData.hpp
│       │   ├── Response.hpp
│       │   ├── HandleCgi.hpp
│       │   └── ...
│       └── srcs/          # Source files
│           ├── WebServer.cpp
│           ├── Config.cpp
│           ├── HandleData.cpp
│           ├── Response.cpp
│           ├── HandleCgi.cpp
│           ├── parse.cpp
│           └── ...
├── www/                   # Web root directory
├── confs/                 # Configuration files
├── error/                 # Error pages
├── cgi-bin/              # CGI scripts
├── Makefile              # Build configuration
└── README.md             # Documentation
```

## 🌍 HTTP Methods Supported

### GET
- Static file serving
- Directory listing (autoindex)
- CGI script execution
- Query string processing

### POST
- File uploads
- Form data processing
- CGI script execution
- Request body handling

### DELETE
- File deletion
- Resource removal
- Error handling for non-existent resources

## 🔍 Key Features Explained

### 1. Configuration Parsing
- Custom parser for `.conf` files
- Support for nested blocks (server, location)
- Directive validation and error reporting
- Configuration inheritance

### 2. Request Processing
- Complete HTTP/1.1 request parsing
- Header validation and processing
- Body handling for POST requests
- Query string parsing

### 3. Response Generation
- Dynamic response creation
- Status code management
- Header generation
- Error page serving

### 4. CGI Support
- Process creation and management
- Environment variable setup
- Input/output handling
- Multiple interpreter support

### 5. File Operations
- Static file serving
- MIME type detection
- Directory listing
- File upload handling

## 🧪 Testing

### Manual Testing
```bash
# Test basic GET request
curl http://localhost:8080/

# Test POST request
curl -X POST -d "data=test" http://localhost:8080/form

# Test file upload
curl -X POST -F "file=@test.txt" http://localhost:8080/upload

# Test CGI script
curl http://localhost:8080/cgi-bin/test.py

# Test error pages
curl http://localhost:8080/nonexistent
```

### Automated Testing
```bash
# Run with test configuration
./webserv test.conf

# Test multiple concurrent connections
ab -n 1000 -c 10 http://localhost:8080/

# Test with different browsers
# Open http://localhost:8080/ in various browsers
```

## 🐛 Debugging

### Compilation Flags
```bash
# Debug build
make CFLAGS="-g -Wall -Wextra -Werror -std=c++98 -DDEBUG"

# Valgrind check
valgrind --leak-check=full --show-leak-kinds=all ./webserv config.conf
```

### Common Issues
1. **Port already in use**: Check if port is available
2. **Permission denied**: Run with sudo for ports < 1024
3. **Configuration errors**: Validate config file syntax
4. **CGI execution**: Check interpreter paths and permissions

## 📚 Dependencies

### System Libraries
- **epoll**: Event notification interface
- **socket**: Network communication
- **fcntl**: File control operations
- **unistd**: Process management
- **sys/wait**: Process synchronization

### C++ Features
- **STL containers**: std::map, std::vector, std::string
- **File streams**: std::ifstream, std::ofstream
- **String manipulation**: std::string methods
- **Exception handling**: try-catch blocks

## 🔧 Configuration Examples

### Basic Server
```nginx
server {
    listen 8080;
    server_name localhost;
    root /var/www/html;
    index index.html;

    location / {
        allow_methods GET;
    }
}
```

### CGI Server
```nginx
server {
    listen 8081;
    server_name localhost;
    root /var/www/cgi;

    location /cgi-bin {
        allow_methods GET POST;
        cgi_path /usr/bin/python3;
        cgi_ext .py;
    }
}
```

### File Upload Server
```nginx
server {
    listen 8082;
    server_name localhost;
    root /var/www/upload;
    client_max_body_size 10000000;

    location /upload {
        allow_methods POST;
        upload_dir /var/www/uploads;
    }
}
```

## 🔗 Related Projects

- **42-transcendence**: Web gaming platform
- **42-minishell**: Unix shell implementation

---

*This project demonstrates mastery of C++ programming, network programming, and HTTP protocol implementation while providing a fully functional web server.*
