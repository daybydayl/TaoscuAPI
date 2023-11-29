#include "CCommonHead.h"
#include "CTaosSyn.h"


//����rr6000��64λ�汾����Ҫ������Ӧ��ϵͳ
//�����������
system_net_info dnet_obj;
//������ʷ����ʶ���
HAccess hds_obj;
//����ʵʱ����ʶ���
RDB_NET rdb_net;



//���������api
void testmaster(CTaosSyn*& pTaosSyn_obj);
void testExecuteOneQueryDirectofRecordBytes(CTaosSyn*& pTaosSyn_obj);
void testExecuteSqlCtlDB(CTaosSyn*& pTaosSyn_obj);
void testExecuteOneQueryDirectofRecordList(CTaosSyn*& pTaosSyn_obj);

void testExecuteInsertNRecordbyFile(CTaosSyn*& pTaosSyn_obj);
void testExecuteInsertNRecordbyBuf(CTaosSyn*& pTaosSyn_obj);

int main(int argc, char* argv[])
{
	//��ʼ�������������rr6000ȥ���ò���ע�ͼ��ɣ���ʼ��rr6000��64λ�汾����Ҫ������Ӧ��ϵͳ
	char str[60];
	strcpy(str, "lzj");
	dnet_obj.set_system_net_info(str, DNET_NO);

	rdb_net.set_dnet_object(dnet_obj);
	//��ʼ����ʷ����ʶ���
	if (hds_obj.InitAccess(&dnet_obj, HDS_API_TYPE_SERVICE) != RETNO_SUCCESS)
	{
		return -1;
	}


	//��װ�ӿڲ���
	int ret = -1;
	CTaosSyn* pTaosSyn_obj = new CTaosSyn();
	pTaosSyn_obj->m_host = "lzj-VM";
	pTaosSyn_obj->m_user = "root";
	pTaosSyn_obj->m_pass = "taosdata";
	pTaosSyn_obj->m_db = "";
	pTaosSyn_obj->m_port = 6030;

	ret = pTaosSyn_obj->InitAccess(pTaosSyn_obj);
	if (0 != ret)
		return -1;

	//���������api
	testmaster(pTaosSyn_obj);
	

	return 0;
}

void testmaster(CTaosSyn*& pTaosSyn_obj)
{
	//testExecuteOneQueryDirectofRecordBytes(pTaosSyn_obj);
	//testExecuteSqlCtlDB(pTaosSyn_obj);
	//testExecuteOneQueryDirectofRecordList(pTaosSyn_obj);
	//testExecuteInsertNRecordbyFile(pTaosSyn_obj);���Ƿ���Ҫ�������
	testExecuteInsertNRecordbyBuf(pTaosSyn_obj);
}

void testExecuteOneQueryDirectofRecordBytes(CTaosSyn*& pTaosSyn_obj)
{
	int Ret = -1;
	char* szResult = NULL;
	int Record_num;
	int Onerecordlen;
	TAOS_FIELD* fields_info;
	int fields_num = 0;
	Ret = pTaosSyn_obj->ExecuteOneQueryDirectofRecordBytes("select TO_ISO8601(ts, '+00:00'),meas_value,meas_name,meas_type from rr6000.meas_meter1", szResult, Record_num, Onerecordlen, fields_info, fields_num);

	//�������Դ���ֽ����Ƿ���ȷ
	int Offset = 0;
	char date[65] = { 0 };
	int64_t ts = 0;
	double value = 0;
	char name[60] = { 0 };
	int type = 0;
	for (int i = 0; i < Record_num; i++)//��¼����������¼���ȣ���ƫ����
	{
		Offset = 0;
		memcpy(date, szResult + Onerecordlen * i, fields_info[0].bytes);
		Offset += fields_info[0].bytes;
		memcpy((char*)&value, szResult + Onerecordlen * i + Offset, fields_info[1].bytes);
		Offset += fields_info[1].bytes;
		memcpy(name, szResult + Onerecordlen * i + Offset, fields_info[2].bytes);
		Offset += fields_info[2].bytes;
		memcpy((char*)&type, szResult + Onerecordlen * i + Offset, fields_info[3].bytes);
		//sscanf(szResult + Onerecordlen * i + value_Offset, "%f", &value);
	}
	free(szResult);
}

void testExecuteSqlCtlDB(CTaosSyn*& pTaosSyn_obj)
{
	TAOS_RES* res;
	int Ret = -1;
	Ret = pTaosSyn_obj->ExecuteSqlCtlDB("create database IF NOT EXISTS lzj");
	Ret = pTaosSyn_obj->ExecuteSqlCtlDB("use lzj");
	Ret = pTaosSyn_obj->ExecuteSqlCtlDB("create stable IF NOT EXISTS meas_meter(ts timestamp, meas_value double) tags(meas_name nchar(60), meas_type int)");
	Ret = pTaosSyn_obj->ExecuteSqlCtlDB("show stables",res);


	//�鿴res��ȡ���Ƿ�����
	TAOS_ROW    row;
	int         num_fields = taos_field_count(res);
	TAOS_FIELD* fields = taos_fetch_fields(res);
	string str = "";
	while ((row = taos_fetch_row(res))) {
		str.clear();
		taos_print_row((char*)str.c_str(), row, fields, num_fields);
	}

}

