#include "handler.h"
#include "http_response.h"
#include "http_request.h"
#include "response_parser.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <istream>
#include <ostream>
#include <boost/asio.hpp>


// Dynamic handler creation code
std::map<std::string, RequestHandler* (*)(void)>* request_handler_builders = nullptr;

RequestHandler* RequestHandler::CreateByName(const char* type) {
  const auto type_and_builder = request_handler_builders->find(type);
  if (type_and_builder == request_handler_builders->end()) {
    return nullptr;
  }
  return (*type_and_builder->second)();
}

//////////////////////////////////////////////////////////////////////////////////////
////////////////////////////  Not Found Handler  /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

RequestHandler::Status NotFoundHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return RequestHandler::PASS;
}

RequestHandler::Status NotFoundHandler::HandleRequest(const Request& request, Response* response){
    std::cout << "\nNotFoundHandler::HandleRequest" << std::endl;

    std::string body = "404 NOT FOUND";
    response->SetStatus(Response::NOT_FOUND);
    response->AddHeader("Content-Length", std::to_string(body.size()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(body);
    return RequestHandler::PASS;
}

//////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Echo Handler /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

RequestHandler::Status EchoHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return RequestHandler::PASS;
}

RequestHandler::Status EchoHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nEchoHandler::HandleRequest" << std::endl;

    response->SetStatus(Response::OK);
    response->AddHeader("Content-Length", std::to_string(request.raw_request().size() - 4));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(request.raw_request());
    return RequestHandler::PASS;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Static Handler /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

// Sets dir_from_config to directory specified after root in config
RequestHandler::Status StaticHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    for (unsigned int i = 0; i < config.statements_.size(); i++){
        // get the root from the child block
        if (config.statements_[i]->tokens_[0] == "root" && config.statements_[i]->tokens_.size() == 2){
            serve_path = config.statements_[i]->tokens_[1];
        }
    }

    this->uri_prefix = uri_prefix;

    //Code to add the / between the file path and serve_path properly
    if (serve_path.length() != 0 && serve_path[serve_path.length()-1] != '/'){
        if (uri_prefix.length() != 0 && uri_prefix[uri_prefix.length()-1] == '/'){
            serve_path += '/';
        }
    }else{
        if (uri_prefix.length() != 0 && uri_prefix[uri_prefix.length()-1] != '/'){
            if (serve_path.length() != 0){
                serve_path = serve_path.substr(0, serve_path.length() - 1);
            }
        }
    }

    return RequestHandler::PASS;
}

// Attempts to serve file that was requested by the request object
// If not possible, we return fail signal for outside class to handle
// calling not found handler
RequestHandler::Status StaticHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nStaticHandler::HandleRequest" << std::endl;

    std::string abs_path = serve_path;

    //get the file name/path from the request url
    std::string file_path = request.uri().substr(uri_prefix.length());
    abs_path += file_path;

    if(!is_regular_file(abs_path.c_str())) {
      std::cerr << "Error: " << abs_path << " does not exist" << std::endl;
      return RequestHandler::FAIL;
    }

    // save content_type header based on requested file extension
    std::string content_type = get_content_type(abs_path);

    // raw byte array
    std::string to_send = read_file(abs_path);
    std::cout << "Serving file from: " << abs_path << std::endl;

    response->SetStatus(Response::OK);
    response->AddHeader("Content-Length", std::to_string(to_send.size()));
    response->AddHeader("Content-Type", content_type);
    response->SetBody(to_send);
    return RequestHandler::PASS;
}

// checks if file exists and is regular file
bool StaticHandler::is_regular_file(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}


// gets content-type based on the file extension of requested file
std::string StaticHandler::get_content_type(std::string filename) {
    unsigned int i;

    // search for either last period or last slash in filename
    for (i = filename.size() - 1; i >= 0; i--) {
        // file with no extension type
        if (filename[i] == '/') {
            return "text/plain";
        } else if (filename[i] == '.') { // found pos of beginning of extension
            break;
        }
    }

    std::string ext = filename.substr(i + 1, std::string::npos);
    std::string content_type;

    // based on ext decide content_type header
    if (ext == "html") {
        content_type = "text/html";
    } else if (ext == "jpg") {
        content_type = "image/jpeg";
    } else if (ext == "pdf") {
        content_type = "application/pdf";
    } else {
        content_type = "text/plain";
    }
    return content_type;
}

