#ifndef _WINDOWS
	#include <unistd.h>
#endif
#include "CTaosSyn.h"

CTaosSyn::CTaosSyn()
{
	m_ptaos = NULL;
	m_host.clear();
	m_user.clear();
	m_pass.clear();
	m_db.clear();
	m_port = 6030;
}

CTaosSyn::~CTaosSyn()
{
	//�ͷ�taos���
	taos_close(m_ptaos);
	taos_cleanup();
}

int CTaosSyn::InitAccess(const CTaosSyn* TaosSyn_obj)
{
	//����������ݿ�ı���ֵ�Ƿ����
	if (m_host == "" || m_user == "" || m_pass == "")
	{
		return VARIABLE_NO_VALUE_TAOS;
	}
	//m_ptaos = taos_connect("Linx.Linx-lzj", "root", "taosdata", NULL, 6030);//��˼���
	m_ptaos = taos_connect(TaosSyn_obj->m_host.c_str(), TaosSyn_obj->m_user.c_str(), TaosSyn_obj->m_pass.c_str(), TaosSyn_obj->m_db.c_str(), TaosSyn_obj->m_port);//���ݿ���
	if (m_ptaos)
		return SUCCESS_TAOS;
	else
		return HANDLE_FAILED_TAOS;
}

int CTaosSyn::ExecuteSqlCtlDB(const char* sqlstr)
{
	//��ȡsql���ݷ��ؽ����
	TAOS_RES* res;			//������ֽ���,����д���޷�������
	res = taos_query(m_ptaos, sqlstr);
	//��������Ƿ���������
	if (res == NULL || taos_errno(res) != 0) {
		taos_free_result(res);
		return RES_EXE_FAILED;
	}

	return SUCCESS_TAOS;
}
int CTaosSyn::ExecuteSqlCtlDB(const char* sqlstr, TAOS_RES*& Res)
{

	//��ȡsql���ݷ��ؽ����
	TAOS_RES* res;			//������ֽ���,����д���޷�������
	res = taos_query(m_ptaos, sqlstr);
	//��������Ƿ���������
	if (res == NULL || taos_errno(res) != 0) {
		taos_free_result(res);
		return RES_EXE_FAILED;
	}

	Res = res;
	
	return SUCCESS_TAOS;
}

