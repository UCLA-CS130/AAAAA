#include "HttpRequest.h"
#include <vector>
#include <iostream>
using namespace std;

HttpRequest::HttpRequest(vector <char> request)
{
	message_request = request;
	m_err = 200;
	decodeFirstLine();
}

HttpRequest::HttpRequest()
{
	m_err = 200;
}


int HttpRequest::decodeFirstLine()
{
	int spaces = 0;

	string method; 
	string url; 
	string version;

	setErr(false);

	for (unsigned int i=0; i < message_request.size(); i++)	
	{
		if (message_request[i] == '\r')
			break;

		if (message_request[i] == ' ') {
		  if (i == 0)
		    setErr(400);
		  if (message_request[i-1] == ' ')
		    setErr(400);
		     spaces++;
		     continue;
		}

		switch(spaces)	
		{
		case 0:
			method += message_request[i];
			break;
		case 1:
			url += message_request[i];
			break;
		case 2:
			version += message_request[i];
			break;
		}	
	}

	cout << "this is the method " << method << endl;
	if (method != "GET" && method != "POST") {
			cerr << "Invalid method in request line!" << endl;
			setErr(501);
	}

	if (version != "HTTP/1.0" && version != "HTTP/1.1") {
		cerr << "Unsupported version in request line!" << endl;
		setErr(505);
	}
	if (url.size() < 1)
	  setErr(400);

	setMethod(method);
	setUrl(url);
	setVersion(version);

	if (m_err != 200)
		return -1;

	return 0;
}

string HttpRequest::getMethod()
{
  return m_method;
}

void HttpRequest::setMethod(string method)
{
  m_method = method;
}

string HttpRequest::getUrl()
{
  return m_url;
}

void HttpRequest::setUrl(string url)
{
  m_url = url;
}

void HttpRequest::setErr(int e)
{
  m_err = e;
}

int HttpRequest::getErr()
{
  return m_err;
}

void HttpRequest::setVersion(string v)
{
	m_version = v;
}

string HttpRequest::getVersion()
{
	return m_version;
}