#include<iostream>
#include "public.h"
#include "Connection.h"

//��ʼ�����ݿ�����,ʵ������malloc��һ��MYSQL�ṹ���С��ָ��
Connection::Connection()
{
	_conn = mysql_init(nullptr);
}
//�ͷ����ݿ�������Դ
Connection::~Connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}
//�������ݿ�
bool Connection::connect(string ip, unsigned short port, string user, string password, string dbname)
{
	MYSQL* p = mysql_real_connect(_conn,ip.c_str(), user.c_str(), password.c_str(), dbname.c_str(), port, nullptr, 0);
	return p != nullptr;
}
//���²���
bool Connection::update(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("����ʧ�ܣ�"+sql);
		return false;
	}
	return true;
}
//��ѯ����
MYSQL_RES* Connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("��ѯʧ�ܣ�" + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}