int CTaosSyn::ExecuteOneQueryDirectofRecordBytes(const char* szSqlSen, char*& szResult, int& Record_num, int& Onerecordlen, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;			//������ֽ���
	TAOS_ROW row;			//һ�м�¼����

	//���ر����ĳ�ʼ��
	szResult = NULL;
	Onerecordlen = 0;
	Record_num = 0;
	fields_info = 0;
	fields_num = 0;

	int blockrecord_num = 0;
	//��ȡsql���ݷ��ؽ����
	res = taos_query(m_ptaos, szSqlSen);
	while ((blockrecord_num = taos_fetch_block(res, &row)))
		Record_num += blockrecord_num;
	//Record_num = taos_fetch_block(res, &row);//���ﲻҪ��ģ�����Ϊ�˻�ȡ��¼��
	res = taos_query(m_ptaos, szSqlSen);//����Ϊɶ���»�ȡһ�Σ���Ϊ����block������ȡ�߾�û��,���ʱ��ܿ�Ͳ�����
	//��������Ƿ���������
	if (res == NULL || taos_errno(res) != 0) {
		//printf("failed to select, reason:%s\n", taos_errstr(res));
		taos_free_result(res);
		return RES_EXE_FAILED;
	}

	//������л�ȡ����Ϣ����
	fields_info = taos_fetch_fields(res);
	//������л�ȡ�����
	fields_num = taos_num_fields(res);
	//����һ����¼����
	for (int i = 0; i < fields_num; i++)
	{
		Onerecordlen += fields_info[i].bytes;
	}

	//���szResult�ռ䣬���¿������м�¼����ռ�
	if (szResult != NULL)
	{
		free(szResult);
		szResult = (char*)malloc(Record_num * Onerecordlen);
	}
	else
		szResult = (char*)malloc(Record_num * Onerecordlen);
	
	//������ʱ����һ����¼��ת������ֽ���
	char *buf_Onerecord = (char*)malloc(Onerecordlen);
	
	//���м�¼���ݰ� ��Ӧ�����������ֽ��� ��ʽ����szResult��
	for (int i = 0; i < Record_num; i++)
	{
		//ÿ�µ�һ����¼������ʱ��¼�ֽ����ÿ�
		memcpy(buf_Onerecord, "", Onerecordlen);
		row = taos_fetch_row(res);

		//����ת�ַ���Ϊ�˱Ƚ�һ��
		char str[100] = { 0 };
		taos_print_row(str, row, fields_info, fields_num);

		//������row��û��ת����ʽ���ֽ��������ֶ�ת���ɶ�Ӧ���͵ı���������szResult�ֽ���
		int field_Offset = 0;// ��ƫ����
		for (int j = 0; j < fields_num; j++)
		{
			if (row[j] == NULL)
				continue;

			////������ʱ����һ����ת������ֽ�����Ҳ����ͨ�����ַ�ʽ�ȿ���һ�����������ռ䣬���ŵ����������buf�������ﲻӰ��ʱ�临�Ӷȣ�����ν��
			//char *buf_Onefield = (char*)malloc()

			switch (fields_info[j].type)// �����ֶ����ͽ��д���
			{
			case TSDB_DATA_TYPE_BOOL:// 1
			case TSDB_DATA_TYPE_TINYINT:// 2
				{
					int8_t tmp = *(int8_t*)row[j];//������ʱ���ն�Ӧ��ʽ����
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int8_t));//������ֵ�������¼�ֽ�����
					field_Offset += sizeof(int8_t);//ƫ��������
				}
				break;
			case TSDB_DATA_TYPE_UTINYINT:// 11
				{
					uint8_t tmp = *(uint8_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint8_t));
					field_Offset += sizeof(uint8_t);
				}
				break;
			case TSDB_DATA_TYPE_SMALLINT:// 3
				{
					int16_t tmp = *(int16_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int16_t));
					field_Offset += sizeof(int16_t);
				}
				break;
			case TSDB_DATA_TYPE_USMALLINT:// 12
				{
					uint16_t tmp = *(uint16_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint16_t));
					field_Offset += sizeof(uint16_t);
				}
				break;
			case TSDB_DATA_TYPE_INT:// 4
				{
					int32_t tmp = *(int32_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int32_t));
					field_Offset += sizeof(int32_t);
				}
				break;
			case TSDB_DATA_TYPE_UINT://13
				{
					uint32_t tmp = *(uint32_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint32_t));
					field_Offset += sizeof(uint32_t);
				}
				break;
			case TSDB_DATA_TYPE_BIGINT:// 5
			case TSDB_DATA_TYPE_TIMESTAMP:// 9
				{
					int64_t tmp = *(int64_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int64_t));
					field_Offset += sizeof(int64_t);
				}
				break;
			case TSDB_DATA_TYPE_UBIGINT:// 14
				{
					uint64_t tmp = *(uint64_t*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint64_t));
					field_Offset += sizeof(uint64_t);
				}
				break;
			case TSDB_DATA_TYPE_FLOAT:// 6
				{
					float tmp = *(float*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(float));
					field_Offset += sizeof(float);
				}
				break;
			case TSDB_DATA_TYPE_DOUBLE:// 7
				{
					double tmp = *(double*)row[j];
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(double));
					field_Offset += sizeof(double);
				}
				break;
			case TSDB_DATA_TYPE_BINARY:// 8
			case TSDB_DATA_TYPE_NCHAR:// 10
				{
					//����ֶ�����ΪBINARY��NCHAR
					/*
					�������һ��BINARY��NCHAR
					BINARY����¼���ֽ��ַ���������ֻ���ڴ��� ASCII �ɼ��ַ������ĵȶ��ֽ��ַ���ʹ�� NCHAR
					NCHAR����¼�������ֽ��ַ����ڵ��ַ������������ַ���ÿ�� NCHAR �ַ�ռ�� 4 �ֽڵĴ洢�ռ䡣�ַ�������ʹ�õ��������ã��ַ����ڵĵ���������ת���ַ� \'��
						   NCHAR ʹ��ʱ��ָ���ַ�����С������Ϊ NCHAR(10) ���б�ʾ���е��ַ������洢 10 �� NCHAR �ַ�������û��ַ������ȳ����������ȣ����ᱨ��
					*/
					int32_t charLen = ((uint16_t*)((char*)row[j] - sizeof(uint16_t)))[0];//��ȡ�ֶ�ֵ���ȣ�uint16_t��VarDataLenT������taos�ٷ���ô��
					if (fields_info[j].type == TSDB_DATA_TYPE_BINARY)
					{
						if (charLen > fields_info[j].bytes || charLen < 0)//ȷ����������Ч��Χ��
							return BINARY_OR_NCHAR_LEN_OVERFLOW;
					}
					else
						if (charLen > fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//ȷ����������Ч��Χ��,�����int32_t��Ϊÿ���ַ�ռ4�ֽڣ���ΪΪ�˿ɴ��������ֵ���2�ֽڵ��ַ�
							return BINARY_OR_NCHAR_LEN_OVERFLOW;

					//��Ϊ����char�������ʵ�ַ����ȣ�Ϊ�˷��㣬����ת�ɶ���ʱ�򳤶�
					char* vchar = (char*)malloc(fields_info[j].bytes);//������ʱ���峤�Ƚ���
					memset(vchar, 0, fields_info[j].bytes);//ȫ����0
					memcpy(vchar, (char*)row[j], charLen);//����ǰ����ʵ������

					memcpy(buf_Onerecord + field_Offset, vchar, fields_info[j].bytes);//����ʱ�����뵥��¼buf
					field_Offset += fields_info[j].bytes;

					free(vchar);
				}
				break;
			default:
				break;
			}

		}
		//��һ����ת�����͵ļ�¼�ֽ������buf�ֽ�����
		memcpy(szResult + Onerecordlen * i, (char*)buf_Onerecord, Onerecordlen);
	}


	////�������Դ���ֽ����Ƿ���ȷ
	//int Offset = 0;
	//int64_t ts = 0;
	//double value = 0;
	//char name[60] = { 0 };
	//int type = 0;
	//for (int i = 0; i < Record_num; i++)//��¼����������¼���ȣ���ƫ����
	//{
	//	Offset = 0;
	//	memcpy((char*)&ts, szResult + Onerecordlen * i, fields_info[0].bytes);
	//	Offset += fields_info[0].bytes;
	//	memcpy((char*)&value, szResult + Onerecordlen * i + Offset, fields_info[1].bytes);
	//	Offset += fields_info[1].bytes;
	//	memcpy(name, szResult + Onerecordlen * i + Offset, fields_info[2].bytes);
	//	Offset += fields_info[2].bytes;
	//	memcpy((char*)&type, szResult + Onerecordlen * i + Offset, fields_info[3].bytes);
	//	//sscanf(szResult + Onerecordlen * i + value_Offset, "%f", &value);
	//}


	free(buf_Onerecord);
	return SUCCESS_TAOS;
}

