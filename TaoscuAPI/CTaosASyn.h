#pragma once
#ifndef CTAOSASYN_H
#define CTAOSASYN_H
#ifndef _WINDOWS
#include <unistd.h>
#endif // !_WINDOWS
#include <string>
#include "TaosCHead.h"
using namespace std;


/*=========================���ַ���ͬ���첽�н������࣬���濴�Ƿ���Ҫ�ϲ����в����ڴ�й©====================================*/


class CTaosASyn
{
public:
	CTaosASyn();
	~CTaosASyn();

	//�������ݿ�������ǰ��ֵ�������ݿ�ĳ�Ա������m_host��m_user��m_pass(Ĭ��m_db=NULL��m_port=6030���ɲ���)
	int InitAccess(
		const CTaosASyn* TaosSyn_obj				//[in]�Ѹ�ֵ�Ķ���ע����const��Ϊ�������
	);

public:
	//�첽��ѯ���������GetRecordList�ӿ�ȡresult��ִ���겻Ӱ�������̼�����Ҫ����ʱ�ڼ���(������ǰ��װ)
	int WaitOneQueryDirectofRecordList(
		const char*		szSqlSen,					//[in]Sql���
		int&			Record_num,					//[out]���ؼ�¼��
		TAOS_FIELD*&	fields_info,				//[out]����Ϣ����
		int&			fields_num,					//[out]����
		const int		nExecDBNo = 0				//[in]���ѯ�����ݿ��ţ�����������չ��ȱʡֵΪ0
	);
	//�ȴ��첽��ѯ��ȡ�����ɺ󷵻ؽ��buf
	int GetRecordList(char**& nResult);
	//�ͷ�**Result����ָ��
	int FreePtP(
		char**& Result,								//[in]���ͷŵĶ���ָ��
		int& Record_num								//[in]���һ��ָ����
	);

//�ڲ��ӿ� �ص���������ҪΪ��̬��ȫ�ֺ�������Ϊ�ص�������Ҫ������ƥ�䣬��ĳ�Ա����Ĭ�ϻ��һ��thisָ��
private:
	//����WaitOneQueryDirectofRecordList
	static void GetBlock_CallBack(void* param, TAOS_RES* res, int code);//���ûص��������ε�����ָ�룬res�������code���������״̬
	static void AnalysisRow_CallBack(void* param, TAOS_RES* res, int numOfRows);//���ûص��������ε�����ָ�룬res�������code���������״̬


//taosͬ���ӿڳ�Ա����
public:
	TAOS* m_ptaos;								//taos���ݿ���
	//�������ݿ����ӱ���
	string				m_host;					//TDengine��Ⱥ����һ�ڵ��FQDN
	string				m_user;					//�û���
	string				m_pass;					//����
	string				m_db;					//���ݿ����֣�����û�û���ṩ��Ҳ�����������ӣ��û�����ͨ�������Ӵ����µ����ݿ⣬����û��ṩ�����ݿ����֣���˵�������ݿ��û��Ѿ������ã�ȱʡʹ�ø����ݿⲽ��
	int					m_port;					//taosd��������Ķ˿ڣ�Ĭ��6030

//�ڲ���Ա����
private:
	char**				m_szResult;				//�첽��ѯ���ص�buf��άָ��
	int					m_Retresult;			//buf״̬����ֵ
	bool				m_FinishedResult;		//buf�Ƿ�װ����־

};



#endif // !CTAOSASYN_H



