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
void testInsertNRecordtoOneTablebyBuf(CTaosSyn*& pTaosSyn_obj);
void testInsertNRecordtoNTablebyBuf1(CTaosSyn*& pTaosSyn_obj);
void testInsertNRecordtoNTablebyBuf2(CTaosSyn*& pTaosSyn_obj);
void testInsertNRecordtoNTablebyBufofSample(CTaosSyn*& pTaosSyn_obj);

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
	//testInsertNRecordtoOneTablebyBuf(pTaosSyn_obj);
	//testInsertNRecordtoNTablebyBuf1(pTaosSyn_obj);
	//testInsertNRecordtoNTablebyBuf2(pTaosSyn_obj);
	testInsertNRecordtoNTablebyBufofSample(pTaosSyn_obj);
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

void testInsertNRecordtoOneTablebyBuf(CTaosSyn*& pTaosSyn_obj)
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
	Ret = pTaosSyn_obj->InsertNRecordtoOneTablebyBuf(data_buf, field_num, record_num, table_fields_info, "rr6000", "meastest1", "meas_test", (const char**)tags_value, tagsnum);//, (const char**)tags_name



	//�ͷſռ�
	FREE((char*&)table_fields_info);
	free(data_buf);
	pTaosSyn_obj->FreePtP(tags_value, tagsnum);
	pTaosSyn_obj->FreePtP(tags_name, tagsnum);

}

void testInsertNRecordtoNTablebyBuf1(CTaosSyn*& pTaosSyn_obj)
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
	const int field_num = 6;//��4����
	char chEngName[field_num][32];//32Ϊ��Ӣ�ĳ���
	TABLE_HEAD_FIELDS_INFO* table_fields_info = (TABLE_HEAD_FIELDS_INFO*)malloc(sizeof(TABLE_HEAD_FIELDS_INFO));
	if (table_fields_info == NULL)
		return;

	//��Ҫ���Ӣ����
	strcpy(chEngName[0], "meas_id");//���ӱ�������
	strcpy(chEngName[1], "fac_id");
	strcpy(chEngName[2], "meas_name");
	strcpy(chEngName[3], "meas_status");
	strcpy(chEngName[4], "meas_value");
	strcpy(chEngName[5], "meas_id");//��TAG����
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

	//�ӿ�ʹ�ò���
	const int valuesnum = 2;//�����г���ʱ���������ֵ�����
	const int tagsnum = 3;//��Ҫ��tagֵ�ĳ���tags����
	short tbname_hfield_no = table_fields_info[0].hdb_field_no;//����������ʷ���
	short values_hfield_no[valuesnum];//�������ֵ�����ʷ������飬���ڽӿ��ڲ����Ҷ�Ӧ����
	values_hfield_no[0] = table_fields_info[3].hdb_field_no;
	values_hfield_no[1] = table_fields_info[4].hdb_field_no;

	short TAGs_hfield_no[tagsnum];//���tagֵ�����ʷ�������
	TAGs_hfield_no[0] = table_fields_info[1].hdb_field_no;
	TAGs_hfield_no[1] = table_fields_info[2].hdb_field_no;
	TAGs_hfield_no[2] = table_fields_info[5].hdb_field_no;

	Ret = pTaosSyn_obj->InsertNRecordtoNTablebyBuf(
		data_buf,				//�����buf
		field_num,				//buf�ﵥ����¼����
		record_num,				//buf���¼��
		table_fields_info,		//����Ϣ
		"lzj",					//��д��Ķ�Ӧ���ݿ�
		"lzj_meter",			//��������ش��ڣ��ǰ���¼����Ӧ�ӱ�
		tbname_hfield_no,		//������¼��Ҫ����������ʷ���
		values_hfield_no,		//������¼��������ֵ����ʷ�������
		TAGs_hfield_no,			//������¼����TAG���ݵ���ʷ������飬����TAGֵȫ���������TAGsNameȱʡ��
		tagsnum					//TAG����
	);


	//�ͷſռ�
	FREE((char*&)table_fields_info);
	free(data_buf);

}

