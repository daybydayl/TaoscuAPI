#pragma once
#ifndef CTAOSASYN_H
#define CTAOSASYN_H
#include <string>
#include "TaosCHead.h"
using namespace std;



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
	int ExecuteOneQueryDirectofRecordList(
		const char*		szSqlSen,					//[in]Sql���
		char**&			nResult,					//[out]����ȫ���������ֽ�����ʽ
		int&			Record_num,					//[out]���ؼ�¼��
		int&			Onerecordlen,				//[out]һ����¼�ĳ���
		TAOS_FIELD*&	fields_info,				//[out]����Ϣ����
		int&			fields_num,					//[out]����
		const int		nExecDBNo = 0				//[in]���ѯ�����ݿ��ţ�����������չ��ȱʡֵΪ0
	);

public://�ص���������ҪΪ��̬��ȫ�ֺ�������Ϊ�ص�������Ҫ������ƥ�䣬��ĳ�Ա����Ĭ�ϻ��һ��thisָ��
	static void GetBlock_CallBack(void* param, TAOS_RES* res, int code);//���ûص��������ε�����ָ�룬res�������code���������״̬
	static void AnalysisRow_CallBack(void* param, TAOS_RES* res, int numOfRows);//���ûص��������ε�����ָ�룬res�������code���������״̬


public://taosͬ���ӿڳ�Ա����
	TAOS* m_ptaos;				//taos���ݿ���

	//�������ݿ����ӱ���
	string				m_host;					//TDengine��Ⱥ����һ�ڵ��FQDN
	string				m_user;					//�û���
	string				m_pass;					//����
	string				m_db;					//���ݿ����֣�����û�û���ṩ��Ҳ�����������ӣ��û�����ͨ�������Ӵ����µ����ݿ⣬����û��ṩ�����ݿ����֣���˵�������ݿ��û��Ѿ������ã�ȱʡʹ�ø����ݿⲽ��
	int					m_port;					//taosd��������Ķ˿ڣ�Ĭ��6030

};



#endif // !CTAOSASYN_H



