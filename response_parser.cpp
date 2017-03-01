#include "response_parser.h"
#include <iostream>

ResponseParser::ResponseParser() {
	m_redirect_path = "";
	m_content_type = ""; 
	m_status_code = 0; 
	//m_response = "";
	//m_content_type = "";
}

ResponseParser::ParseStatus ResponseParser::Parse(std::string response) {

	m_headers.clear();

	size_t index = 0;
	index = response.find(" ");
	std::string rest_of_status_line = response.substr(index + 1);

	// Find the response code
	index = rest_of_status_line.find(" ");
	std::string status_code = rest_of_status_line.substr(0, index);

	// Status code should be all digits
	if (!status_code.empty() && is_digits(status_code))
		m_status_code = std::stoi(status_code);
	else 
		return ResponseParser::ParseStatus::NO_STATUS;

	size_t end_index = 0; 
	end_index = response.find("\r\n\r\n"); 
	index = response.find("\r\n");
	std::string rest_of_headers = response.substr(index + 2);

	size_t index2 = rest_of_headers.find("\n");
	size_t counter = index + index2 + 4; 
	while(counter < end_index && index2 != std::string::npos) {
		//find header name 
		size_t first_colon = rest_of_headers.find_first_of(":"); 
		std::string header_name = rest_of_headers.substr(0, first_colon); 

		std::cout << "Header name: " << header_name << std::endl;

		//find header value 
		size_t end_of_row = rest_of_headers.find("\r\n");  
		std::string header_value = rest_of_headers.substr(first_colon + 2, end_of_row - first_colon - 2); 

		std::cout << "Header value: " << header_value << std::endl;
		rest_of_headers = rest_of_headers.substr(end_of_row + 2, end_index);
		index2 = end_of_row;
		counter += index2; 

		if (header_name == "Location") 
			m_redirect_path = header_value;
		else
			m_headers.push_back(std::make_pair(header_name, header_value));

		std::cout << "counter : " << counter << " end_index : " << end_index << std::endl;
	}

	std::string m_response_body = response.substr(end_index + 4);
	m_headers.push_back(std::make_pair("Content", m_response_body));

	return ResponseParser::ParseStatus::OK;	
}

// Function that determines whether every element of a string is a digit
bool ResponseParser::is_digits(const std::string &str) 
{
    return str.find_first_not_of("0123456789") == std::string::npos;
}

// Looks for the end of a line in the response using \r
size_t ResponseParser::find_line_ending(std::string input_string)
{
	for (size_t i = 0; i < input_string.length(); i++)
		if (input_string[i] == '\r' || input_string[i] == ';')
			return i;

	return -1;
}

std::vector<std::pair<std::string, std::string>> ResponseParser::getHeaders() {
	return m_headers;
}