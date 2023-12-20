#pragma once
#ifndef TAOSCHEAD_H
#define TAOSCHEAD_H
#pragma execution_character_set("utf-8")//设置中文编码

#include "taos.h"
//#include <QtCore>

//包含网络子系统头文件
#include "dnet_all.h"
//包含历史库子系统头文件
#include "hds_all.h"
//包含实时库子系统头文件
#include "rdb_all.h"

#include <vector>



#define SUCCESS_TAOS					0	//执行成功
#define VARIABLE_NO_VALUE_TAOS			1	//数据库连接变量未赋值
#define HANDLE_FAILED_TAOS				2	//句柄获取失败
#define RES_EXE_FAILED					3	//sql查询的res结果集失败
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//字段nchar或binary长度溢出
#define CURRENCY_ERROR					5	//通用错误，如：内存释放数量错误
#define OPEN_FIEL_FAILED				6	//文件打开失败
#define ACCESS_DB_FAILED				7	//数据库访问失败
#define DATA_TYPE_ERROR					8	//数据类型错误


#endif // !TAOSCHEAD_H