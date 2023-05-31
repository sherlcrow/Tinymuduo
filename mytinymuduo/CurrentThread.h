#pragma once
#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <sys/syscall.h>

//ʹ��linux�ں˺���::syscall(SYS_gettid)��ȡtid���Ĵ���ȫ�ֱ�������
//�߳�tid��������__thread���α�֤��ͬ�߳�֮�以������
namespace CurrentThread
{
	//__thread��GCC���õ��ֲ߳̾��洢��ʩ����洢Ч�ʿ��Ժ�ȫ�ֱ�����ȣ�
	//__thread������ÿһ���߳��ж���һ�ݶ���ʵ�������߳�ֵ�ǻ������ŵġ�
	extern __thread int t_cachedTid;  // ����tid���壬������ϵͳ����

	void cacheTid();

	inline int tid()
	{
		if (__builtin_expect(t_cachedTid == 0, 0)) //��֧Ԥ���Ż�
		{
			cacheTid();
		}
		return t_cachedTid;
	}
}
#endif