// reads raw file into vector of characters
std::string StaticHandler::read_file(std::string filename) {
    std::ifstream ifs(filename, std::ios::binary|std::ios::ate);
    std::ifstream::pos_type pos = ifs.tellg();

    std::vector<char> result(pos);

    ifs.seekg(0, std::ios::beg);
    ifs.read(&result[0], pos);

    std::string str_rep(result.begin(), result.end());

    return str_rep;
}


//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Status Handler /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////


RequestHandler::Status StatusHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {
    return RequestHandler::PASS;
}

//reads the log file of handled requests and their corresponding responses and fills a map with them
bool StatusHandler::addHandledRequests() {
    std::ifstream infile("request_response_log.txt");

    if (!infile.is_open())
        return false;

    std::string request_url;
    int response_code;
    while (infile >> request_url >> response_code)
    {
        map_of_request_and_responses[request_url].push_back(response_code);
    }

    infile.close();

    return true; 
}

//reads the log file of handler names and their corresponding uri's and fills a map with them
bool StatusHandler::addHandlerMapping() {
    std::ifstream infile("handler_names.txt");

    if (!infile.is_open()) 
        return false;

    std::string name, url;
    while (infile >> name >> url)
    {
        map_of_uri_to_handler[name] = url;
    }

    infile.close();

    return true;
}

//Adds the handler names mapping and handler response to reqeust mapping that were created above
//to the message response body and creates the response object.
RequestHandler::Status StatusHandler::HandleRequest(const Request& request, Response* response) {
    if (!addHandledRequests() || !addHandlerMapping()){
        return RequestHandler::FAIL;
    }

    std::cout << "\nStatusHandler::HandleRequest" << std::endl;

    std::string to_send;

    to_send += "These are the handlers available and their URIs:\n";
    for (auto const& x : map_of_uri_to_handler){
        to_send += x.first;
        to_send += " -> ";
        to_send += x.second;
        to_send += "\n";
    }

    to_send += "\nThese are the requests received and their corresponding response codes\n";
    for (auto const& x : map_of_request_and_responses){
        for(auto const& c: x.second){
            to_send += x.first;
            to_send += " -> ";
            to_send += std::to_string(c);
            to_send += "\n";
        }

    }
    response->SetStatus(Response::OK);
    response->AddHeader("Content-Length", std::to_string(to_send.length()));
    response->AddHeader("Content-Type", "text/plain");
    response->SetBody(to_send);

    map_of_uri_to_handler.clear();
    map_of_request_and_responses.clear();

    return RequestHandler::PASS;
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Proxy Handler /////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

RequestHandler::Status ProxyHandler::Init(const std::string& uri_prefix, const NginxConfig& config) {

    for (unsigned int i = 0; i < config.statements_.size(); i++){
        // get the root from the child block
        if (config.statements_[i]->tokens_[0] == "host" && config.statements_[i]->tokens_.size() == 2){
            host = config.statements_[i]->tokens_[1];
        }
        else if (config.statements_[i]->tokens_[0] == "port" && config.statements_[i]->tokens_.size() == 2){
            port = config.statements_[i]->tokens_[1];
        }
    }
    std::cout << "host: " << host << std::endl;
    std::cout << "port: " << port << std::endl;

    this->uri_prefix = uri_prefix;

    return RequestHandler::PASS;
}

std::string ProxyHandler::get_response(std::string path)
{

  using boost::asio::ip::tcp;

  try
  {

    boost::asio::io_service io_service;

    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);


    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    std::string request_str = "GET " + path + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" 
    + "Accept: */*\r\n" + "Connection: keep-alive\r\n\r\n";
    request_stream << request_str;

    std::cout << "REQUEST: " << path << std::endl;
    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line. The response streambuf will automatically
    // grow to accommodate the entire line. The growth may be limited by passing
    // a maximum size to the streambuf constructor.
    boost::asio::streambuf response;

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Write whatever content we already have to output.
    std::string s = "";
    if (response.size() > 0){
        std::string a( (std::istreambuf_iterator<char>(&response)), std::istreambuf_iterator<char>() );
        s += a;
    }

    boost::system::error_code error;

    if (!response_parser.Parse(s) == ResponseParser::ParseStatus::OK) {
        std::cout << "Response parsing error!\n";
    }   // Handle redirect
    else if (response_parser.getStatus() == 302 && !response_parser.getRedirectPath().empty()) {
        
        // Assume redirect location has format "http://www.something.com/"
        std::string parsed_redirect = response_parser.getRedirectPath();
        parsed_redirect = parsed_redirect.substr(parsed_redirect.find("w"));

        // Want to set host to "www.something.com" without the last slash
        size_t first_slash = parsed_redirect.find("/");

        std::string host_ = parsed_redirect.substr(0, first_slash);
        host_ = host_.substr(0, host_.length());
        std::cout << "HOST: " << host_ << std::endl;

        // Get a list of endpoints corresponding to the server name.
        tcp::resolver::query new_query(host_, "http");
        endpoint_iterator = resolver.resolve(new_query);

        // Try each endpoint until we successfully establish a connection.
        boost::asio::connect(socket, endpoint_iterator);

        // Construct new request with the proper host
        std::string new_request_str = "GET " + path + " HTTP/1.1\r\n" + "Host: " + host_ + "\r\n" 
        + "Accept: */*\r\n" + "Connection: keep-alive\r\n\r\n";

        boost::asio::streambuf new_request;
        std::ostream new_request_stream(&new_request);
        new_request_stream << new_request_str;

        boost::asio::write(socket, new_request);

        boost::asio::streambuf response_redirect;
        // Read the response headers, which are terminated by a blank line.
        // boost::asio::read_until(socket, response_redirect, "\r\n\r\n");

        // // Write whatever content we already have to output.
        s = "";
        // if (response_redirect.size() > 0){
        //     std::string a( (std::istreambuf_iterator<char>(&response_redirect)), std::istreambuf_iterator<char>() );
        //     s += a;
        // }

            // Read until EOF, writing data to output as we go.
        while (boost::asio::read(socket, response_redirect,
              boost::asio::transfer_at_least(1), error)){
            std::string a( (std::istreambuf_iterator<char>(&response_redirect)), std::istreambuf_iterator<char>() );
            s += a;
        }
    }
    else { // Request returned 200

        response_parser.emptyVector();
        // Read until EOF, writing data to output as we go.
        while (boost::asio::read(socket, response,
              boost::asio::transfer_at_least(1), error)){
            std::string a( (std::istreambuf_iterator<char>(&response)), std::istreambuf_iterator<char>() );
            s += a;
        }
    }

    if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);

    return s;

  }
  catch (std::exception& e)
  {
    std::cout << "Exception: " << e.what() << "\n";
  }

  return "";
}

