#include "stdafx.h"
#include <math.h>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "WHJson.h"
#include "WHCommon.h"

namespace WHJson
{

#define FILTER_SPACE while(isspace((int)c)) {c=reader.read();}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

CJsonValuePtr CWHJson::getValue(CBufferJsonReader & reader)
{
	char c=reader.read();
	FILTER_SPACE;

	switch(c)
	{
		case '{':
		{
			return getObj(reader);
			break;
		}
		case '[':
		{
			return getArray(reader);
			break;
		}
		case '"':
		{
			return getString(reader, c);
			break;
		}
		case 'T':
		case 't':
		case 'F':
		case 'f':
		{
			return getBoolean(reader, c);
			break;
		}
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
		{
			return getNum(reader, c);
			break;
		}
		case 'n':
		case 'N':
		{
			return getNull(reader, c);
		}
		default:
			{
				char s[64];
				snprintf(s, sizeof(s), "buffer overflow when peekBuf, over %u.", (uint32_t)(uint32_t)reader.getCur());
				throw exception(s);
			}
	}
}

CJsonValueObjPtr CWHJson::getObj(CBufferJsonReader & reader)
{
	CJsonValueObjPtr p = new CJsonValueObj();
	bool bFirst=true;
	while(1)
	{
		char c=reader.read();
		FILTER_SPACE;
		if(c == '}' && bFirst)
		{
			return p;
		}
		bFirst=false;

		if(c != '"')
		{
			char s[64];
			snprintf(s, sizeof(s), "get obj error(key is not string)[pos:%u]", (uint32_t)reader.getCur());
			throw exception(s);
		}
		CJsonValueStringPtr pString=getString(reader);
		c=reader.read();
		FILTER_SPACE;
		if(c != ':')
		{
			char s[64];
			snprintf(s, sizeof(s), "get obj error(: not find)[pos:%u]", (uint32_t)reader.getCur());
			throw exception(s);
		}
		CJsonValuePtr pValue=getValue(reader);
		p->value[pString->value]=pValue;

		c=reader.read();
		FILTER_SPACE;

		if(c == ',')
			continue;
		if(c == '}')
			return p;

		char s[64];
		snprintf(s, sizeof(s), "get obj error(, not find)[pos:%u]", (uint32_t)reader.getCur());
		throw exception(s);
	}
}

CJsonValueArrayPtr CWHJson::getArray(CBufferJsonReader & reader)
{
	CJsonValueArrayPtr p = new CJsonValueArray();
	bool bFirst=true;
	while(1)
	{
		char c;
		if(bFirst)
		{
			c=reader.read();
			FILTER_SPACE;
			if(c == ']')
			{
				return p;
			}
			reader.back();
		}
		bFirst=false;

		CJsonValuePtr pValue=getValue(reader);
		p->push_back(pValue);

		c=reader.read();
		FILTER_SPACE;
		if(c == ',')
			continue;
		if(c == ']')
			return p;

		char s[64];
		snprintf(s, sizeof(s), "get vector error(, not find )[pos:%u]", (uint32_t)reader.getCur());
		throw exception(s);
	}
}

CJsonValueStringPtr CWHJson::getString(CBufferJsonReader & reader,char head)
{
	CJsonValueStringPtr p = new CJsonValueString();
	const char * pChar=reader.getPoint();
	char c;
	uint32_t i=0;
	while(1)
	{
		c=reader.read();
		if(c == '\\')
		{
			p->value.append(pChar,i);
			pChar=pChar+i+2;
			i=0;
			c=reader.read();
			if(c == '\\' || c == '\"' || c == '/')
				p->value.append(1,c);
			else if(c == 'b')
				p->value.append(1,'\b');
			else if(c == 'f')
				p->value.append(1,'\f');
			else if(c == 'n')
				p->value.append(1,'\n');
			else if(c == 'r')
				p->value.append(1,'\r');
			else if(c == 't')
				p->value.append(1,'\t');
			else if(c == 'u')
			{
				uint32_t iCode=getHex(reader);

				if (iCode < 0x00080)
				{
					p->value.append(1,(char)(iCode & 0xFF));
				}
				else if (iCode < 0x00800)
				{
					p->value.append(1,(char)(0xC0 + ((iCode >> 6) & 0x1F)));
					p->value.append(1,(char)(0x80 + (iCode & 0x3F)));
				}
				else if (iCode < 0x10000)
				{
					p->value.append(1,(char)(0xE0 + ((iCode >> 12) & 0x0F)));
					p->value.append(1,(char)(0x80 + ((iCode >> 6) & 0x3F)));
					p->value.append(1,(char)(0x80 + (iCode & 0x3F)));
				}
				else
				{
					p->value.append(1,(char)(0xF0 + ((iCode >> 18) & 0x07)));
					p->value.append(1,(char)(0x80 + ((iCode >> 12) & 0x3F)));
					p->value.append(1,(char)(0x80 + ((iCode >> 6) & 0x3F)));
					p->value.append(1,(char)(0x80 + (iCode & 0x3F)));
				}

				pChar+=4;
			}
		}
		else if(c==head)
			break;
		else
			i++;
	}
	p->value.append(pChar,i);
	return p;
}

CJsonValueNumPtr CWHJson::getNum(CBufferJsonReader & reader,char head)
{
	bool bOk=true;
	bool bFloat=false;
	bool bExponential=false;
	bool bNegative=false;
	bool bExponentialNegative=false;
	int64_t iInt=0;
	double dFloat=0;
	double dFloatRat=0;
	int64_t iExponential=0;
	if(head == '-')
	{
		bOk=false;
		bNegative=true;
	}
	else
		iInt=head-0x30;
	char c;
	bool bNeedBack=false;
	while(1)
	{
		if(reader.hasEnd())
			break;
		c=reader.read();
		if(c>=0x30 && c<=0x39)
		{
			bOk=true;
			if(bExponential)
				iExponential=iExponential*10+c-0x30;
			else if(bFloat)
			{
				dFloat=dFloat+dFloatRat*(c-0x30);
				dFloatRat=dFloatRat*0.1;
			}
			else
				iInt=iInt*10+c-0x30;
		}
		else if(c == '.' && !bFloat && !bExponential && bOk)
		{
			bOk=false;
			bFloat=true;
			dFloatRat=0.1;
		}
		else if((c == 'e' || c == 'E') && !bExponential && bOk)
		{
			bOk=false;
			bExponential=true;
			iExponential=0;
			if(reader.hasEnd())
				break;
			c=reader.read();
			if(c == '-')
				bExponentialNegative=true;
			else if(c == '+')
				bExponentialNegative=false;
			else if(c>=0x30 && c<=0x39)
			{
				bOk=true;
				bExponential=(bool)(c-0x30);
			}
			else
			{
				bNeedBack=true;
				break;
			}
		}
		else
		{
			bNeedBack=true;
			break;
		}
	}
	if(!bOk)
	{
		char s[64];
		snprintf(s, sizeof(s), "get num error[pos:%u]", (uint32_t)reader.getCur());
		throw exception(s);
	}
	if(bNeedBack)
		reader.back();
	if(bExponentialNegative)
		iExponential=0-iExponential;
	double dResult=(iInt+dFloat)*pow(10,iExponential);
	if(bNegative)
		dResult=0-dResult;
	CJsonValueNumPtr p = new CJsonValueNum();
	p->value=dResult;
	p->isInt=!bFloat;
	return p;
}

//为了提高效率和代码好写就先这么写了
CJsonValueBooleanPtr CWHJson::getBoolean(CBufferJsonReader & reader,char c)
{
	bool bOk=false;
	bool bValue;
	if(c=='t'||c=='T')
	{
		c=reader.read();
		if(c=='r'||c=='R')
		{
			c=reader.read();
			if(c=='u'||c=='U')
			{
				c=reader.read();
				if(c=='e'||c=='E')
				{
					bValue=true;
					bOk=true;
				}
			}
		}
	}
	else if(c=='f'||c=='F')
	{
		c=reader.read();
		if(c=='a'||c=='A')
		{
			c=reader.read();
			if(c=='l'||c=='L')
			{
				c=reader.read();
				if(c=='s'||c=='S')
				{
					c=reader.read();
					if(c=='e'||c=='E')
					{
						bValue=false;
						bOk=true;
					}
				}
			}
		}
	}

	if(!bOk)
	{
		char s[64];
		snprintf(s, sizeof(s), "get bool error[pos:%u]", (uint32_t)reader.getCur());
		throw exception(s);
	}

	CJsonValueBooleanPtr p = new CJsonValueBoolean();
	p->value=bValue;
	return p;
}

CJsonValuePtr CWHJson::getNull(CBufferJsonReader & reader,char c)
{
	assert(c=='n' || c=='N');
	bool bOk=false;
	c=reader.read();
	if(c=='u'||c=='U')
	{
		c=reader.read();
		if(c=='l'||c=='L')
		{
			c=reader.read();
			if(c=='l'||c=='L')
			{
				bOk=true;
			}
		}
	}
	if(!bOk)
	{
		char s[64];
		snprintf(s, sizeof(s), "get NULL error[pos:%u]", (uint32_t)reader.getCur());
		throw exception(s);
	}
	return NULL;
}

uint32_t CWHJson::getHex(CBufferJsonReader & reader)
{
	uint32_t iCode=0;
	char c;
	for(int iLoop=0;iLoop<4;iLoop++)
	{
		c=reader.read();
		if(c>='a'&&c<='f')
			iCode=iCode*16+c-'a'+10;
		else if(c>='A'&&c<='F')
			iCode=iCode*16+c-'A'+10;
		else if(c>='0'&&c<='9')
			iCode=iCode*16+c-'0';
		else
		{
			char s[64];
			snprintf(s, sizeof(s), "get string error3(\\u)[pos:%u]", (uint32_t)reader.getCur());
			throw exception(s);
		}
	}
	return iCode;
}

string CWHJson::writeValue(const CJsonValuePtr & p)
{
	string ostr;
	writeValue(p, ostr);
	return ostr;
}

void CWHJson::writeValue(const CJsonValuePtr& p, vector<char>& buf)
{
  string ostr;
  writeValue(p, ostr);
  buf.assign(ostr.begin(), ostr.end());
}

void CWHJson::writeValue(const CJsonValuePtr & p, string& ostr)
{
	if(!p)
	{
		ostr += "null";
		return;
	}
	switch(p->getType())
	{
		case eJsonTypeString :
			writeString(CJsonValueStringPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeNum:
			writeNum(CJsonValueNumPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeObj:
			writeObj(CJsonValueObjPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeArray:
			writeArray(CJsonValueArrayPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeBoolean:
			writeBoolean(CJsonValueBooleanPtr::dynamicCast(p), ostr);
			break;
		default:
			assert(false);
	}
}

void CWHJson::writeString(const CJsonValueStringPtr & p, string& ostr)
{
	writeString(p->value, ostr);
}

void CWHJson::writeString(const string & s, string& ostr)
{
	ostr += "\"";
	std::string::const_iterator it(s.begin()),
		itEnd(s.end());
	for (; it != itEnd; ++it)
	{
		switch(*it)
		{
			case '"':
				ostr += "\\\"";
				break;
			case '\\':
				ostr += "\\\\";
				break;
			case '/':
				ostr += "\\/";
				break;
			case '\b':
				ostr += "\\b";
				break;
			case '\f':
				ostr += "\\f";
				break;
			case '\n':
				ostr += "\\n";
				break;
			case '\r':
				ostr += "\\r";
				break;
			case '\t':
				ostr += "\\t";
				break;
			default:
			{
				if((unsigned char)(*it)<0x20)
				{
					char buf[16];
					snprintf(buf,sizeof(buf),"\\u%04x",(unsigned char)*it);
					ostr += string(buf,6);
				}
				else
				{
					ostr.push_back(*it);
				}
				break;
			}
		}
	}
	ostr += "\"";
}


void CWHJson::writeNum(const CJsonValueNumPtr & p, string& ostr)
{
	ostringstream ss;
	if (!p->isInt)
	{
		ss << CWHCommon::tostr(p->value) ;
	}
	else
	{
		ss << (int64_t)p->value;
	}

	ostr += ss.str();
}


void CWHJson::writeObj(const CJsonValueObjPtr & p, string& ostr)
{
	ostr += "{";
	unordered_map<string,CJsonValuePtr>::const_iterator it(p->value.begin()), it_end(p->value.end());
	while (it != it_end)
	{
		writeString(it->first, ostr);
		ostr += ":" ;
		writeValue(it->second, ostr);
		if(++it != it_end)
		{
			ostr += ",";
		}
	}
	ostr += "}";
}

void CWHJson::writeArray(const CJsonValueArrayPtr & p, string& ostr)
{
	ostr += "[";
	vector<CJsonValuePtr>::const_iterator it(p->value.begin()), it_end(p->value.end());
	while (it != it_end)
	{
		writeValue(*it, ostr);
		if (++it != it_end)
		{
			ostr += ",";
		}
	}
	ostr += "]";
}

void CWHJson::writeBoolean(const CJsonValueBooleanPtr & p, string& ostr)
{
	if(p->value)
		ostr += "true";
	else
		ostr += "false";
}

CJsonValuePtr CWHJson::getValue(const string & str)
{
	CBufferJsonReader reader;
	reader.setBuffer(str.c_str(),str.length());
	return getValue(reader);
}

CJsonValuePtr CWHJson::getValue(const vector<char>& buf)
{
	CBufferJsonReader reader;
	reader.setBuffer(buf);
	return getValue(reader);
}

//json里面定义的空白字符
bool CWHJson::isspace(char c)
{
	if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
		return true;
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void CJsonWriteOstream::writeValue(const CJsonValuePtr & p, ostream& ostr)
{
	if(!p)
	{
		ostr << "null";
		return;
	}
	switch(p->getType())
	{
		case eJsonTypeString :
			writeString(CJsonValueStringPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeNum:
			writeNum(CJsonValueNumPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeObj:
			writeObj(CJsonValueObjPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeArray:
			writeArray(CJsonValueArrayPtr::dynamicCast(p), ostr);
			break;
		case eJsonTypeBoolean:
			writeBoolean(CJsonValueBooleanPtr::dynamicCast(p), ostr);
			break;
		default:
			assert(false);
	}
}

void CJsonWriteOstream::writeString(const CJsonValueStringPtr & p, ostream& sReturn)
{
	writeString(p->value, sReturn);
}

void CJsonWriteOstream::writeString(const string & s, ostream& sReturn)
{
	sReturn << "\"";
	std::string::const_iterator it(s.begin()),
		itEnd(s.end());
	for (; it != itEnd; ++it)
	{
		switch(*it)
		{
			case '"':
				sReturn << "\\\"";
				break;
			case '\\':
				sReturn << "\\\\";
				break;
			case '/':
				sReturn <<"\\/";
				break;
			case '\b':
				sReturn << "\\b";
				break;
			case '\f':
				sReturn << "\\f";
				break;
			case '\n':
				sReturn << "\\n";
				break;
			case '\r':
				sReturn << "\\r";
				break;
			case '\t':
				sReturn << "\\t";
				break;
			default:
			{
				if((unsigned char)(*it)<0x20)
				{
					char buf[16];
					snprintf(buf,sizeof(buf),"\\u%04x",(unsigned char)*it);
					sReturn << string(buf,6);
				}
				else
				{
					sReturn << *it;
				}
				break;
			}
		}
	}
	sReturn << "\"";
}


void CJsonWriteOstream::writeNum(const CJsonValueNumPtr & p, ostream& ostr)
{
	if (!p->isInt)
	{
		ostr << CWHCommon::tostr(p->value);
	}
	else
	{
		ostr << (int64_t)p->value;
	}
}


void CJsonWriteOstream::writeObj(const CJsonValueObjPtr & p, ostream& ostr)
{
	ostr << "{ ";
	unordered_map<string,CJsonValuePtr>::const_iterator it(p->value.begin()), it_end(p->value.end());
	while (it != it_end)
	{
		writeString(it->first, ostr);
		ostr << ": " ;
		writeValue(it->second, ostr);
		if(++it != it_end)
		{
			ostr << ", ";
		}
	}
	ostr << " }";
}

void CJsonWriteOstream::writeArray(const CJsonValueArrayPtr & p, ostream& ostr)
{
	ostr << "[ ";
	vector<CJsonValuePtr>::const_iterator it(p->value.begin()), it_end(p->value.end());
	while (it != it_end)
	{
		writeValue(*it, ostr);
		if (++it != it_end)
		{
			ostr << ", ";
		}
	}
	ostr << " ]";
}

void CJsonWriteOstream::writeBoolean(const CJsonValueBooleanPtr & p, ostream& ostr)
{
	if(p->value)
		ostr << "true";
	else
		ostr << "false";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

}