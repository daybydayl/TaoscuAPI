#include "CCommonHead.h"
#include "CTaosSyn.h"

//测试已完成api
void testmaster(CTaosSyn*& pTaosSyn_obj);
void testExecuteOneQueryDirectofRecordBytes(CTaosSyn*& pTaosSyn_obj);
void testExecuteOneQueryDirectofRecordList(CTaosSyn*& pTaosSyn_obj);

int main(int argc, char* argv[])
{
	int ret = -1;
	CTaosSyn* pTaosSyn_obj = new CTaosSyn();
	pTaosSyn_obj->m_host = "lzj-VM";
	pTaosSyn_obj->m_user = "root";
	pTaosSyn_obj->m_pass = "taosdata";
	pTaosSyn_obj->m_db = "";
	pTaosSyn_obj->m_port = 6030;
	TAOS* pTaos = NULL;

	ret = pTaosSyn_obj->InitAccess(pTaosSyn_obj, pTaos);
	if (0 != ret)
		return -1;

	
	//测试已完成api
	testmaster(pTaosSyn_obj);
	

	

	return 0;
}

void testmaster(CTaosSyn*& pTaosSyn_obj)
{
	testExecuteOneQueryDirectofRecordBytes(pTaosSyn_obj);
	//testExecuteOneQueryDirectofRecordList(pTaosSyn_obj);
}

void testExecuteOneQueryDirectofRecordBytes(CTaosSyn*& pTaosSyn_obj)
{
	char* szResult = NULL;
	int Record_num;
	int Onerecordlen;
	TAOS_FIELD* fields_info;
	int fields_num = 0;
	pTaosSyn_obj->ExecuteOneQueryDirectofRecordBytes("select TO_ISO8601(ts, '+00:00'),meas_value,meas_name,meas_type from rr6000.meas_meter1", szResult, Record_num, Onerecordlen, fields_info, fields_num);

	//解析测试存放字节流是否正确
	int Offset = 0;
	char date[65] = { 0 };
	int64_t ts = 0;
	double value = 0;
	char name[60] = { 0 };
	int type = 0;
	for (int i = 0; i < Record_num; i++)//记录数，单挑记录长度，域偏移量
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

void testExecuteOneQueryDirectofRecordList(CTaosSyn*& pTaosSyn_obj)
{

	char** Result = NULL;
	int Record_num;
	TAOS_FIELD* fields_info;
	int fields_num = 0;
	pTaosSyn_obj->ExecuteOneQueryDirectofRecordList("select TO_ISO8601(ts, '+00:00'),meas_value,meas_name,meas_type from rr6000.meas_meter1", Result, Record_num, fields_info, fields_num);


	//解析测试存放字节流是否正确
	int Offset = 0;
	char date[65] = { 0 };
	int64_t ts = 0;
	double value = 0;
	char name[60] = { 0 };
	int type = 0;
	for (int i = 0; i < Record_num; i++)//记录数，单挑记录长度，域偏移量
	{
		Offset = 0;
		memcpy(date, Result[i] + Offset, fields_info[0].bytes);
		Offset += fields_info[0].bytes;
		memcpy((char*)&value, Result[i] + Offset, fields_info[1].bytes);
		Offset += fields_info[1].bytes;
		memcpy(name, Result[i] + Offset, fields_info[2].bytes);
		Offset += fields_info[2].bytes;
		memcpy((char*)&type, Result[i] + Offset, fields_info[3].bytes);
		//sscanf(szResult + Onerecordlen * i + value_Offset, "%f", &value);
	}
	pTaosSyn_obj->FreePtP(Result, Record_num);
}
