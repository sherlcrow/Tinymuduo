#include"FileUtil.h"
#include"Logger.h"

FileUtil::FileUtil(std::string& fileName)
    :fp_(::fopen(fileName.c_str(), "ae")),
      writtenBytes_(0)
{
    // 将fd_缓冲区设置为本地的buffer_
    ::setbuffer(fp_, buffer_, sizeof(buffer_));//setbuffer设置文件流缓冲区
}

FileUtil::~FileUtil()
{
    ::fclose(fp_);
}

void FileUtil::append(const char* data, size_t len)
{   
    // 记录已经写入的数据大小
    size_t written = 0;

    while (written != len)
    {
        // 还需写入的数据大小
        size_t remain = len - written;
        size_t n = write(data + written, remain);
        if (n != remain)
        {
            int err = ferror(fp_);
            if (err)
            {
                fprintf(stderr, "FileUtil::append() failed %s\n", getErrnoMsg(err));
            }
        }
        // 更新写入的数据大小
        written += n;
    }
    // 记录目前为止写入的数据大小，超过限制会滚动日志
    writtenBytes_ += written;
}

void FileUtil::flush()
{
    ::fflush(fp_);
}

size_t FileUtil::write(const char* data, size_t len)
{
    //fwrite_unlocked是fwrite的无锁版本，线程不安全，更快
    //异步单线程输出日志，不需要考虑线程安全？
    return ::fwrite_unlocked(data, 1, len, fp_);
}
