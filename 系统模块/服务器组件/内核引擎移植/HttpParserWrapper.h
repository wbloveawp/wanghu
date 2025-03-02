#ifndef HttpParserWrapper_h
#define HttpParserWrapper_h

#pragma once


#include <iostream>
#include "http_parser.h"

class CHttpParserWrapper
{
public:
	CHttpParserWrapper();

	virtual ~CHttpParserWrapper() { }

	void ParseHttpContent(const char* buf, size_t len);

	bool IsReadWebSocketKey() { return m_bReadWebSocketKey; }
	bool HasWebSocketKey() { return m_WebSocketKey.size() > 0; }
	void SetReadWebSocketKey(bool bReadWebSocketKey) { m_bReadWebSocketKey = bReadWebSocketKey; }
	void SetWebSocketKey(const char* szWebSocketKey, size_t length) { m_WebSocketKey.append(szWebSocketKey, length); }
	char* GetWebSocketKey() { return (char*)m_WebSocketKey.c_str(); }

	size_t GetHeaderLength() { return m_nHeaderLength; }
	void SetHeaderLength(size_t nTotalLen) { m_nHeaderLength = nTotalLen; }

	static int OnUrl(http_parser* parser, const char *at, size_t length, void* obj);
	static int OnHeaderField(http_parser* parser, const char *at, size_t length, void* obj);
	static int OnHeaderValue(http_parser* parser, const char *at, size_t length, void* obj);
	static int OnHeadersComplete(http_parser* parser, void* obj);
	static int OnBody(http_parser* parser, const char *at, size_t length, void* obj);
	static int OnMessageComplete(http_parser* parser, void* obj);

private:
	http_parser m_http_parser;
	http_parser_settings m_settings;

	std::string	m_WebSocketKey;
	bool	m_bReadWebSocketKey;

	size_t  m_nHeaderLength;
	

};

#endif

