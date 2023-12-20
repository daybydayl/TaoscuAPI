#include "CTaosASyn.h"

struct AnalysisRowStruct {
	char**&			szResult;
	int&			Record_num;
	int&			Onerecordlen;
	TAOS_FIELD*&	fields_info;
	int&			fields_num;
	int&			callbackRet;	//�ص������ķ���ֵ
	vector<char*>	ptr_vect;		//����ʱ���ÿ����¼���ݵ�ָ��
	// ���캯�������ڳ�ʼ�����ó�Ա
	AnalysisRowStruct(char**& rSzResult, int& rRecordNum, int& rOnerecordlen,TAOS_FIELD*& rFieldsInfo, int& rFieldsNum, int& rCallbackRet, vector<char*> rptr_vect)
		: szResult(rSzResult), Record_num(rRecordNum), Onerecordlen(rOnerecordlen),
		fields_info(rFieldsInfo), fields_num(rFieldsNum), callbackRet(rCallbackRet), ptr_vect(rptr_vect){
		// ���������߼�
	}

};


CTaosASyn::CTaosASyn()
{
	m_ptaos = NULL;
	m_host.clear();
	m_user.clear();
	m_pass.clear();
	m_db.clear();
	m_port = 6030;
}

CTaosASyn::~CTaosASyn()
{
	//�ͷ�taos���
	taos_close(m_ptaos);
	taos_cleanup();
}

int CTaosASyn::InitAccess(const CTaosASyn* TaosSyn_obj)
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

int CTaosASyn::ExecuteOneQueryDirectofRecordList(const char* szSqlSen, char**& szResult, int& Record_num, int& Onerecordlen, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;//������ֽ���
	int Ret = 0;//�����첽��ʵû�ã������Ż�

	//���ر����ĳ�ʼ��
	Onerecordlen = 0;
	Record_num = 0;
	fields_info = 0;
	fields_num = 0;

	vector<char*>	ptr_vect;//�ۼ��첽��ѯ��¼��vec
	AnalysisRowStruct* pAlyRowStru = new AnalysisRowStruct(szResult, Record_num, Onerecordlen, fields_info, fields_num, Ret, ptr_vect);

	//ֱ���첽��ȡ���ݼ�
	taos_query_a(m_ptaos, szSqlSen, GetBlock_CallBack, (void *)pAlyRowStru);//���ֻ������⣬�����ٿ���

	//delete pAlyRowStru;
	return Ret;
}

void CTaosASyn::GetBlock_CallBack(void* param, TAOS_RES* res, int code)
{
	AnalysisRowStruct* pAlyRowStru = static_cast<AnalysisRowStruct*>(param);

	if (code == 0 && res) {

		//������л�ȡ����Ϣ����
		pAlyRowStru->fields_info = taos_fetch_fields(res);
		//������л�ȡ�����
		pAlyRowStru->fields_num = taos_num_fields(res);

		//����һ����¼����
		for (int i = 0; i < pAlyRowStru->fields_num; i++)
		{
			pAlyRowStru->Onerecordlen += pAlyRowStru->fields_info[i].bytes;
		}

		// �첽API����ȡһ����¼��Ĭ�Ͽ��С4096
		taos_fetch_rows_a(res, AnalysisRow_CallBack, (void *)pAlyRowStru);
	}
	else {
		//printf("%s select failed, code:%d\n", pTable->name, code);
		pAlyRowStru->callbackRet = RES_EXE_FAILED;
		taos_free_result(res);
		taos_cleanup();
		exit(1);
	}

	

}

void CTaosASyn::AnalysisRow_CallBack(void* param, TAOS_RES* res, int numOfRows)
{
	AnalysisRowStruct* pAlyRowStru = static_cast<AnalysisRowStruct*>(param);
	TAOS_ROW row;			//һ�м�¼����
	if (numOfRows > 0)
	{
		for (int i = 0; i < numOfRows; i++)
		{
			row = taos_fetch_row(res);
			/*������¼*/
			//������ʱ����һ����¼��ת������ֽ���
			char* buf_Onerecord = (char*)malloc(pAlyRowStru->Onerecordlen);
			pAlyRowStru->ptr_vect.push_back(buf_Onerecord);//����vector

			//������row��û��ת����ʽ���ֽ��������ֶ�ת���ɶ�Ӧ���͵ı���������szResult�ֽ���
			int field_Offset = 0;// ��ƫ����
			for (int j = 0; j < pAlyRowStru->fields_num; j++)
			{
				if (row[j] == NULL)
					continue;

				switch (pAlyRowStru->fields_info[j].type)// �����ֶ����ͽ��д���
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
					if (pAlyRowStru->fields_info[j].type == TSDB_DATA_TYPE_BINARY)
					{
						if (charLen > pAlyRowStru->fields_info[j].bytes || charLen < 0)//ȷ����������Ч��Χ��
							pAlyRowStru->callbackRet = BINARY_OR_NCHAR_LEN_OVERFLOW;
					}
					else
						if (charLen > pAlyRowStru->fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//ȷ����������Ч��Χ��,�����int32_t��Ϊÿ���ַ�ռ4�ֽڣ���ΪΪ�˿ɴ��������ֵ���2�ֽڵ��ַ�
							pAlyRowStru->callbackRet = BINARY_OR_NCHAR_LEN_OVERFLOW;

					//��Ϊ����char�������ʵ�ַ����ȣ�Ϊ�˷��㣬����ת�ɶ���ʱ�򳤶�
					char* vchar = (char*)malloc(pAlyRowStru->fields_info[j].bytes);//������ʱ���峤�Ƚ���
					memset(vchar, 0, pAlyRowStru->fields_info[j].bytes);//ȫ����0
					memcpy(vchar, (char*)row[j], charLen);//����ǰ����ʵ������

					memcpy(buf_Onerecord + field_Offset, vchar, pAlyRowStru->fields_info[j].bytes);//����ʱ�����뵥��¼buf
					field_Offset += pAlyRowStru->fields_info[j].bytes;

					free(vchar);
				}
				break;
				default:
					break;
				}

			}


		}
		pAlyRowStru->Record_num += numOfRows;//�ۼӼ�¼��

		if (pAlyRowStru->Record_num%numOfRows!=0 || pAlyRowStru->Record_num==numOfRows)//���������˾Ϳ�����buf��С
		{
			if (pAlyRowStru->szResult != NULL)
			{
				free(pAlyRowStru->szResult);
				//������ά��ַָ���ſռ�
				pAlyRowStru->szResult = (char**)malloc(pAlyRowStru->Record_num * sizeof(char*));
			}
			else
				pAlyRowStru->szResult = (char**)malloc(pAlyRowStru->Record_num * sizeof(char*));

			for (int i = 0; i < pAlyRowStru->Record_num; i++)
			{
				pAlyRowStru->szResult[i] = pAlyRowStru->ptr_vect[i];//��ֵ��char*ָ��
			}
		}

		taos_fetch_rows_a(res, AnalysisRow_CallBack, pAlyRowStru);//��ȡ��һ����¼

	}
	else
	{
		pAlyRowStru->callbackRet = RES_EXE_FAILED;
		//taos_free_result(res);//�����첽�����ͷţ��ͷŶ�fields_info�Ļ�ȡ�ʹ����ˣ�ԭ�����
	}


}
