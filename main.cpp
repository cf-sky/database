#include<iostream>
using namespace std;
#include "Connection.h"
#include "CommonConnectionPool.h"

int main()
{
	//д������̵���
#if 0
	/*
	Connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')","zhang san", 20, "male");
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	conn.update(sql);
	return 0;
	*/
	
	//�������Ҫ��loadConfigFile���ó�public
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	cp->loadConfigFile();
}
#endif

//���̲߳���
#if 1
	/*
	//δʹ�����ӳ�
	clock_t begin = clock();
	for (int i = 0; i < 100; ++i)
	{
		Connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql);
	}
	clock_t end = clock();
	*/
	//ʹ�����ӳ�
	clock_t begin = clock();
	ConnectionPool* cp = ConnectionPool::getConnectionPool();
	for (int i = 0; i < 100; i++)
	{
		shared_ptr<Connection> sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
		sp->update(sql);
	}
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
}
#endif 

//���̲߳���
#if 0
	
	//ʹ�����ӳ�
	clock_t begin = clock();
	thread t1([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 25; i++)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t2([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 25; i++)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t3([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 25; i++)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	thread t4([]() {
		ConnectionPool* cp = ConnectionPool::getConnectionPool();
		for (int i = 0; i < 25; i++)
		{
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			shared_ptr<Connection> sp = cp->getConnection();
			sp->update(sql);
		}
		});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	
	//δʹ�����ӳ�
	/*
	Connection conn;
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	clock_t begin = clock();
	thread t1([]() {
		for (int i = 0; i < 25; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t2([]() {
		for (int i = 0; i < 25; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t3([]() {
		for (int i = 0; i < 25; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t4([]() {
		for (int i = 0; i < 25; ++i)
		{
			Connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into stu(name,age,sex) values('%s',%d,'%s')", "zhang san", 20, "male");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	clock_t end = clock();
	cout << (end - begin) << "ms" << endl;
	*/
}
#endif