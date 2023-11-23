#pragma once
#ifndef CCOMMONHEAD_H
#define CCOMMONHEAD_H
#pragma execution_character_set("utf-8")//设置中文编码

#include "C:\TDengine\include\taos.h"
#include <QtCore>

//包含网络子系统头文件
#include "D:/rr6000/src/vc/include/dnet_all.h"
//包含历史库子系统头文件
#include "D:/rr6000/src/vc/include/hds_all.h"
//包含实时库子系统头文件
#include "D:/rr6000/src/vc/include/rdb_all.h"



#define SUCCESS_TAOS					0	//执行成功
#define VARIABLE_NO_VALUE_TAOS			1	//数据库连接变量未赋值
#define HANDLE_FAILED_TAOS				2	//句柄获取失败
#define RES_QUERY_FAILED				3	//sql查询的res结果集失败
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//字段nchar或binary长度溢出
#define CURRENCY_ERROR					5	//通用错误，如：内存释放数量错误






#endif // !CCOMMONHEAD_H