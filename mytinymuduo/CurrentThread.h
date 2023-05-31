#pragma once
#ifndef CURRENT_THREAD_H
#define CURRENT_THREAD_H

#include <unistd.h>
#include <sys/syscall.h>

//使用linux内核函数::syscall(SYS_gettid)获取tid消耗大，用全局变量保存
//线程tid，并且用__thread修饰保证不同线程之间互不干扰
namespace CurrentThread
{
	//__thread是GCC内置的线程局部存储设施，其存储效率可以和全局变量相比；
	//__thread变量在每一个线程中都有一份独立实例，各线程值是互不干扰的。
	extern __thread int t_cachedTid;  // 保存tid缓冲，避免多次系统调用

	void cacheTid();

	inline int tid()
	{
		if (__builtin_expect(t_cachedTid == 0, 0)) //分支预测优化
		{
			cacheTid();
		}
		return t_cachedTid;
	}
}
#endif