RequestHandler::Status ProxyHandler::HandleRequest(const Request& request, Response* response) {
    std::cout << "\nProxyHandler::HandleRequest" << std::endl;

    response->SetStatus(Response::OK);

    // Pass in the request uri
    std::string request_uri = request.uri();
    std::string response_string = get_response(request_uri);
    // size_t content_index = response_string.find("Content-Type: ");
    // std::string content_sub = response_string.substr(content_index);
    // size_t end_row = content_sub.find("/");
    // content_sub = content_sub.substr(0, end_row);

    // if (content_sub == "image"){
    //     size_t end_index = response_string.find("\r\n\r\n");
    //     response->SetBody(response_string.substr(end_index + 4));
    // }
    // std::cout << response_string << std::endl; 

    response_parser.Parse(response_string);

    response->AddHeader("Content-Length", std::to_string(response_string.length()));

    auto header_vec = response_parser.getHeaders();

    for(auto i = header_vec.begin(); i != header_vec.end(); i++) {
        if((*i).first == "Content-Type") {
            size_t first_slash = (*i).second.find_first_of("/"); 
            if((*i).second.substr(0, first_slash) == "image"){
                size_t end_index = response_string.find("\r\n\r\n");
                response->SetBody(response_string.substr(end_index + 4));
                break;
            }
        }
        if((*i).first == "Content") {
            response->SetBody((*i).second); 
            break;
        }
        //std::cout << "Name: " << (*i).first << " " << "Value: " <<  (*i).second << std::endl;
        response->AddHeader((*i).first, (*i).second);
        //res_str += (*it).first + ": " + (*it).second + "\r\n";
    }

    // response->AddHeader("Content-Type", response_parser.getContentType());
    //response->SetBody(response_parser.getResponseBody());
    //response->SetBody();
    response_parser.emptyVector();

    return RequestHandler::PASS;
}