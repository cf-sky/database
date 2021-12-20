#include <iostream>
#include "CommonConnectionPool.h"
#include "public.h"

//线程安全的懒汉式单例函数接口
ConnectionPool* ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool;//对于static对象的初始化由编译器自动进行lock和Unlock
	return &pool;
}

//从配置文件中加载配置项
bool ConnectionPool::loadConfigFile()
{
	FILE* pf = fopen("mysql.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}
	while(!feof(pf))//如果文件没有读到末尾
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);//一次读一行数据
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1)//无效的配置项
		{
			continue;
		}
		//port=3306\n
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);
		
		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeout")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

//连接池的构造
ConnectionPool::ConnectionPool()
{
	//加载配置项了
	if (!loadConfigFile())
	{
		return;
	}
	//创建初始数量的连接
	for (int i = 0; i < _initSize; ++i)
	{
		Connection* p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();//刷新一下开始空闲的起始时间
		_connectionQue.push(p);
		_connectionCnt++;
	}
	//启动一个新的线程，作为连接的生产者，将这个成员方法绑定一个当前对象，
	//要不然这个成员方法不能直接作为一个线程函数，线程函数都是C接口
	//thread底层调用的是pthread_create
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();
	
	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对应连接的回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

//生产者线程
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);//引用传递_queueMutex
		while (!_connectionQue.empty())
		{
			cv.wait(lock);//队列不空，此时生产线程进入等待状态
		}
		//连接数量没有到达上限，继续创建新的连接
		if (_connectionCnt < _maxSize)
		{
			Connection* p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			_connectionCnt++;
		}
		//通知消费者线程可以消费连接
		cv.notify_all();
	}
}

//消费者线程，给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);

	while(_connectionQue.empty())
	{
		//如果没被唤醒，是超时醒来地
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(_connectionTimeout)))
		{
			if (_connectionQue.empty())
			{
				LOG("获取空闲连接超时了！连接失败");
				return nullptr;
			}
		}
	}
	/*
	shared_ptr智能指针析构时，会把connection资源直接delete掉,
	相当于调用connection的析构函数，connection就被析构掉了，
	这里需要自定义shared_ptr的释放资源方式，把connection直接归还到queue中
	*/
	shared_ptr<Connection> sp(_connectionQue.front(),
		[&](Connection* pcon) {
			//这里是在服务器应用线程中用到的，所以一点要考虑到队列的线程安全操作
			unique_lock<mutex> lock(_queueMutex);
			pcon->refreshAliveTime();
			_connectionQue.push(pcon);
		}
		);
	_connectionQue.pop();
	cv.notify_all();//消费完连接以后，通知生产者线程检查，若队列为空，则赶紧生产
	
	return sp;
}

//扫描超过maxIdleTime时间的空闲连接，进行对应连接的回收
void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		//通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));
		//扫描整个队列，释放多余连接
		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection* p = _connectionQue.front();
			if (p->getAliveTime() >= (_maxIdleTime*1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;//调用~connection()释放连接
			}
			else
			{
				break;//队头的连接没有超过maxIdleTime,其他的肯定没有
			}
		}
	}
}