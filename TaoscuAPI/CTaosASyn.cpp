#include "CTaosASyn.h"

struct AnalysisRowStruct {
	char**&			szResult;
	int&			Record_num;
	int&			Onerecordlen;
	TAOS_FIELD*&	fields_info;
	int&			fields_num;
	int&			callbackRet;	//回调函数的返回值
	vector<char*>	ptr_vect;		//建临时存放每条记录数据的指针
	// 构造函数，用于初始化引用成员
	AnalysisRowStruct(char**& rSzResult, int& rRecordNum, int& rOnerecordlen,TAOS_FIELD*& rFieldsInfo, int& rFieldsNum, int& rCallbackRet, vector<char*> rptr_vect)
		: szResult(rSzResult), Record_num(rRecordNum), Onerecordlen(rOnerecordlen),
		fields_info(rFieldsInfo), fields_num(rFieldsNum), callbackRet(rCallbackRet), ptr_vect(rptr_vect){
		// 其他构造逻辑
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
	//释放taos句柄
	taos_close(m_ptaos);
	taos_cleanup();
}

int CTaosASyn::InitAccess(const CTaosASyn* TaosSyn_obj)
{
	//检查连接数据库的变量值是否存在
	if (m_host == "" || m_user == "" || m_pass == "")
	{
		return VARIABLE_NO_VALUE_TAOS;
	}
	//m_ptaos = taos_connect("Linx.Linx-lzj", "root", "taosdata", NULL, 6030);//凝思句柄
	m_ptaos = taos_connect(TaosSyn_obj->m_host.c_str(), TaosSyn_obj->m_user.c_str(), TaosSyn_obj->m_pass.c_str(), TaosSyn_obj->m_db.c_str(), TaosSyn_obj->m_port);//数据库句柄
	if (m_ptaos)
		return SUCCESS_TAOS;
	else
		return HANDLE_FAILED_TAOS;
}

int CTaosASyn::ExecuteOneQueryDirectofRecordList(const char* szSqlSen, char**& szResult, int& Record_num, int& Onerecordlen, TAOS_FIELD*& fields_info, int& fields_num, const int nExecDBNo)
{
	TAOS_RES* res;//结果集字节流
	int Ret = 0;//这里异步其实没用，可以优化

	//返回变量的初始化
	Onerecordlen = 0;
	Record_num = 0;
	fields_info = 0;
	fields_num = 0;

	vector<char*>	ptr_vect;//累加异步查询记录的vec
	AnalysisRowStruct* pAlyRowStru = new AnalysisRowStruct(szResult, Record_num, Onerecordlen, fields_info, fields_num, Ret, ptr_vect);

	//直接异步获取数据集
	taos_query_a(m_ptaos, szSqlSen, GetBlock_CallBack, (void *)pAlyRowStru);//这种还有问题，明天再看下

	//delete pAlyRowStru;
	return Ret;
}

void CTaosASyn::GetBlock_CallBack(void* param, TAOS_RES* res, int code)
{
	AnalysisRowStruct* pAlyRowStru = static_cast<AnalysisRowStruct*>(param);

	if (code == 0 && res) {

		//结果集中获取域信息数组
		pAlyRowStru->fields_info = taos_fetch_fields(res);
		//结果集中获取域个数
		pAlyRowStru->fields_num = taos_num_fields(res);

		//计算一条记录长度
		for (int i = 0; i < pAlyRowStru->fields_num; i++)
		{
			pAlyRowStru->Onerecordlen += pAlyRowStru->fields_info[i].bytes;
		}

		// 异步API来获取一批记录，默认块大小4096
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
	TAOS_ROW row;			//一行记录数据
	if (numOfRows > 0)
	{
		for (int i = 0; i < numOfRows; i++)
		{
			row = taos_fetch_row(res);
			/*分析记录*/
			//开辟临时接收一条记录的转换后的字节流
			char* buf_Onerecord = (char*)malloc(pAlyRowStru->Onerecordlen);
			pAlyRowStru->ptr_vect.push_back(buf_Onerecord);//存入vector

			//这里是row是没有转换格式的字节流，需手动转换成对应类型的变量来存入szResult字节流
			int field_Offset = 0;// 域偏移量
			for (int j = 0; j < pAlyRowStru->fields_num; j++)
			{
				if (row[j] == NULL)
					continue;

				switch (pAlyRowStru->fields_info[j].type)// 根据字段类型进行处理
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
					if (pAlyRowStru->fields_info[j].type == TSDB_DATA_TYPE_BINARY)
					{
						if (charLen > pAlyRowStru->fields_info[j].bytes || charLen < 0)//确保长度在有效范围内
							pAlyRowStru->callbackRet = BINARY_OR_NCHAR_LEN_OVERFLOW;
					}
					else
						if (charLen > pAlyRowStru->fields_info[j].bytes * sizeof(int32_t) || charLen < 0)//确保长度在有效范围内,这里乘int32_t因为每个字符占4字节，因为为了可存中文这种单个2字节的字符
							pAlyRowStru->callbackRet = BINARY_OR_NCHAR_LEN_OVERFLOW;

					//因为这里char存的是真实字符长度，为了方便，这里转成定义时域长度
					char* vchar = (char*)malloc(pAlyRowStru->fields_info[j].bytes);//开辟临时域定义长度接收
					memset(vchar, 0, pAlyRowStru->fields_info[j].bytes);//全部置0
					memcpy(vchar, (char*)row[j], charLen);//填入前面真实串部分

					memcpy(buf_Onerecord + field_Offset, vchar, pAlyRowStru->fields_info[j].bytes);//将临时串填入单记录buf
					field_Offset += pAlyRowStru->fields_info[j].bytes;

					free(vchar);
				}
				break;
				default:
					break;
				}

			}


		}
		pAlyRowStru->Record_num += numOfRows;//累加记录数

		if (pAlyRowStru->Record_num%numOfRows!=0 || pAlyRowStru->Record_num==numOfRows)//若到最后块了就开辟总buf大小
		{
			if (pAlyRowStru->szResult != NULL)
			{
				free(pAlyRowStru->szResult);
				//创建二维地址指针存放空间
				pAlyRowStru->szResult = (char**)malloc(pAlyRowStru->Record_num * sizeof(char*));
			}
			else
				pAlyRowStru->szResult = (char**)malloc(pAlyRowStru->Record_num * sizeof(char*));

			for (int i = 0; i < pAlyRowStru->Record_num; i++)
			{
				pAlyRowStru->szResult[i] = pAlyRowStru->ptr_vect[i];//赋值给char*指针
			}
		}

		taos_fetch_rows_a(res, AnalysisRow_CallBack, pAlyRowStru);//获取下一批记录

	}
	else
	{
		pAlyRowStru->callbackRet = RES_EXE_FAILED;
		//taos_free_result(res);//这里异步不能释放，释放对fields_info的获取就错误了，原因不理解
	}


}
