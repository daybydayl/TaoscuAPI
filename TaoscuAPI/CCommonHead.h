#pragma once
#ifndef CCOMMONHEAD_H
#define CCOMMONHEAD_H
#pragma execution_character_set("utf-8")//�������ı���

#include "C:\TDengine\include\taos.h"
#include <QtCore>

//����������ϵͳͷ�ļ�
#include "D:/rr6000/src/vc/include/dnet_all.h"
//������ʷ����ϵͳͷ�ļ�
#include "D:/rr6000/src/vc/include/hds_all.h"
//����ʵʱ����ϵͳͷ�ļ�
#include "D:/rr6000/src/vc/include/rdb_all.h"



#define SUCCESS_TAOS					0	//ִ�гɹ�
#define VARIABLE_NO_VALUE_TAOS			1	//���ݿ����ӱ���δ��ֵ
#define HANDLE_FAILED_TAOS				2	//�����ȡʧ��
#define RES_QUERY_FAILED				3	//sql��ѯ��res�����ʧ��
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//�ֶ�nchar��binary�������
#define CURRENCY_ERROR					5	//ͨ�ô����磺�ڴ��ͷ���������






#endif // !CCOMMONHEAD_H