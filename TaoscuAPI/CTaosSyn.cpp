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
}

int CTaosSyn::InitAccess(const CTaosSyn* TaosSyn_obj, TAOS* & pTaos)
{

	//����������ݿ�ı���ֵ�Ƿ����
	if (m_host == "" || m_user == "" || m_pass == "")
	{
		return VARIABLE_NO_VALUE_TAOS;
	}

	//m_ptaos = taos_connect("Linx.Linx-lzj", "root", "taosdata", NULL, 6030);//��˼���
	m_ptaos = taos_connect(TaosSyn_obj->m_host.c_str(), TaosSyn_obj->m_user.c_str(), TaosSyn_obj->m_pass.c_str(), TaosSyn_obj->m_db.c_str(), TaosSyn_obj->m_port);//���ݿ���

	pTaos = m_ptaos;

	if (m_ptaos)
		return SUCCESS_TAOS;
	else
		return HANDLE_FAILED_TAOS;
}

int CTaosSyn::ExecuteOneQueryDirectofRecordBytes(const char* szSqlSen, char*& szResult, int& Record_num, int& Onerecordlen, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;			//������ֽ���
	TAOS_ROW row;			//һ�м�¼����

	Record_num = 0;
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
		return RES_QUERY_FAILED;
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

	Record_num = 0;
	//��ȡsql���ݷ��ؽ����
	res = taos_query(m_ptaos, szSqlSen);

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
