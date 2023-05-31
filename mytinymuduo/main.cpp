#define _CRT_SECURE_NO_WARNINGS 1
#include "ThreadPool.h"
#include "Logger.h"
#include <iostream>

static void myOutput(const char* data, int len)
{
	fwrite(data, len, sizeof(char), stdout);
}
int main()
{
	Logger::setOutput(myOutput);
}