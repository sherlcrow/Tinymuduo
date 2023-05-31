#pragma once
#ifndef FIXED_BUFFER_H
#define FIXED_BUFFER_H

#include "noncopyable.h"
#include <assert.h>
#include <string.h> // memcpy
#include <strings.h>
#include <string>
const int kSmallBuffer = 4000;
const int kLargeBuffer = 4000 * 1000;

/*
			length			avail
	|----------------|----------------|
  data_				cur               end
*/
template<int SIZE>
class FixedBuffer : noncopyable
{
public:
	FixedBuffer() : cur_(data_)
	{
	}

	void append(const char* buf, size_t len)
	{
		if (static_cast<size_t>(avail()) > len)
		{
			memcpy(cur_, buf, len);
			cur_ += len;
		}
	}

	const char* data() const { return data_; }

	// 此处修改了length的定义，最初定义为 ? ? ? end() - data_ ? ? ?
	int length() const { return static_cast<int>(cur_ - data_); } 
	char* current() { return cur_; }
	int avail() const { return static_cast<int>(end() - cur_); }
	void add(size_t len) { cur_ += len; }

	void reset() { cur_ = data_; }
	void bzero() { ::bzero(data_, sizeof(data_)); }

	std::string toString() const { return std::string(data_, length()); }
private:
	const char* end() const { return data_ + sizeof(data_); };//末尾位置
	char data_[SIZE]; //数据段
	char* cur_; //当前位置
};
#endif