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

	//检查连接数据库的变量值是否存在
	if (m_host == "" || m_user == "" || m_pass == "")
	{
		return VARIABLE_NO_VALUE_TAOS;
	}

	//m_ptaos = taos_connect("Linx.Linx-lzj", "root", "taosdata", NULL, 6030);//凝思句柄
	m_ptaos = taos_connect(TaosSyn_obj->m_host.c_str(), TaosSyn_obj->m_user.c_str(), TaosSyn_obj->m_pass.c_str(), TaosSyn_obj->m_db.c_str(), TaosSyn_obj->m_port);//数据库句柄

	pTaos = m_ptaos;

	if (m_ptaos)
		return SUCCESS_TAOS;
	else
		return HANDLE_FAILED_TAOS;
}

int CTaosSyn::ExecuteOneQueryDirectofRecordBytes(const char* szSqlSen, char*& szResult, int& Record_num, int& Onerecordlen, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;			//结果集字节流
	TAOS_ROW row;			//一行记录数据

	Record_num = 0;
	int blockrecord_num = 0;
	//获取sql数据返回结果集
	res = taos_query(m_ptaos, szSqlSen);
	while ((blockrecord_num = taos_fetch_block(res, &row)))
		Record_num += blockrecord_num;
	//Record_num = taos_fetch_block(res, &row);//这里不要别的，就是为了获取记录数
	res = taos_query(m_ptaos, szSqlSen);//这里为啥重新获取一次，因为上面block好像是取走就没了,这查时间很快就不计了
	//检查结果集是否正常返回
	if (res == NULL || taos_errno(res) != 0) {
		//printf("failed to select, reason:%s\n", taos_errstr(res));
		taos_free_result(res);
		return RES_QUERY_FAILED;
	}

	//结果集中获取域信息数组
	fields_info = taos_fetch_fields(res);
	//结果集中获取域个数
	fields_num = taos_num_fields(res);
	//计算一条记录长度
	for (int i = 0; i < fields_num; i++)
	{
		Onerecordlen += fields_info[i].bytes;
	}

	//检查szResult空间，重新开辟所有记录所需空间
	if (szResult != NULL)
	{
		free(szResult);
		szResult = (char*)malloc(Record_num * Onerecordlen);
	}
	else
		szResult = (char*)malloc(Record_num * Onerecordlen);
	
	//开辟临时接收一条记录的转换后的字节流
	char *buf_Onerecord = (char*)malloc(Onerecordlen);
	
	//所有记录数据按 对应数据类型以字节流 方式放入szResult中
	for (int i = 0; i < Record_num; i++)
	{
		//每新的一条记录都将临时记录字节流置空
		memcpy(buf_Onerecord, "", Onerecordlen);
		row = taos_fetch_row(res);

		//这里转字符串为了比较一下
		char str[100] = { 0 };
		taos_print_row(str, row, fields_info, fields_num);

		//这里是row是没有转换格式的字节流，需手动转换成对应类型的变量来存入szResult字节流
		int field_Offset = 0;// 域偏移量
		for (int j = 0; j < fields_num; j++)
		{
			if (row[j] == NULL)
				continue;

			////开辟临时接收一个域转换后的字节流，也可以通过这种方式先开辟一个域最大所需空间，都放到最后再填入buf，但这里不影响时间复杂度，无所谓了
			//char *buf_Onefield = (char*)malloc()

			switch (fields_info[j].type)// 根据字段类型进行处理
			{
			case TSDB_DATA_TYPE_BOOL:// 1
			case TSDB_DATA_TYPE_TINYINT:// 2
				{
					int8_t tmp = *(int8_t*)row[j];//变量临时接收对应格式数据
					memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int8_t));//将变量值填到单挑记录字节流中
					field_Offset += sizeof(int8_t);//偏移量增加
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
					//如果字段类型为BINARY或NCHAR
					/*
					这里解释一下BINARY或NCHAR
					BINARY：记录单字节字符串，建议只用于处理 ASCII 可见字符，中文等多字节字符需使用 NCHAR
					NCHAR：记录包含多字节字符在内的字符串，如中文字符。每个 NCHAR 字符占用 4 字节的存储空间。字符串两端使用单引号引用，字符串内的单引号需用转义字符 \'。
						   NCHAR 使用时须指定字符串大小，类型为 NCHAR(10) 的列表示此列的字符串最多存储 10 个 NCHAR 字符。如果用户字符串长度超出声明长度，将会报错。
					*/
					int32_t charLen = ((uint16_t*)((char*)row[j] - sizeof(uint16_t)))[0];//获取字段值长度，uint16_t是VarDataLenT，这里taos官方这么做
					if (fields_info[j].type == TSDB_DATA_TYPE_BINARY)
					{
						if (charLen > fields_info[j].bytes || charLen < 0)//确保长度在有效范围内
							return BINARY_OR_NCHAR_LEN_OVERFLOW;
					}
					else
						if (charLen > fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//确保长度在有效范围内,这里乘int32_t因为每个字符占4字节，因为为了可存中文这种单个2字节的字符
							return BINARY_OR_NCHAR_LEN_OVERFLOW;

					//因为这里char存的是真实字符长度，为了方便，这里转成定义时域长度
					char* vchar = (char*)malloc(fields_info[j].bytes);//开辟临时域定义长度接收
					memset(vchar, 0, fields_info[j].bytes);//全部置0
					memcpy(vchar, (char*)row[j], charLen);//填入前面真实串部分

					memcpy(buf_Onerecord + field_Offset, vchar, fields_info[j].bytes);//将临时串填入单记录buf
					field_Offset += fields_info[j].bytes;

					free(vchar);
				}
				break;
			default:
				break;
			}

		}
		//将一条已转换类型的记录字节流填到总buf字节流中
		memcpy(szResult + Onerecordlen * i, (char*)buf_Onerecord, Onerecordlen);
	}


	////解析测试存放字节流是否正确
	//int Offset = 0;
	//int64_t ts = 0;
	//double value = 0;
	//char name[60] = { 0 };
	//int type = 0;
	//for (int i = 0; i < Record_num; i++)//记录数，单挑记录长度，域偏移量
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
	TAOS_RES* res;			//结果集字节流
	TAOS_ROW row;			//一行记录数据
	int Onerecordlen = 0;	//一条记录长度

	Record_num = 0;
	//获取sql数据返回结果集
	res = taos_query(m_ptaos, szSqlSen);

	//建临时存放每条记录数据的指针
	vector<char*>	ptr_vect;

	//结果集中获取域信息数组
	fields_info = taos_fetch_fields(res);
	//结果集中获取域个数
	fields_num = taos_num_fields(res);
	//计算一条记录长度
	for (int i = 0; i < fields_num; i++)
	{
		Onerecordlen += fields_info[i].bytes;
	}

	while ((row = taos_fetch_row(res)))
	{
		//开辟临时接收一条记录的转换后的字节流
		char* buf_Onerecord = (char*)malloc(Onerecordlen);
		ptr_vect.push_back(buf_Onerecord);//存入vector

		//这里是row是没有转换格式的字节流，需手动转换成对应类型的变量来存入szResult字节流
		int field_Offset = 0;// 域偏移量
		for (int j = 0; j < fields_num; j++)
		{
			if (row[j] == NULL)
				continue;

			////开辟临时接收一个域转换后的字节流
			//char *buf_Onefield = (char*)malloc()

			switch (fields_info[j].type)// 根据字段类型进行处理
			{
			case TSDB_DATA_TYPE_BOOL:// 1
			case TSDB_DATA_TYPE_TINYINT:// 2
			{
				int8_t tmp = *(int8_t*)row[j];//变量临时接收对应格式数据
				memcpy(buf_Onerecord + field_Offset, (char*)&tmp, sizeof(int8_t));//将变量值填到单挑记录字节流中
				field_Offset += sizeof(int8_t);//偏移量增加
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
				//如果字段类型为BINARY或NCHAR
				/*
				这里解释一下BINARY或NCHAR
				BINARY：记录单字节字符串，建议只用于处理 ASCII 可见字符，中文等多字节字符需使用 NCHAR
				NCHAR：记录包含多字节字符在内的字符串，如中文字符。每个 NCHAR 字符占用 4 字节的存储空间。字符串两端使用单引号引用，字符串内的单引号需用转义字符 \'。
					   NCHAR 使用时须指定字符串大小，类型为 NCHAR(10) 的列表示此列的字符串最多存储 10 个 NCHAR 字符。如果用户字符串长度超出声明长度，将会报错。
				*/
				int32_t charLen = ((uint16_t*)((char*)row[j] - sizeof(uint16_t)))[0];//获取字段值长度，uint16_t是VarDataLenT，这里taos官方这么做
				if (fields_info[j].type == TSDB_DATA_TYPE_BINARY)
				{
					if (charLen > fields_info[j].bytes || charLen < 0)//确保长度在有效范围内
						return BINARY_OR_NCHAR_LEN_OVERFLOW;
				}
				else
					if (charLen > fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//确保长度在有效范围内,这里乘int32_t因为每个字符占4字节，因为为了可存中文这种单个2字节的字符
						return BINARY_OR_NCHAR_LEN_OVERFLOW;

				//因为这里char存的是真实字符长度，为了方便，这里转成定义时域长度
				char* vchar = (char*)malloc(fields_info[j].bytes);//开辟临时域定义长度接收
				memset(vchar, 0, fields_info[j].bytes);//全部置0
				memcpy(vchar, (char*)row[j], charLen);//填入前面真实串部分

				memcpy(buf_Onerecord + field_Offset, vchar, fields_info[j].bytes);//将临时串填入单记录buf
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
		//创建二维地址指针存放空间
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