void testExecuteOneQueryDirectofRecordList(CTaosSyn*& pTaosSyn_obj)
{
	int Ret = -1;
	char** Result = NULL;
	int Record_num;
	TAOS_FIELD* fields_info;
	int fields_num = 0;
	Ret = pTaosSyn_obj->ExecuteOneQueryDirectofRecordList("select TO_ISO8601(ts, '+00:00'),meas_value,meas_name,meas_type from rr6000.meas_meter1", Result, Record_num, fields_info, fields_num);


	//�������Դ���ֽ����Ƿ���ȷ
	int Offset = 0;
	char date[65] = { 0 };
	int64_t ts = 0;
	double value = 0;
	char name[60] = { 0 };
	int type = 0;
	for (int i = 0; i < Record_num; i++)//��¼����������¼���ȣ���ƫ����
	{
		Offset = 0;
		memcpy(date, Result[i] + Offset, fields_info[0].bytes);
		Offset += fields_info[0].bytes;
		memcpy((char*)&value, Result[i] + Offset, fields_info[1].bytes);
		Offset += fields_info[1].bytes;
		memcpy(name, Result[i] + Offset, fields_info[2].bytes);
		Offset += fields_info[2].bytes;
		memcpy((char*)&type, Result[i] + Offset, fields_info[3].bytes);
	}
	pTaosSyn_obj->FreePtP(Result, Record_num);
}

void testExecuteInsertNRecordbyFile(CTaosSyn*& pTaosSyn_obj)
{
	//�ӿڴ����
	string ntbname = "rr6000.meas_meter2";
	string nfileloc = "D:/My_program/TaoscuAPI/TaoscuAPI/testdata.txt";
	int nTAGNum = 2;
	char** nTAGsName = (char**)malloc(nTAGNum * sizeof(char*));

	char field0[10] = "factory";
	char field1[10] = "2";
	nTAGsName[0] = field0;
	nTAGsName[1] = field1;

	pTaosSyn_obj->ExecuteInsertNRecordbyFile(ntbname.c_str(), nfileloc.c_str(), nTAGNum, nTAGsName);
	

}

void testExecuteInsertNRecordbyBuf(CTaosSyn*& pTaosSyn_obj)
{
	//��rr6000��ȡ��¼��
	/*ͨ����Ӣ������ȡ�����ݣ������ݵĳ��Ⱥ�λ��ƫ�����������õ�*/
	int Ret = -1;
	
	int record_num = 0;
	int result_len = 0;
	int recodeIdx = 0;
	int fieldIdx = 0;

	//���ƫ�������飬����������������
	int shOffset[16] = { 0 };
	const int field_num = 1;//��4����
	char chEngName[field_num][32];//32Ϊ��Ӣ�ĳ���
	TABLE_HEAD_FIELDS_INFO* table_fields_info = (TABLE_HEAD_FIELDS_INFO*)malloc(sizeof(TABLE_HEAD_FIELDS_INFO));
	if (table_fields_info == NULL)
		return;

	//��Ҫ���Ӣ����
	strcpy(chEngName[0], "meas_value");
	//strcpy(chEngName[1], "meas_name");
	//���巵��buf,��ʼ����С����
	char* data_buf = (char*)malloc(6);

	//ͨ��Ӣ��������ȡ�������Ϣ�ͼ�¼���ݼ�
	Ret = rdb_net.read_table_data_by_english_names_net(
		MEAS_INFO_NO,
		DNET_APP_TYPE_SCADA,
		chEngName,
		field_num,
		table_fields_info,
		data_buf,
		record_num,
		result_len
	);
	if (Ret < 0)
	{
		FREE((char*&)table_fields_info);
		free(data_buf);
		return;
	}


	int tagsnum = 3;
	//tag��Ӧֵ
	char** tags_value = (char**)malloc(tagsnum * sizeof(char*));
	char* value0 = (char*)malloc(12);
	strcpy(value0, "12");
	char* value1 = (char*)malloc(12);
	strcpy(value1, "16");
	char* value2 = (char*)malloc(12);
	strcpy(value2, "\'lzj2\'");//���������ģ���������������������������������������������������������
	tags_value[0] = value0;
	tags_value[1] = value1;
	tags_value[2] = value2;

	//ֻȡ����tag����tag��
	char** tags_name = (char**)malloc(tagsnum * sizeof(char*));
	char* chTAGName0 = (char*)malloc(12);
	strcpy(chTAGName0, "meas_id");
	char* chTAGName1 = (char*)malloc(12);
	strcpy(chTAGName1, "fac_id");
	char* chTAGName2 = (char*)malloc(12);
	strcpy(chTAGName2, "meas_name");
	tags_name[0] = chTAGName0;
	tags_name[1] = chTAGName1;
	tags_name[2] = chTAGName2;

	//����Ĭ���ӱ����ڣ���rr6000��������meas_test��������ômeastest1�ӱ���ȫ��tag�����ݲ��룬meas_test������ṹ��ts,meas_value,meas_id(tag),fac_id(tag),meas_name(tag)
	Ret = pTaosSyn_obj->ExecuteInsertNRecordbyBuf(data_buf, field_num, record_num, table_fields_info, "rr6000", "meastest1", "meas_test", (const char**)tags_value, tagsnum);//, (const char**)tags_name



	//�ͷſռ�
	FREE((char*&)table_fields_info);
	free(data_buf);
	pTaosSyn_obj->FreePtP(tags_value, tagsnum);
	pTaosSyn_obj->FreePtP(tags_name, tagsnum);

}
