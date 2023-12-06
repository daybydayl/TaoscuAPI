#pragma once
#ifndef CTAOSSYN
#define CTAOSSYN
#include <vector>
#include <map>
#include <string>
#include <QtCore>//Qt5�Ŀ���ֻ�õ�GBK��utf8����ת��
#include <iostream>
#include <fstream>//���ļ�����
#pragma execution_character_set("utf-8")//���������������
//#include "CCommonHead.h"
using namespace std;

#include "taos.h"//�������ڣ����Լ�·������λ��
//����������ϵͳͷ�ļ�
#include "dnet_all.h"
//������ʷ����ϵͳͷ�ļ�
#include "hds_all.h"
//����ʵʱ����ϵͳͷ�ļ�
#include "rdb_all.h"

#ifdef TAOSLIB_EXPORTS
#define TAOSLIB_API __declspec(dllexport)
#else
#define TAOSLIB_API __declspec(dllimport)
#endif

class TAOSLIB_API CTaosSyn
{
public:
	CTaosSyn();
	~CTaosSyn();

	//�������ݿ�������ǰ��ֵ�������ݿ�ĳ�Ա������m_host��m_user��m_pass(Ĭ��m_db=NULL��m_port=6030���ɲ���)
	int InitAccess(
		const CTaosSyn* TaosSyn_obj					//[in]�Ѹ�ֵ�Ķ���ע����const��Ϊ�������
	);

	//sqlֱ�Ӳ������ݿ⣬������ DQL��DML �� DDL ��䡣
	int ExecuteSqlCtlDB(
		const char* sqlstr							//[in]ִ�е�sql���
	);
	//���أ�����ԭ�������
	int ExecuteSqlCtlDB(
		const char* sqlstr,							//[in]ִ�е�sql���
		TAOS_RES*&	res								//[out]����ԭ���Ľ��������taos�ӿڽ���
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


	/*=================δ��ɣ����������Ƿ���Ҫ===========================*/
	int ExecuteInsertNRecordbyFile(
		const char*		tbname,						//[in]��Ҫ�������ݵĿ��Ӧ��������rr6000.meas1(��ͨ��)��rr6000.meas_meter1(������)
		const char*		FileLoc,					//[in]�����ļ�·��
		int				TAGNum = 0,					//[in]�򳬼����Ӧ��tag��������Ŀ���Ϊ�������������������������ȱʡ����
		char**			TAGsName = 0				//[in]����ָ���������[������ַ����][��������ռ��ַ]���򳬼���tag��Ӧ�������
	);

	//buf�еĸ�����¼����ͬһ�ű��У�ͨ��rr6000ϵͳ��ȡ�ļ�¼buf�ֽ����������ݲ�����е��ӱ�
	int InsertNRecordtoOneTablebyBuf(
		const char*		buf,						//[in]�����buf
		const int		fields_num,					//[in]buf�ﵥ����¼����
		const int		record_num,					//[in]buf���¼��
		const TABLE_HEAD_FIELDS_INFO* table_fields_info,//[in]����Ϣ
		const char*		dbname,						//[in]д���Ӧ���ݿ�
		const char*		tbname,						//[in]д���Ӧ�ӱ��ӱ�������沿�ֿ�ȱʡ
		const char*		stbname = 0,				//[in]���ӱ����ڣ�����������ӱ������ĳ�����
		const char**	TAGsValue = 0,				//[in]stbname�ǿ�ʱ�����ӱ������tagֵ���ַ�������ϵ����ţ�����TAGֵȫ�����TAGsName��ȱʡ��TAGsNum����ȱʡ
		const int		TAGsNum = 0,				//[in]TAG����
		const char**	TAGsName = 0				//[in]stbname�ǿ���TAGsValue��ȫ��tagʱ�����ӱ������tag�ֶ���
	);

	//buf�еĸ�����¼�½��ӱ�
	int InsertNRecordtoNTablebyBuf(
		const char*		buf,						//[in]�����buf
		const int		fields_num,					//[in]buf�ﵥ����¼����
		const int		record_num,					//[in]buf���¼��
		const TABLE_HEAD_FIELDS_INFO* table_fields_info,//[in]����Ϣ
		const char*		dbname,						//[in]д���Ӧ���ݿ�
		const char*		stbname,					//[in]��������ش��ڣ��ǰ���¼����Ӧ�ӱ�
		const short		tdname_hfield_no,			//[in]������¼��Ҫ����������ʷ���(hdb_field_no)
		const short*	values_hfield_no,			//[in]������¼��������ֵ����ʷ�������
		const short*	TAGs_hfield_no = 0,			//[in]������¼����TAG���ݵ���ʷ������飬�ַ�������ϵ����ţ���TAGֵȫ�����TAGsName��ȱʡ��TAGsNum����ȱʡ
		const int		TAGsNum = 0,				//[in]TAG����
		const char**	TAGsName = 0				//[in]TAGs_hfield_no�����ȫ��tagʱ�����ӱ�����ĳ���tag�ֶ���������TAGֵ��Ĭ��NULL
	);




public://taosͬ���ӿڳ�Ա����
	TAOS*				m_ptaos;				//taos���ݿ���

	//�������ݿ����ӱ���
	string				m_host;					//TDengine��Ⱥ����һ�ڵ��FQDN
	string				m_user;					//�û���
	string				m_pass;					//����
	string				m_db;					//���ݿ����֣�����û�û���ṩ��Ҳ�����������ӣ��û�����ͨ�������Ӵ����µ����ݿ⣬����û��ṩ�����ݿ����֣���˵�������ݿ��û��Ѿ������ã�ȱʡʹ�ø����ݿⲽ��
	int					m_port;					//taosd��������Ķ˿ڣ�Ĭ��6030

};

#define SUCCESS_TAOS					0	//ִ�гɹ�
#define VARIABLE_NO_VALUE_TAOS			1	//���ݿ����ӱ���δ��ֵ
#define HANDLE_FAILED_TAOS				2	//�����ȡʧ��
#define RES_EXE_FAILED					3	//ִ��ʧ��
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//�ֶ�nchar��binary�������
#define CURRENCY_ERROR					5	//ͨ�ô����磺�ڴ��ͷ���������
#define OPEN_FIEL_FAILED				6	//�ļ���ʧ��
#define ACCESS_DB_FAILED				7	//���ݿ����ʧ��
#define DATA_TYPE_ERROR					8	//�������ʹ���


#endif // !CTAOSSYN