int CTaosSyn::ExecuteOneQueryDirectofRecordList(const char* szSqlSen, char**& Result, int& Record_num, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;			//������ֽ���
	TAOS_ROW row;			//һ�м�¼����
	int Onerecordlen = 0;	//һ����¼����

	//���ر�����ʼ��
	Result = NULL;
	Record_num = 0;
	fields_info = NULL;
	fields_num = 0;

	//��ȡsql���ݷ��ؽ����
	res = taos_query(m_ptaos, szSqlSen);
	//��������Ƿ���������
	if (res == NULL || taos_errno(res) != 0) {
		//printf("failed to select, reason:%s\n", taos_errstr(res));
		taos_free_result(res);
		return RES_EXE_FAILED;
	}

	//����ʱ���ÿ����¼���ݵ�ָ��
	vector<char*>	ptr_vect;

	//������л�ȡ����Ϣ����
	fields_info = taos_fetch_fields(res);
	//������л�ȡ�����
	fields_num = taos_num_fields(res);
	//����һ����¼����
	for (int i = 0; i < fields_num; i++)
	{
		Onerecordlen += fields_info[i].bytes;
	}

	while ((row = taos_fetch_row(res)))
	{
		//������ʱ����һ����¼��ת������ֽ���
		char* buf_Onerecord = (char*)malloc(Onerecordlen);
		ptr_vect.push_back(buf_Onerecord);//����vector

		//������row��û��ת����ʽ���ֽ��������ֶ�ת���ɶ�Ӧ���͵ı���������szResult�ֽ���
		int field_Offset = 0;// ��ƫ����
		for (int j = 0; j < fields_num; j++)
		{
			if (row[j] == NULL)
				continue;

			////������ʱ����һ����ת������ֽ���
			//char *buf_Onefield = (char*)malloc()

			switch (fields_info[j].type)// �����ֶ����ͽ��д���
			{
			case TSDB_DATA_TYPE_BOOL:// 1
			case TSDB_DATA_TYPE_TINYINT:// 2
			{
				int8_t tmp = *(int8_t*)row[j];//������ʱ���ն�Ӧ��ʽ����
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int8_t));//������ֵ�������¼�ֽ�����
				field_Offset += sizeof(int8_t);//ƫ��������
			}
			break;
			case TSDB_DATA_TYPE_UTINYINT:// 11
			{
				uint8_t tmp = *(uint8_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint8_t));
				field_Offset += sizeof(uint8_t);
			}
			break;
			case TSDB_DATA_TYPE_SMALLINT:// 3
			{
				int16_t tmp = *(int16_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int16_t));
				field_Offset += sizeof(int16_t);
			}
			break;
			case TSDB_DATA_TYPE_USMALLINT:// 12
			{
				uint16_t tmp = *(uint16_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint16_t));
				field_Offset += sizeof(uint16_t);
			}
			break;
			case TSDB_DATA_TYPE_INT:// 4
			{
				int32_t tmp = *(int32_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int32_t));
				field_Offset += sizeof(int32_t);
			}
			break;
			case TSDB_DATA_TYPE_UINT://13
			{
				uint32_t tmp = *(uint32_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint32_t));
				field_Offset += sizeof(uint32_t);
			}
			break;
			case TSDB_DATA_TYPE_BIGINT:// 5
			case TSDB_DATA_TYPE_TIMESTAMP:// 9
			{
				int64_t tmp = *(int64_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int64_t));
				field_Offset += sizeof(int64_t);
			}
			break;
			case TSDB_DATA_TYPE_UBIGINT:// 14
			{
				uint64_t tmp = *(uint64_t*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(uint64_t));
				field_Offset += sizeof(uint64_t);
			}
			break;
			case TSDB_DATA_TYPE_FLOAT:// 6
			{
				float tmp = *(float*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(float));
				field_Offset += sizeof(float);
			}
			break;
			case TSDB_DATA_TYPE_DOUBLE:// 7
			{
				double tmp = *(double*)row[j];
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(double));
				field_Offset += sizeof(double);
			}
			break;
			case TSDB_DATA_TYPE_BINARY:// 8
			case TSDB_DATA_TYPE_NCHAR:// 10
			{
				//����ֶ�����ΪBINARY��NCHAR
				/*
				�������һ��BINARY��NCHAR
				BINARY����¼���ֽ��ַ���������ֻ���ڴ��� ASCII �ɼ��ַ������ĵȶ��ֽ��ַ���ʹ�� NCHAR
				NCHAR����¼�������ֽ��ַ����ڵ��ַ������������ַ���ÿ�� NCHAR �ַ�ռ�� 4 �ֽڵĴ洢�ռ䡣�ַ�������ʹ�õ��������ã��ַ����ڵĵ���������ת���ַ� \'��
					   NCHAR ʹ��ʱ��ָ���ַ�����С������Ϊ NCHAR(10) ���б�ʾ���е��ַ������洢 10 �� NCHAR �ַ�������û��ַ������ȳ����������ȣ����ᱨ��
				*/
				int32_t charLen = ((uint16_t*)((char*)row[j] - sizeof(uint16_t)))[0];//��ȡ�ֶ�ֵ���ȣ�uint16_t��VarDataLenT������taos�ٷ���ô��
				if (fields_info[j].type == TSDB_DATA_TYPE_BINARY)
				{
					if (charLen > fields_info[j].bytes || charLen < 0)//ȷ����������Ч��Χ��
						return BINARY_OR_NCHAR_LEN_OVERFLOW;
				}
				else
					if (charLen > fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//ȷ����������Ч��Χ��,�����int32_t��Ϊÿ���ַ�ռ4�ֽڣ���ΪΪ�˿ɴ��������ֵ���2�ֽڵ��ַ�
						return BINARY_OR_NCHAR_LEN_OVERFLOW;

				//��Ϊ����char�������ʵ�ַ����ȣ�Ϊ�˷��㣬����ת�ɶ���ʱ�򳤶�
				char* vchar = (char*)malloc(fields_info[j].bytes);//������ʱ���峤�Ƚ���
				memset(vchar, 0, fields_info[j].bytes);//ȫ����0
				memcpy(vchar, (char*)row[j], charLen);//����ǰ����ʵ������

				memcpy(buf_Onerecord + field_Offset, vchar, fields_info[j].bytes);//����ʱ�����뵥��¼buf
				field_Offset += fields_info[j].bytes;

				free(vchar);
			}
			break;
			default:
				break;
			}

		}

		Record_num++;
	}

	if (Result != NULL)
	{
		free(Result);
		//������ά��ַָ���ſռ�
		Result = (char**)malloc(Record_num * sizeof(char*));
	}
	else
		Result = (char**)malloc(Record_num * sizeof(char*));



	for (int i = 0; i < Record_num; i++)
	{
		Result[i] = ptr_vect[i];
	}

	return SUCCESS_TAOS;
}

