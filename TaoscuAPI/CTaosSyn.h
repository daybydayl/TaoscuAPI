#pragma once
#ifndef CTAOSSYN
#define CTAOSSYN
#include <vector>
#include <string>
#include <iostream>
#include <fstream>//读文件所需
//#include "CCommonHead.h"
using namespace std;

#include "taos.h"
//包含网络子系统头文件
#include "dnet_all.h"
//包含历史库子系统头文件
#include "hds_all.h"
//包含实时库子系统头文件
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
	//连接数据库句柄，提前赋值连接数据库的成员变量：m_host、m_user、m_pass(默认m_db=NULL，m_port=6030，可不赋)
	int InitAccess(
		const CTaosSyn* TaosSyn_obj,				//[in]已赋值的对象
		TAOS*			pTaos = 0					//[out]数据库句柄.连接数据库并返回数据库句柄。注：带const都为传入参数
	);

	//sql直接操作数据库，可以是 DQL、DML 或 DDL 语句。
	int ExecuteSqlCtlDB(
		const char* sqlstr,							//[in]执行的sql语句
		TAOS_RES*	res = 0							//[out]返回原生的结果集，若不是查询语句或不返回的sql，可缺省不填
	);

	//获取sql全部记录，以一维字节流方式放入nResult,返回值0成功。
	//优点：取数据方便。缺点：时间比总单条也慢些，因内部通过块遍历获取了行数(实测210万条4列数据 11秒)
	int ExecuteOneQueryDirectofRecordBytes(
		const char*		szSqlSen,					//[in]Sql语句
		char*&			nResult,					//[out]返回全部数据以字节流方式
		int&			Record_num,					//[out]返回记录数
		int&			Onerecordlen,				//[out]一条记录的长度
		TAOS_FIELD*&	fields_info,				//[out]域信息数组
		int&			fields_num,					//[out]域数
		const int		nExecDBNo = 0				//[in]需查询的数据库编号，这里留做扩展，缺省值为0
	);


	//获取sql全部数据，以二维空间字节流放入Result，返回值0成功。/*优先这个接口，速度更佳，实测210万条4列数据 4秒*/
	//优点：速度快，取数据方便。缺点：内存开销稍大，释放内存需先对每个二级指针进行释放，再释放一级指针。可以使用FreePtP接口释放
	int ExecuteOneQueryDirectofRecordList(
		const char*		szSqlSen,					//[in]Sql语句
		char**&			Result,						//[out]返回记录数个记录数据以字节流方式，[记录指针序号][记录数据空间指针]
		int&			Record_num,					//[out]返回记录数
		TAOS_FIELD*&	fields_info,				//[out]域信息数组
		int&			fields_num,					//[out]域数
		const int		nExecDBNo = 0				//[in]需查询的数据库编号，这里留做扩展，缺省值为0
	);
	//释放**Result二级指针
	int FreePtP(
		char**&			Result,						//[in]需释放的二级指针
		int&			Record_num					//[in]存放一级指针数
	);



	/*======================写入=======================*/
	
	int ExecuteInsertNRecordbyFile(
		const char*		tbname,						//[in]需要插入数据的库对应表名，如rr6000.meas1(普通表)、rr6000.meas_meter1(超级表)
		const char*		FileLoc,					//[in]数据文件路径
		int				TAGNum = 0,					//[in]向超级表对应的tag数量，若目标表为超级表，才需此两个参数，否则缺省即可
		char**			TAGsName = 0				//[in]二级指针的域名，[域名地址索引][存放域名空间地址]，向超级表tag对应表插数据
	);/*=================未完成，待定，看是否需要===========================*/

	//通过rr6000系统获取的记录buf字节流来将数据插入taos库中的子表
	int ExecuteInsertNRecordbyBuf(
		const char*		buf,						//传入的buf
		const int		fields_num,					//buf里单条记录域数
		const int		record_num,					//buf里记录数
		const TABLE_HEAD_FIELDS_INFO* table_fields_info,//域信息
		const char*		dbname,						//写入对应数据库
		const char*		tbname,						//写入对应子表，子表存在下面部分可缺省
		const char*		stbname = 0,				//若子表不存在，这里填创建的子表所属的超级表
		const char**	TAGsValue = 0,				//stbname非空时，建子表所填的tag值，字符串请加上单引号，并且TAG值全填，后面TAGsName可缺省，TAGsNum不可缺省
		const int		TAGsNum = 0,				//TAG数量
		const char**	TAGsName = 0				//stbname非空时，建子表所填的tag字段名
	);


public://taos同步接口成员变量
	TAOS*				m_ptaos;				//taos数据库句柄

	//创建数据库连接变量
	string				m_host;					//TDengine集群中任一节点的FQDN
	string				m_user;					//用户名
	string				m_pass;					//密码
	string				m_db;					//数据库名字，如果用户没有提供，也可以正常连接，用户可以通过该连接创建新的数据库，如果用户提供了数据库名字，则说明该数据库用户已经创建好，缺省使用该数据库步骤
	int					m_port;					//taosd程序监听的端口，默认6030





};


#define SUCCESS_TAOS					0	//执行成功
#define VARIABLE_NO_VALUE_TAOS			1	//数据库连接变量未赋值
#define HANDLE_FAILED_TAOS				2	//句柄获取失败
#define RES_EXE_FAILED					3	//执行失败
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//字段nchar或binary长度溢出
#define CURRENCY_ERROR					5	//通用错误，如：内存释放数量错误
#define OPEN_FIEL_FAILED				6	//文件打开失败
#define ACCESS_DB_FAILED				7	//数据库访问失败
#define DATA_TYPE_ERROR					8	//数据类型错误


#endif // !CTAOSSYN