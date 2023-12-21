#pragma once
#ifndef CTAOSASYN_H
#define CTAOSASYN_H
#ifndef _WINDOWS
#include <unistd.h>
#endif // !_WINDOWS
#include <string>
#include "TaosCHead.h"
using namespace std;


/*=========================部分方法同步异步有交集冗余，后面看是否需要合并，有部分内存泄漏====================================*/


class CTaosASyn
{
public:
	CTaosASyn();
	~CTaosASyn();

	//连接数据库句柄，提前赋值连接数据库的成员变量：m_host、m_user、m_pass(默认m_db=NULL，m_port=6030，可不赋)
	int InitAccess(
		const CTaosASyn* TaosSyn_obj				//[in]已赋值的对象。注：带const都为传入参数
	);

public:
	//异步查询结果，需用GetRecordList接口取result，执行完不影响主进程继续，要数据时在即可(类似提前下装)
	int WaitOneQueryDirectofRecordList(
		const char*		szSqlSen,					//[in]Sql语句
		int&			Record_num,					//[out]返回记录数
		TAOS_FIELD*&	fields_info,				//[out]域信息数组
		int&			fields_num,					//[out]域数
		const int		nExecDBNo = 0				//[in]需查询的数据库编号，这里留做扩展，缺省值为0
	);
	//等待异步查询获取结果完成后返回结果buf
	int GetRecordList(char**& nResult);
	//释放**Result二级指针
	int FreePtP(
		char**& Result,								//[in]需释放的二级指针
		int& Record_num								//[in]存放一级指针数
	);

//内部接口 回调函数，需要为静态或全局函数，因为回调函数需要个参数匹配，类的成员函数默认会多一个this指针
private:
	//用于WaitOneQueryDirectofRecordList
	static void GetBlock_CallBack(void* param, TAOS_RES* res, int code);//调用回调函数传参的万能指针，res结果集，code结果集返回状态
	static void AnalysisRow_CallBack(void* param, TAOS_RES* res, int numOfRows);//调用回调函数传参的万能指针，res结果集，code结果集返回状态


//taos同步接口成员变量
public:
	TAOS* m_ptaos;								//taos数据库句柄
	//创建数据库连接变量
	string				m_host;					//TDengine集群中任一节点的FQDN
	string				m_user;					//用户名
	string				m_pass;					//密码
	string				m_db;					//数据库名字，如果用户没有提供，也可以正常连接，用户可以通过该连接创建新的数据库，如果用户提供了数据库名字，则说明该数据库用户已经创建好，缺省使用该数据库步骤
	int					m_port;					//taosd程序监听的端口，默认6030

//内部成员变量
private:
	char**				m_szResult;				//异步查询返回的buf二维指针
	int					m_Retresult;			//buf状态返回值
	bool				m_FinishedResult;		//buf是否装满标志

};



#endif // !CTAOSASYN_H



