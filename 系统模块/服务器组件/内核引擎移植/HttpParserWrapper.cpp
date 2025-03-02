#include "stdafx.h"
#include "HttpParserWrapper.h"

CHttpParserWrapper::CHttpParserWrapper()
{
}


void CHttpParserWrapper::ParseHttpContent(const char* buf, size_t len)
{
	http_parser_init(&m_http_parser, HTTP_REQUEST);
	memset(&m_settings, 0, sizeof(m_settings));
	m_settings.on_url = OnUrl;
	m_settings.on_header_field = OnHeaderField;
	m_settings.on_header_value = OnHeaderValue;
	m_settings.on_headers_complete = OnHeadersComplete;
	m_settings.on_body = OnBody;
	m_settings.on_message_complete = OnMessageComplete;
	m_settings.object = this;

	m_bReadWebSocketKey = false;
	m_WebSocketKey.clear();

	m_nHeaderLength = 0;

	http_parser_execute(&m_http_parser, &m_settings, buf, len);
}


int CHttpParserWrapper::OnUrl(http_parser* parser, const char *at,
	size_t length, void* obj)
{
	return 0;
}

int CHttpParserWrapper::OnHeaderField(http_parser* parser, const char *at,
	size_t length, void* obj)
{
	if (!((CHttpParserWrapper*)obj)->HasWebSocketKey())
	{
		if (_strnicmp(at, "Sec-WebSocket-Key", 17) == 0)
		{
			((CHttpParserWrapper*)obj)->SetReadWebSocketKey(true);
		}
	}
	return 0;
}

int CHttpParserWrapper::OnHeaderValue(http_parser* parser, const char *at, size_t length, void* obj)
{
	if (((CHttpParserWrapper*)obj)->IsReadWebSocketKey())
	{
		((CHttpParserWrapper*)obj)->SetWebSocketKey(at, length);
		((CHttpParserWrapper*)obj)->SetReadWebSocketKey(false);
	}

	return 0;
}

int CHttpParserWrapper::OnHeadersComplete(http_parser* parser, void* obj)
{
	((CHttpParserWrapper*)obj)->SetHeaderLength(parser->nread);
	return 0;
}

int CHttpParserWrapper::OnBody(http_parser* parser, const char *at, size_t length, void* obj)
{
	return 0;
}

int CHttpParserWrapper::OnMessageComplete(http_parser* parser, void* obj)
{
	return 0;
}