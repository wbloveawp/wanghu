#ifndef SERVICE_ATTRIB_MANAGER_HEAD_FILE
#define SERVICE_ATTRIB_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//�������Թ���
class CServiceAttribManager
{
	//��������
protected:
	static tagServiceAttrib		m_ServiceAttribArray[];			//��������

	//��������
public:
	//���캯��
	CServiceAttribManager();
	//��������
	virtual ~CServiceAttribManager();

	//���ܺ���
public:
	//ö�ٷ���
	static tagServiceAttrib* EnumServiceAttrib(WORD wIndex);
	//��ȡ����
	static tagServiceAttrib* SearchServiceAttrib(WORD wServiceModule);	
};

//////////////////////////////////////////////////////////////////////////

#endif