int CTaosSyn::FreePtP(char**& Result, int& Record_num)
{
	if (Record_num < 0)
		return CURRENCY_ERROR;

	for (int i = 0; i < Record_num; ++i) {
		free(Result[i]);
	}
	free(Result);
	return SUCCESS_TAOS;
}

int CTaosSyn::ExecuteInsertNRecordbyFile(const char* tbname, const char* FileLoc, int TAGNum, char** TAGsName)
{
	//INSERT INTO meters (current, voltage) VALUES (10.2, 219);
	string sql = "INSERT INTO ";
	sql.append(tbname);
	sql.append(" (");
	for (int i = 0; i < TAGNum; i++)
	{
		sql += TAGsName[i];
		if(i!=TAGNum-1)
			sql += ", ";
	}
	sql += ") VALUES (";

	ifstream file(FileLoc);
	if (!file)
		return OPEN_FIEL_FAILED;

	string line;
	string subchar;
	char split = ',';//�ָ���

	int flagiftimestmp = 1;//�״��ж��ļ��Ƿ��ʱ��
	int chkiftimestmp;//����ļ����������Ƿ���ʱ���
	int overplus_fields = TAGNum;//�ļ�����ʣ������
	while (getline(file, line))
	{
		char* cstr = new char[line.length() + 1];
		strcpy(cstr, line.c_str()); // ��stringת��ΪC�ַ���

		subchar = strtok(cstr, &split);
		if (subchar[0] == '#')
			continue;

		//�ж��Ƿ���ʱ������У��оͼ��룬û�оͰ���ǰʱ��
		chkiftimestmp = 0;
		if (1 == flagiftimestmp)
		{
			for (int i = 0; i < subchar.size(); i++)
				if (subchar[i] == '-' || subchar[i] == ':')
					chkiftimestmp++;
			flagiftimestmp = 0;
		}
		if (4 == chkiftimestmp)
		{
			sql += subchar.c_str();
			overplus_fields--;
		}
		else
		{
			sql += "now, ";
			sql += subchar.c_str();
			overplus_fields -= 2;
			
		}
		if(overplus_fields > 0)
			sql += ", ";

		for (int i = 0; i < overplus_fields; i++)
		{
			subchar = strtok(NULL, &split);
			sql += subchar.c_str();
			if(overplus_fields -1 != i)
				sql += ", ";
		}

		sql += ')';

		TAOS_RES* res = 0;
		res = taos_query(m_ptaos, sql.c_str());
		if (res == NULL)
			return 0;

		free(cstr);
	}
	file.close();




	return SUCCESS_TAOS;
}