void testInsertNRecordtoNTablebyBuf2(CTaosSyn*& pTaosSyn_obj)
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
	const int field_num = 4;//��4����
	char chEngName[field_num][32];//32Ϊ��Ӣ�ĳ���
	TABLE_HEAD_FIELDS_INFO* table_fields_info = (TABLE_HEAD_FIELDS_INFO*)malloc(sizeof(TABLE_HEAD_FIELDS_INFO));
	if (table_fields_info == NULL)
		return;

	//��Ҫ���Ӣ����
	strcpy(chEngName[0], "sw_id");
	strcpy(chEngName[1], "sw_name");
	strcpy(chEngName[2], "last_clear_gzp_time");
	strcpy(chEngName[3], "yx_value");
	//���巵��buf,��ʼ����С����
	char* data_buf = (char*)malloc(6);

	//ͨ��Ӣ��������ȡ�������Ϣ�ͼ�¼���ݼ�
	Ret = rdb_net.read_table_data_by_english_names_net(
		SW_INFO_NO,
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

	//�ӿ�ʹ�ò���
	const int valuesnum = 1;//�����г���ʱ���������ֵ�����
	const int tagsnum = 2;//��Ҫ��tagֵ�ĳ���tags����
	short tdname_hfield_no = table_fields_info[0].hdb_field_no;//����������ʷ���
	short values_hfield_no[valuesnum];//�������ֵ�����ʷ������飬���ڽӿ��ڲ����Ҷ�Ӧ����
	values_hfield_no[0] = table_fields_info[3].hdb_field_no;

	short TAGs_hfield_no[tagsnum];//���tagֵ�����ʷ�������
	TAGs_hfield_no[0] = table_fields_info[1].hdb_field_no;
	TAGs_hfield_no[1] = table_fields_info[2].hdb_field_no;

	const char** TAGsName = (const char**)malloc(tagsnum * sizeof(char*));
	char tag1[12] = "sw_name";
	char tag2[22] = "last_clear_gzp_time";
	TAGsName[0] = tag1;
	TAGsName[1] = tag2;


	Ret = pTaosSyn_obj->InsertNRecordtoNTablebyBuf(
		data_buf,				//�����buf
		field_num,				//buf�ﵥ����¼����
		record_num,				//buf���¼��
		table_fields_info,		//����Ϣ
		"lzj",					//��д��Ķ�Ӧ���ݿ�
		"testsw_meter",			//��������ش��ڣ��ǰ���¼����Ӧ�ӱ�
		tdname_hfield_no,		//������¼��Ҫ����������ʷ���
		values_hfield_no,		//������¼��������ֵ����ʷ�������
		TAGs_hfield_no,			//������¼����TAG���ݵ���ʷ������飬����TAGֵȫ���������TAGsNameȱʡ��
		tagsnum,				//TAG����
		TAGsName
	);


	//�ͷſռ�
	FREE((char*&)table_fields_info);
	free(data_buf);

}


void testInsertNRecordtoNTablebyBufofSample(CTaosSyn*& pTaosSyn_obj)
{
	int Ret = -1;

	int record_num = 0;
	int result_len = 0;
	int recodeIdx = 0;
	int fieldIdx = 0;

	//���ƫ�������飬����������������
	int shOffset[16] = { 0 };
	const int field_num = 4;//��4����
	char chEngName[field_num][32];//32Ϊ��Ӣ�ĳ���
	TABLE_HEAD_FIELDS_INFO* table_fields_info = (TABLE_HEAD_FIELDS_INFO*)malloc(sizeof(TABLE_HEAD_FIELDS_INFO));
	if (table_fields_info == NULL)
		return;

	//��Ҫ���Ӣ����
	strcpy(chEngName[0], "pe_id");
	strcpy(chEngName[1], "pe_name");
	strcpy(chEngName[2], "sample_model");
	strcpy(chEngName[3], "sample_interval");
	//���巵��buf,��ʼ����С����
	char* data_buf = (char*)malloc(6);

	//ͨ��Ӣ��������ȡ�������Ϣ�ͼ�¼���ݼ�
	Ret = rdb_net.read_table_data_by_english_names_net(
		PE_INFO_NO,
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

	//�ӿ�ʹ�ò���
	const int valuesnum = 1;//�����г���ʱ���������ֵ�����
	const int tagsnum = 2;//��Ҫ��tagֵ�ĳ���tags����
	short tdname_hfield_no = table_fields_info[0].hdb_field_no;//����������ʷ���
	short values_hfield_no[valuesnum];//�������ֵ�����ʷ������飬���ڽӿ��ڲ����Ҷ�Ӧ����
	values_hfield_no[0] = table_fields_info[1].hdb_field_no;

	short TAGs_hfield_no[tagsnum];//���tagֵ�����ʷ�������
	TAGs_hfield_no[0] = table_fields_info[2].hdb_field_no;
	TAGs_hfield_no[1] = table_fields_info[3].hdb_field_no;

	short Sample_no[2];//��Ų�����ʽ������
	Sample_no[0] = 29;
	Sample_no[1] = 19;


	Ret = pTaosSyn_obj->InsertNRecordtoNTablebyBufofSample(
		data_buf,				//�����buf
		field_num,				//buf�ﵥ����¼����
		record_num,				//buf���¼��
		table_fields_info,		//����Ϣ
		"lzj",					//��д��Ķ�Ӧ���ݿ�
		"pe_meter",			//��������ش��ڣ��ǰ���¼����Ӧ�ӱ�
		tdname_hfield_no,		//������¼��Ҫ����������ʷ���
		values_hfield_no,		//������¼��������ֵ����ʷ�������
		TAGs_hfield_no,			//������¼����TAG���ݵ���ʷ������飬����TAGֵȫ���������TAGsNameȱʡ��
		tagsnum,				//TAG����
		Sample_no
	);


	//�ͷſռ�
	FREE((char*&)table_fields_info);
	free(data_buf);
}