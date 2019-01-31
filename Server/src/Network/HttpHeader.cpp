#include "HttpHeader.h"
#include <boost/algorithm/string.hpp>

HttpHeader::HttpHeader()
{

}

HttpHeader::HttpHeader(const std::string& headerStr)
{
	initWithString(headerStr);
}

HttpHeader::HttpHeader(const char* data, size_t length)
{
	string headerStr(data, length);
	initWithString(headerStr);
}

HttpHeader::~HttpHeader()
{

}

void HttpHeader::initWithString(const string& headerStr)
{
	size_t startPos = 0;
	size_t endPos = headerStr.find("\r\n", startPos);
	while (endPos != string::npos)
	{
		string line = headerStr.substr(startPos, endPos - startPos);
		if (line.length() > 0)
		{
			size_t pos = line.find(":");
			if (pos != string::npos)
			{
				string key = line.substr(0, pos);
				string value = line.substr(pos + 1);
				boost::algorithm::trim(key);
				boost::algorithm::trim(value);
				params[key] = value;
			}
			else
			{
				header.push_back(line);
			}
		}
		startPos = endPos + 2;
		endPos = headerStr.find("\r\n", startPos);
	}
}

const string& HttpHeader::toString()
{
	if (rawData.length() == 0)
	{
		for (auto iter = header.begin(); iter != header.end(); ++iter)
		{
			rawData += *iter;
		}
		for (auto iter = params.begin(); iter != params.end(); ++iter)
		{
			if (iter->first != "NULL")
			{
				rawData += iter->first;
				rawData += ": ";
			}
			rawData += iter->second;
			rawData += "\r\n";
		}
		rawData += "\r\n";
	}
	return rawData;
}

std::string HttpHeader::getValueForKey(const string& key)
{
	auto val = params.find(key);
	if (val != params.end())
	{
		return val->second;
	}
	return std::string();
}

void HttpHeader::setValueForKey(const string& key, const  string& value)
{
	params[key] = value;
	rawData.clear();
}

void HttpHeader::addHeader(const string& headerStr)
{
	header.push_back(headerStr);
	rawData.clear();
}

void HttpHeader::removeHeader(const string& headerStr)
{
	header.remove(headerStr);
	rawData.clear();
}
