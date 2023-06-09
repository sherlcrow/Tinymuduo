#include "LogStream.h"
#include <algorithm>

//用一个对称的 digits 数组搞定了负数转换的边界条件(二进制补码的正负数表示范围不对称):
static const char digits[] = { '9', '8', '7', '6', '5', '4', '3', '2', '1', '0',
                               '1', '2', '3', '4', '5', '6', '7', '8', '9' };

//把整数分解为字符串并追加到buffer后面
//具体而言，先将整数从后往前取出末尾数值，最后加上符号获赠
// int len = snprintf(buffer_.current(), kMaxNumericSize, "%d", v);
// buffer_.add(len);
// 相比于直接使用snprintf这样可以涵盖更多数据类型，例如Long longlong等
template <typename T>
void LogStream::formatInteger(T num)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        char* start = buffer_.current();
        char* cur = start;
        const char* zero = digits + 9;
        bool negative = (num < 0); // 是否为负数

        // 末尾取值加入，最后反转
        do {
            int remainder = static_cast<int>(num % 10);
            *(cur++) = zero[remainder];
            num = num / 10;
        } while (num != 0);

        if (negative) {
            *(cur++) = '-';
        }
        *cur = '\0';

        std::reverse(start, cur);
        
        buffer_.add(static_cast<int>(cur - start)); // cur_向后移动
    }
}

LogStream& LogStream::operator<<(short v)
{
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v)
{
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v)
{
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(float v)
{
    *this << static_cast<double>(v);
    return *this;
}

LogStream& LogStream::operator<<(double v)
{
    if (buffer_.avail() >= kMaxNumericSize)
    {
        int len = snprintf(buffer_.current(), kMaxNumericSize, "%.12g", v);
        buffer_.add(len);
        return *this;
    }
}

LogStream& LogStream::operator<<(char c)
{
    buffer_.append(&c, 1);
    return *this;
}

LogStream& LogStream::operator<<(const void* data)
{
    *this << static_cast<const char*>(data);
    return *this;
}

LogStream& LogStream::operator<<(const char* str)
{
    if (str)
    {
        buffer_.append(str, strlen(str));
    }
    else
    {
        buffer_.append("(null)", 6);
    }
    return *this;
}

LogStream& LogStream::operator<<(const unsigned char* str)
{
    return operator<<(reinterpret_cast<const char*>(str));
}

LogStream& LogStream::operator<<(const std::string& str)
{
    buffer_.append(str.c_str(), str.size());
    return *this;
}

LogStream& LogStream::operator<<(const Buffer& buf)
{
    *this << buf.toString();
    return *this;
}

LogStream& LogStream::operator<<(const GeneralTemplate& g)
{
    buffer_.append(g.data_, g.len_);
    return *this;
}