int CTaosSyn::ExecuteInsertNRecordbyBuf(const char* buf, const int fields_num, const int record_num, const TABLE_HEAD_FIELDS_INFO* table_fields_info, const char* dbname, const char* tbname, const char* stbname, const char** TAGsValue, const int TAGsNum, const char** TAGsName)
{

	//��ȡsql���ݷ��ؽ����
	TAOS_RES* res;			//������ֽ���,����д���޷�������

	string sq = "use ";
	sq += dbname;
	res = taos_query(m_ptaos, sq.c_str());
	//��������Ƿ���������
	if (res == NULL || taos_errno(res) != 0) {
		
		return ACCESS_DB_FAILED;
	}
	taos_free_result(res);
	res = NULL;

	bool ifstb = false;
	bool ifallTAG = false;
	string tmpsql1;
	string tmpsql2;
	//����г�������׷�ӳ�����ͨ�����
	if (0 != stbname)
	{
		ifstb = true;

		//�����TAG���飬�Ͳ���ȫ��TAG�������TAG����TAGֵ
		if (0 != TAGsName)
		{
			tmpsql1 += " USING ";
			tmpsql1 += stbname;
			tmpsql1 += ' ';
			tmpsql1 += '(';
			for (int i = 0; i < TAGsNum; i++)
			{
				tmpsql1 += TAGsName[i];
				if (i < TAGsNum - 1)
					tmpsql1 += ", ";
			}
			tmpsql1 += ") ";

			tmpsql1 += "TAGS (";
			for (int i = 0; i < TAGsNum; i++)
			{
				tmpsql1 += TAGsValue[i];
				if (i < TAGsNum - 1)
					tmpsql1 += ", ";
			}
			tmpsql1 += ") ";
		}
		else//����ȫ��TAG����ֱ�����TAGֵ����
		{
			ifallTAG = true;

			tmpsql2 += " USING ";
			tmpsql2 += stbname;
			tmpsql2 += ' ';
			tmpsql2 += "TAGS (";
			for (int i = 0; i < TAGsNum; i++)
			{
				tmpsql2 += TAGsValue[i];
				if (i < TAGsNum - 1)
					tmpsql2 += ", ";
			}
			tmpsql2 += ") ";
		}
	}
		
	

	int nfields_num = fields_num;
	int nrecord_num = record_num;
	//��ȡ�ɹ�����ʼʹ�ñ������Ϣ
	//��¼һ����¼���Ⱥ���ƫ����
	int nOneRecordLen = 0;
	int fieldIdx = 0;
	int recordIdx = 0;
	//���ƫ�������飬����������������
	vector<int>	shOffset;//����ÿ�����ƫ����
	vector<string> ntmpstr;//����ÿ�������ַ���
	for (fieldIdx = 0; fieldIdx < nfields_num; fieldIdx++)
	{
		if (fieldIdx == 0)
			shOffset.push_back(0);
		else
			shOffset.push_back(table_fields_info[fieldIdx - 1].field_len + shOffset[fieldIdx - 1]);
		
		nOneRecordLen += table_fields_info[fieldIdx].field_len;

		//˳�㶨��һЩ��ʱ�ִ�����������
		string str;
		ntmpstr.push_back(str);
	}

	bool if_first_tb = true;
	string sqlstr;
	//�����ַ�������
	for (recordIdx = 0; recordIdx < record_num; recordIdx++)
	{
		sqlstr = "INSERT INTO ";
		sqlstr += tbname;
		int fieldOffset = 0;
		for (fieldIdx = 0; fieldIdx < nfields_num; fieldIdx++)
		{
			int field_type = table_fields_info[fieldOffset].data_type;

			ntmpstr[fieldIdx] = "";
			switch (field_type)
			{
			case DB_DATA_TYPE_NULL: {//0 δ֪����
				return DATA_TYPE_ERROR;
			}
				break;
			case DB_DATA_TYPE_STRING: {//2�ַ���	
				char str[DB_CHN_DEVICE_NAME_LEN] = { 0 };
				memcpy(str, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;
				ntmpstr[fieldIdx] = str;
			}
				break;
			case DB_DATA_TYPE_CHAR: {//3 �ַ�
				char chr = 0;
				memcpy((char*)&chr, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx] = chr;
			}
				break;
			case DB_DATA_TYPE_SHORT: {//4 ������
				short tmp = 0;
				memcpy((char*)&tmp, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx]  = to_string(tmp);
			}
				break;
			case DB_DATA_TYPE_INT: {//5 32λ����
				int tmp = 0;
				memcpy((char*)&tmp, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx] = to_string(tmp);
			}
				break;
			case DB_DATA_TYPE_FLOAT: {//6 �����ȸ�����
				float tmp = 0;
				memcpy((char*)&tmp, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx] = to_string(tmp);
			}
				break;
			case DB_DATA_TYPE_DOUBLE: {//7 ˫���ȸ�����
				double tmp = 0;
				memcpy((char*)&tmp, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx] = to_string(tmp);
			}
				break;
			case DB_DATA_TYPE_DATETIME: {//9 ʱ����
				return DATA_TYPE_ERROR;
			}
				break;
			case DB_DATA_TYPE_BINARY: {//11 �����ƴ�
				return DATA_TYPE_ERROR;
			}
				break;
			case DB_DATA_TYPE_INT64: {//12 64λ����
				int64_t tmp = 0;
				memcpy((char*)&tmp, buf + recordIdx * nOneRecordLen + shOffset[fieldOffset], table_fields_info[fieldOffset].field_len);
				fieldOffset++;

				ntmpstr[fieldIdx] = to_string(tmp);
			}
				break;
			default:
				break;
			}

		}

		if (ifstb == true && if_first_tb == true)
		{
			if (ifallTAG)
				sqlstr += tmpsql2;
			else
				sqlstr += tmpsql1;

			if_first_tb = false;
		}
			


		sqlstr += " VALUES (now, ";
		for (int i = 0; i < nfields_num; i++)
		{
			sqlstr += ntmpstr[i];
			if (i < nfields_num - 1)
				sqlstr += ", ";
		}
		sqlstr += ')';
			

		
		res = taos_query(m_ptaos, sqlstr.c_str());
		//��������Ƿ���������
		if (res == NULL || taos_errno(res) != 0) {
			taos_free_result(res);
			return RES_EXE_FAILED;
		}
#ifdef _WINDOWS
		Sleep(1);
#else
		usleep(1000);
#endif // _WINDOWS

		
	}

	ntmpstr.clear();

	return SUCCESS_TAOS;
}



