#pragma once
#ifndef CTAOSSYN
#define CTAOSSYN
#include <vector>
#include "CCommonHead.h"
using namespace std;

class CTaosSyn
{
public:
	CTaosSyn();
	~CTaosSyn();
	//连接数据库句柄，提前赋值连接数据库的成员变量：m_host、m_user、m_pass(默认m_db=NULL，m_port=6030，可不赋)
	int InitAccess(
		const CTaosSyn* TaosSyn_obj,				//[in]已赋值的对象
		TAOS*&			pTaos						//[out]数据库句柄.连接数据库并返回数据库句柄。注：带const都为传入参数
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

	








public://taos同步接口成员变量
	TAOS*				m_ptaos;				//taos数据库句柄

	//创建数据库连接变量
	string				m_host;					//TDengine集群中任一节点的FQDN
	string				m_user;					//用户名
	string				m_pass;					//密码
	string				m_db;					//数据库名字，如果用户没有提供，也可以正常连接，用户可以通过该连接创建新的数据库，如果用户提供了数据库名字，则说明该数据库用户已经创建好，缺省使用该数据库步骤
	int					m_port;					//taosd程序监听的端口，默认6030





};

#endif // !CTAOSSYN