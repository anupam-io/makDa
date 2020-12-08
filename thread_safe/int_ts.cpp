#include <iostream>
#include <mutex>
using namespace std;

class int_ts
{
	int num = 0;
	mutex m_lock;

public:
	int_ts()
	{
	}

	int value()
	{
		lock_guard<mutex> guard_obj(m_lock);
		int ret = num;
		return ret;
	}
	int add(int x)
	{
		lock_guard<mutex> guard_obj(m_lock);
		num += x;
		int ret = num;
		return ret;
	}
};