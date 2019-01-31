#pragma once
#include <string>
#include <map>
#include <list>

using namespace std;

class HttpHeader
{
public:
	HttpHeader();
	HttpHeader(const string& headerStr);
	HttpHeader(const char* data, size_t length);
	virtual ~HttpHeader();

	const string& toString();

	string getValueForKey(const string& key);
	void setValueForKey(const string& key, const string& value);

	void addHeader(const string& headerStr);
	void removeHeader(const string& headerStr);
	inline const list<string>& getHeaders(){ return header; }

private:
	string rawData;
	list<string> header;
	map<string, string> params;

	void initWithString(const string& headerStr);
};