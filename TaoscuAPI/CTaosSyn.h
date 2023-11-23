#pragma once
#ifndef CTAOSSYN
#define CTAOSSYN
#include <vector>
#include "CCommonHead.h"
using namespace std;

class CTaosSyn
{
public:
	CTaosSyn();
	~CTaosSyn();
	//�������ݿ�������ǰ��ֵ�������ݿ�ĳ�Ա������m_host��m_user��m_pass(Ĭ��m_db=NULL��m_port=6030���ɲ���)
	int InitAccess(
		const CTaosSyn* TaosSyn_obj,				//[in]�Ѹ�ֵ�Ķ���
		TAOS*&			pTaos						//[out]���ݿ���.�������ݿⲢ�������ݿ�����ע����const��Ϊ�������
	);

	//��ȡsqlȫ����¼����һά�ֽ�����ʽ����nResult,����ֵ0�ɹ���
	//�ŵ㣺ȡ���ݷ��㡣ȱ�㣺ʱ����ܵ���Ҳ��Щ�����ڲ�ͨ���������ȡ������(ʵ��210����4������ 11��)
	int ExecuteOneQueryDirectofRecordBytes(
		const char*		szSqlSen,					//[in]Sql���
		char*&			nResult,					//[out]����ȫ���������ֽ�����ʽ
		int&			Record_num,					//[out]���ؼ�¼��
		int&			Onerecordlen,				//[out]һ����¼�ĳ���
		TAOS_FIELD*&	fields_info,				//[out]����Ϣ����
		int&			fields_num,					//[out]����
		const int		nExecDBNo = 0				//[in]���ѯ�����ݿ��ţ�����������չ��ȱʡֵΪ0
	);


	//��ȡsqlȫ�����ݣ��Զ�ά�ռ��ֽ�������Result������ֵ0�ɹ���/*��������ӿڣ��ٶȸ��ѣ�ʵ��210����4������ 4��*/
	//�ŵ㣺�ٶȿ죬ȡ���ݷ��㡣ȱ�㣺�ڴ濪���Դ��ͷ��ڴ����ȶ�ÿ������ָ������ͷţ����ͷ�һ��ָ�롣����ʹ��FreePtP�ӿ��ͷ�
	int ExecuteOneQueryDirectofRecordList(
		const char*		szSqlSen,					//[in]Sql���
		char**&			Result,						//[out]���ؼ�¼������¼�������ֽ�����ʽ��[��¼ָ�����][��¼���ݿռ�ָ��]
		int&			Record_num,					//[out]���ؼ�¼��
		TAOS_FIELD*&	fields_info,				//[out]����Ϣ����
		int&			fields_num,					//[out]����
		const int		nExecDBNo = 0				//[in]���ѯ�����ݿ��ţ�����������չ��ȱʡֵΪ0
	);
	//�ͷ�**Result����ָ��
	int FreePtP(
		char**&			Result,						//[in]���ͷŵĶ���ָ��
		int&			Record_num					//[in]���һ��ָ����
	);

	/*======================д��=======================*/

	








public://taosͬ���ӿڳ�Ա����
	TAOS*				m_ptaos;				//taos���ݿ���

	//�������ݿ����ӱ���
	string				m_host;					//TDengine��Ⱥ����һ�ڵ��FQDN
	string				m_user;					//�û���
	string				m_pass;					//����
	string				m_db;					//���ݿ����֣�����û�û���ṩ��Ҳ�����������ӣ��û�����ͨ�������Ӵ����µ����ݿ⣬����û��ṩ�����ݿ����֣���˵�������ݿ��û��Ѿ������ã�ȱʡʹ�ø����ݿⲽ��
	int					m_port;					//taosd��������Ķ˿ڣ�Ĭ��6030





};

#endif // !CTAOSSYN