#include"FileUtil.h"
#include"Logger.h"

FileUtil::FileUtil(std::string& fileName)
    :fp_(::fopen(fileName.c_str(), "ae")),
      writtenBytes_(0)
{
    // ��fd_����������Ϊ���ص�buffer_
    ::setbuffer(fp_, buffer_, sizeof(buffer_));//setbuffer�����ļ���������
}

FileUtil::~FileUtil()
{
    ::fclose(fp_);
}

void FileUtil::append(const char* data, size_t len)
{   
    // ��¼�Ѿ�д������ݴ�С
    size_t written = 0;

    while (written != len)
    {
        // ����д������ݴ�С
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
        // ����д������ݴ�С
        written += n;
    }
    // ��¼ĿǰΪֹд������ݴ�С���������ƻ������־
    writtenBytes_ += written;
}

void FileUtil::flush()
{
    ::fflush(fp_);
}

size_t FileUtil::write(const char* data, size_t len)
{
    //fwrite_unlocked��fwrite�������汾���̲߳���ȫ������
    //�첽���߳������־������Ҫ�����̰߳�ȫ��
    return ::fwrite_unlocked(data, 1, len, fp_);
}
