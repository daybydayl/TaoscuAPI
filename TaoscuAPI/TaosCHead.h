#pragma once
#ifndef TAOSCHEAD_H
#define TAOSCHEAD_H
#pragma execution_character_set("utf-8")//�������ı���

#include "taos.h"
//#include <QtCore>

//����������ϵͳͷ�ļ�
#include "dnet_all.h"
//������ʷ����ϵͳͷ�ļ�
#include "hds_all.h"
//����ʵʱ����ϵͳͷ�ļ�
#include "rdb_all.h"

#include <vector>



#define SUCCESS_TAOS					0	//ִ�гɹ�
#define VARIABLE_NO_VALUE_TAOS			1	//���ݿ����ӱ���δ��ֵ
#define HANDLE_FAILED_TAOS				2	//�����ȡʧ��
#define RES_EXE_FAILED					3	//sql��ѯ��res�����ʧ��
#define BINARY_OR_NCHAR_LEN_OVERFLOW	4	//�ֶ�nchar��binary�������
#define CURRENCY_ERROR					5	//ͨ�ô����磺�ڴ��ͷ���������
#define OPEN_FIEL_FAILED				6	//�ļ���ʧ��
#define ACCESS_DB_FAILED				7	//���ݿ����ʧ��
#define DATA_TYPE_ERROR					8	//�������ʹ���


#endif // !TAOSCHEAD_H