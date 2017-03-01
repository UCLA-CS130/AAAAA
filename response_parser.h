#ifndef RESPONSE_PARSER_H
#define RESPONSE_PARSER_H

#include <string>
#include <vector>
#include <sstream>


class ResponseParser {
public:
	enum ParseStatus {
		OK = 0,
		NO_STATUS = 1,
	};

	ResponseParser();

	ParseStatus Parse(std::string response);

	int getStatus() { return m_status_code; }
	//int getContentLength() { return m_content_length; }
	//std::string getContentType() { return m_content_type; }
	std::string getRedirectPath() { return m_redirect_path; }
	std::vector<std::pair<std::string, std::string>> getHeaders();

	void emptyVector() { m_headers.clear(); }

private:
	bool is_digits(const std::string &str);
	size_t find_line_ending(std::string input_string);

	std::vector<std::pair<std::string, std::string>> m_headers;

	int m_status_code;
	//int m_content_length;
	std::string m_content_type;
	std::string m_redirect_path; 
};

#endif