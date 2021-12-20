#pragma once
#include<string>
#include<queue>
#include "Connection.h"
#include<mutex>
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
using namespace std;

//ʵ�����ӳع��ܿ�ģ��
class ConnectionPool
{
public:
	//��ȡ���ӳض���ʵ��
	static ConnectionPool* getConnectionPool();

	//���ⲿ�ṩ�ӿڣ������ӳ��л�ȡһ�����õĿ�������
	shared_ptr<Connection> getConnection();
private:

	//����ģʽ�����캯��˽�л�
	ConnectionPool();

	//�������ļ��м���������
	bool loadConfigFile();

	//�����ڶ������߳��У�ר�Ÿ�������������
	void produceConnectionTask();

	//ɨ�賬��maxIdleTimeʱ��Ŀ������ӣ����ж�Ӧ���ӵĻ���
	void scannerConnectionTask();

	string _ip;//mysql��ip��ַ
	unsigned short _port;//mysql�Ķ˿ں�
	string _username;//mysql�û���¼��
	string _password;//mysql�û�����
	string _dbname;//���ӵ����ݿ���
	int _initSize;//���ӳصĳ�ʼ������
	int _maxSize;//���ӳص����������
	int _maxIdleTime;//���ӳ�������ʱ��
	int _connectionTimeout;//���ӳػ�ȡ���ӵĳ�ʱʱ��

	queue<Connection*> _connectionQue;//�洢mysql���ӵĶ���
	mutex _queueMutex;//ά�����Ӷ��е��̰߳�ȫ������
	//��ͨ��++���������̰߳�ȫ�ģ��������ó�atomicԭ�����ͣ��ǻ���ICS���̰߳�ȫ�Ĳ���
	atomic_int _connectionCnt;//��¼������������connection���ӵ�������
	condition_variable cv;//���������������������������̺߳����������̵߳�ͨ��
};