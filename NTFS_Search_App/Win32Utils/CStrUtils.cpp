#include "CStrUtils.h"
#include <fstream>
#include <algorithm>
#include <locale>
#include <tchar.h>

#define FORMAT_COUNT_MAX    (1024 * 1024 * 64)

// ANSI GBK 编码标准
// 
// 汉字区
// GBK/2：0XBOA1-F7FE 收录 GB 2312 汉字 6763 个，按原序排列
// GBK/3：0X8140-AOFE，收录 CJK 汉字 6080 个
// GBK/4：0XAA40-FEAO，收录 CJK 汉字和增补的汉字 8160 个
// 
// 图形符号区
// GBK/1：0XA1A1-A9FE，除 GB 2312 的符号外，还增补了其它符号
// GBK/5：0XA840-A9AO，扩除非汉字区
// 
// 用户自定义区
// GBK 区域中的空白区，用户可以自己定义字符

// UTF-8 编码标准
// 
// 1字节 U+0000000 - U+0000007F 0xxxxxxx
// 2字节 U+0000080 - U+000007FF 110xxxxx 10xxxxxx
// 3字节 U+0000800 - U+0000FFFF 1110xxxx 10xxxxxx 10xxxxxx
// 4字节 U+0010000 - U+001FFFFF 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
// 5字节 U+0200000 - U+03FFFFFF 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
// 6字节 U+4000000 - U+7FFFFFFF 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

// UTF16 编码标准
// 
// 基本多语言平面(U+0000 - U+FFFF)
// 
// 辅助平面(U+10000 - U+10FFFF)
// 1.码位减去 0x10000，得到20位的代理值(0x00 - 0xFFFFF)
// 2.高10位（范围0 - 0x3FF）加 0xD800 得到高位代理(0xD800 - 0xDBFF)
// 3.低10位（范围0 - 0x3FF）加 0xDC00 得到低位代理(0xDC00 - 0xDFFF)

// UTF-8 Byte count table
unsigned char g_utf8_bytes_count_table[0x100] = {
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,
    0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x06,0x06,0x00,0x00,
};

// UTF-8 Data mask table
unsigned char g_utf8_data_mask_table[0x100] = {
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,0x7F,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
    0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,
    0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,0x0F,
    0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x03,0x03,0x03,0x03,0x01,0x01,0x00,0x00,
};

template<typename charT>
struct TCharCmpEqual {
    explicit TCharCmpEqual(const std::locale& loc) : loc_(loc) {}
    bool operator()(charT ch1, charT ch2) {
        return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
    }
private:
    const std::locale& loc_;
};

template<typename T>
size_t CStrUtils::_FindNoCase(const T& str1, const T& str2, size_t offset)
{
    T strMain = str1.substr(offset);

    typename T::const_iterator it = std::search(strMain.begin(), strMain.end(),
        str2.begin(), str2.end(), TCharCmpEqual<typename T::value_type>(std::locale()));
    if (it != strMain.end())
    {
        return it - strMain.begin() + offset;
    }

    return _tstring::npos;
}

size_t CStrUtils::FindNoCaseA(const std::string& str1, const std::string& str2, size_t offset)
{
    return _FindNoCase(str1, str2, offset);
}

size_t CStrUtils::FindNoCaseW(const std::wstring& str1, const std::wstring& str2, size_t offset)
{
    return _FindNoCase(str1, str2, offset);
}

size_t CStrUtils::FindNoCase(const _tstring& str1, const _tstring& str2, size_t offset)
{
    return _FindNoCase(str1, str2, offset);
}

std::string CStrUtils::FormatA(LPCSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::string strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //成功则赋值字符串并终止循环
        int nSize = _vsnprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            strResult.resize(nSize);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);

    return strResult;
}

std::wstring CStrUtils::FormatW(LPCWSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::wstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsnwprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            strResult.resize(nSize);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);

    return strResult;
}

_tstring CStrUtils::Format(LPCTSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    _tstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsntprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            strResult.resize(nSize);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);

    return strResult;
}

void CStrUtils::OutputDebugA(LPCSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::string strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsnprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            ::OutputDebugStringA(strResult.c_str());
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

void CStrUtils::OutputDebugW(LPCWSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::wstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsnwprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            ::OutputDebugStringW(strResult.c_str());
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

void CStrUtils::OutputDebug(LPCTSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    _tstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsntprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            ::OutputDebugString(strResult.c_str());
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

std::vector<std::string> CStrUtils::SplitStrA(const std::string& str, const std::string& delim)
{
    std::vector<std::string> vectorOut;
    size_t iStart = 0;
    size_t iEnd = 0;

    if (delim.empty())
    {
        vectorOut.push_back(str);
    }
    else
    {
        while ((iStart = str.find_first_not_of(delim, iEnd)) != std::string::npos)
        {
            iEnd = str.find(delim, iStart);
            vectorOut.push_back(str.substr(iStart, iEnd - iStart));
        }
    }

    return vectorOut;
}

std::vector<std::wstring> CStrUtils::SplitStrW(const std::wstring& str, const std::wstring& delim)
{
    std::vector<std::wstring> vectorOut;
    size_t iStart = 0;
    size_t iEnd = 0;

    if (delim.empty())
    {
        vectorOut.push_back(str);
    }
    else
    {
        while ((iStart = str.find_first_not_of(delim, iEnd)) != std::wstring::npos)
        {
            iEnd = str.find(delim, iStart);
            vectorOut.push_back(str.substr(iStart, iEnd - iStart));
        }
    }

    return vectorOut;
}

std::vector<_tstring> CStrUtils::SplitStr(const _tstring& str, const _tstring& delim)
{
#ifdef _UNICODE
    return SplitStrW(str, delim);
#else
    return SplitStrA(str, delim);
#endif
}

std::string& CStrUtils::ReplaceA(std::string& strSrc, const std::string& strFind, const std::string& strReplace, bool bCase/* = false*/)
{
    std::string strDest = strSrc;
    size_t nFind = 0;

    if (strFind.empty())
    {
        return strSrc;
    }

    if (!strFind.empty())
    {
        if (bCase)
        {
            while (_tstring::npos != (nFind = strDest.find(strFind, nFind)))
            {
                strDest.replace(nFind, strFind.size(), strReplace);
                nFind += strReplace.size();
            }
        }
        else
        {
            while (_tstring::npos != (nFind = FindNoCaseA(strDest, strFind, nFind)))
            {
                strDest.replace(nFind, strFind.size(), strReplace);
                nFind += strReplace.size();
            } 
        }
    }

    strSrc = strDest;
    return strSrc;
}

std::wstring& CStrUtils::ReplaceW(std::wstring& strSrc, const std::wstring& strFind, const std::wstring& strReplace, bool bCase/* = false*/)
{
    std::wstring strDest = strSrc;
    size_t nFind = 0;

    if (strFind.empty())
    {
        return strSrc;
    }

    if (bCase)
    {
        while (_tstring::npos != (nFind = strDest.find(strFind, nFind)))
        {
            strDest.replace(nFind, strFind.size(), strReplace);
            nFind += strReplace.size();
        };
    }
    else
    {
        while (_tstring::npos != (nFind = FindNoCaseW(strDest, strFind, nFind)))
        {
            strDest.replace(nFind, strFind.size(), strReplace);
            nFind += strReplace.size();
        };
    }

    strSrc = strDest;
    return strSrc;
}

_tstring& CStrUtils::Replace(_tstring& strSrc, const _tstring& strFind, const _tstring& strReplace, bool bCase/* = false*/)
{
    if (strFind.empty())
    {
        return strSrc;
    }

#ifdef _UNICODE
    return ReplaceW(strSrc, strFind, strReplace, bCase);
#else
    return ReplaceA(strSrc, strFind, strReplace, bCase);
#endif
}

int CStrUtils::CompareA(const std::string& strSrc, const std::string& strDest, bool bCase /* = true*/)
{
    if (bCase)
    {
        return strcmp(strSrc.c_str(), strDest.c_str());
    }

    return _stricmp(strSrc.c_str(), strDest.c_str());
}

int CStrUtils::CompareW(const std::wstring& strSrc, const std::wstring& strDest, bool bCase /* = true*/)
{
    if (bCase)
    {
        return wcscmp(strSrc.c_str(), strDest.c_str());
    }

    return _wcsicmp(strSrc.c_str(), strDest.c_str());
}

int CStrUtils::Compare(const _tstring& strSrc, const _tstring& strDest, bool bCase /* = true*/)
{
#ifdef _UNICODE
    return CompareW(strSrc, strDest, bCase);
#else
    return CompareA(strSrc, strDest, bCase);
#endif
}

std::string CStrUtils::FromFileA(const std::string& strFile)
{
    std::string strContent;

    std::ifstream inputFile(strFile.c_str(), std::ios::binary | std::ios::in);
    if (!inputFile.is_open())
    {
        return strContent;
    }

    //获取文件大小
    inputFile.seekg(0, std::ios::end);
    std::streamoff nSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::string strBuffer(nSize, 0);
    inputFile.read(&strBuffer[0], nSize);
    size_t nByteSize = (size_t)inputFile.gcount();
    if (nByteSize > 0)
    {
        strContent = strBuffer;
    }

    return strContent;
}

std::wstring CStrUtils::FromFileW(const std::wstring& strFile)
{
    std::wstring strContent;

    std::ifstream inputFile(strFile.c_str(), std::ios::binary | std::ios::in);
    if (!inputFile.is_open())
    {
        return strContent;
    }

    //获取文件大小
    inputFile.seekg(0, std::ios::end);
    std::streamoff nSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    std::wstring strBuffer(nSize / sizeof(wchar_t), 0);
    inputFile.read((char*)&strBuffer[0], nSize);
    size_t nByteSize = (size_t)inputFile.gcount();
    if (nByteSize > 0)
    {
        strContent = strBuffer;
    }

    inputFile.close();

    return strContent;
}

_tstring CStrUtils::FromFile(const _tstring& strFile)
{
#ifdef _UNICODE
    return FromFileW(strFile);
#else
    return FromFileA(strFile);
#endif
}

bool CStrUtils::ToFileA(const std::string& str, const std::string& strFile)
{
    std::ofstream outputFile(strFile.c_str(), std::ios::binary | std::ios::out);
    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile.write(str.c_str(), str.size());
    outputFile.close();

    return true;
}

bool CStrUtils::ToFileW(const std::wstring& str, const std::wstring& strFile)
{
    std::ofstream outputFile(strFile.c_str(), std::ios::binary | std::ios::out);
    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile.write((const char*)str.c_str(), str.size() * sizeof(wchar_t));
    outputFile.close();

    return true;
}

bool CStrUtils::ToFile(const _tstring& str, const _tstring& strFile)
{
#ifdef _UNICODE
    return ToFileW(str, strFile);
#else
    return ToFileA(str, strFile);
#endif
}

std::string CStrUtils::ToUpperStrA(const std::string& str)
{
    std::string strResult = str;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), [](char ch)
        {
            if ((ch >= 'a') && (ch <= 'z'))
            {
                ch = ch - ('a' - 'A');
            }

            return ch;
        }
    );

    return strResult;
}

std::wstring CStrUtils::ToUpperStrW(const std::wstring& str)
{
    std::wstring strResult = str;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), [](wchar_t ch)
        {
            if ((ch >= L'a') && (ch <= L'z'))
            {
                ch = ch - (L'a' - L'A');
            }

            return ch;
        }
    );

    return strResult;
}

_tstring CStrUtils::ToUpperStr(const _tstring& str)
{
#ifdef _UNICODE
    return ToUpperStrW(str);
#else
    return ToUpperStrA(str);
#endif
}

std::string CStrUtils::ToLowerStrA(const std::string& str)
{
    std::string strResult = str;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), [](char ch)
        {
            if ((ch >= 'A') && (ch <= 'Z'))
            {
                ch = ch + ('a' - 'A');
            }

            return ch;
        }
    );

    return strResult;
}

std::wstring CStrUtils::ToLowerStrW(const std::wstring& str)
{
    std::wstring strResult = str;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), [](wchar_t ch)
        {
            if ((ch >= L'A') && (ch <= L'Z'))
            {
                ch = ch + (L'a' - L'A');
            }

            return ch;
        }
    );

    return strResult;
}

_tstring CStrUtils::ToLowerStr(const _tstring& str)
{
#ifdef _UNICODE
    return ToLowerStrW(str);
#else
    return ToLowerStrA(str);
#endif
}

std::string CStrUtils::_WStrToMultiStr(UINT CodePage, const std::wstring& str)
{
    //计算缓冲区所需的字节长度
    int cbMultiByte = ::WideCharToMultiByte(CodePage, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    std::string strResult(cbMultiByte, 0);

    //成功则返回写入到指示的缓冲区的字节数
    size_t nConverted = ::WideCharToMultiByte(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size(), NULL, NULL);

    //调整内容长度
    strResult.resize(nConverted);
    return strResult;
}

std::wstring CStrUtils::_MultiStrToWStr(UINT CodePage, const std::string& str)
{
    //计算缓冲区所需的字符长度
    int cchWideChar = ::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, NULL, 0);
    std::wstring strResult(cchWideChar, 0);

    //成功则返回写入到指示的缓冲区的字符数
    size_t nConverted = ::MultiByteToWideChar(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size());

    //调整内容长度
    strResult.resize(nConverted);
    return strResult;
}

std::string CStrUtils::WStrToAStr(const std::wstring& str)
{
    return _WStrToMultiStr(CP_ACP, str);
}

std::string CStrUtils::WStrToU8Str(const std::wstring& str)
{
    return _WStrToMultiStr(CP_UTF8, str);
}

_tstring CStrUtils::WStrToTStr(const std::wstring& str)
{
#ifdef _UNICODE
    return str;
#else
    return _WStrToMultiStr(CP_ACP, str);
#endif
}

std::wstring CStrUtils::AStrToWStr(const std::string& str)
{
    return _MultiStrToWStr(CP_ACP, str);
}

std::string CStrUtils::AStrToU8Str(const std::string& str)
{
    return WStrToU8Str(AStrToWStr(str));
}

_tstring CStrUtils::AStrToTStr(const std::string& str)
{
#ifdef _UNICODE
    return _MultiStrToWStr(CP_ACP, str);
#else
    return str;
#endif
}

std::wstring CStrUtils::U8StrToWStr(const std::string& str)
{
    return _MultiStrToWStr(CP_UTF8, str);
}

std::string CStrUtils::U8StrToAStr(const std::string& str)
{
    return WStrToAStr(U8StrToWStr(str));
}

_tstring CStrUtils::U8StrToTStr(const std::string& str)
{
#ifdef _UNICODE
    return _MultiStrToWStr(CP_UTF8, str);
#else
    return WStrToAStr(U8StrToWStr(str));
#endif
}

std::string CStrUtils::TStrToAStr(const _tstring& str)
{
#ifdef _UNICODE
    return _WStrToMultiStr(CP_ACP, str);
#else
    return str;
#endif
}

std::wstring CStrUtils::TStrToWStr(const _tstring& str)
{
#ifdef _UNICODE
    return str;
#else
    return AStrToWStr(str);
#endif
}

std::string CStrUtils::TStrToU8Str(const _tstring& str)
{
#ifdef _UNICODE
    return WStrToU8Str(str);
#else
    return WStrToU8Str(AStrToWStr(str));
#endif
}

int CStrUtils::GetUtf8Count(const std::string& str)
{
    return GetUtf8Count(str.c_str(), str.size());
}

int CStrUtils::GetUtf16Count(const std::wstring& str)
{
    return GetUtf16Count(str.c_str(), str.size() * sizeof(wchar_t));
}

int CStrUtils::GetUtf8Count(const void* data_ptr, size_t size/* = -1*/)
{
    const uint8_t* data_pos = (const uint8_t*)data_ptr;
    size_t size_count = size;
    uint32_t code_point = 0;
    int32_t byte_count = 0;
    int32_t char_count = 0;
    bool result_flag = true;

    while ((0 != *data_pos) && (0 != size_count))
    {
        uint8_t ch = *data_pos;

        // ASCII 范围
        if (ch < 0x7F)
        {
            code_point = ch;
            char_count++;
        }
        else
        {
            if (0 == byte_count)
            {
                code_point = 0;
                if (ch >= 0xC0)
                {
                    // 获取字符编码字节数
                    byte_count = g_utf8_bytes_count_table[ch];

                    // 获取首字节码点
                    code_point = ch & g_utf8_data_mask_table[ch];

                    /*
                    if (ch >= 0xC0 && ch <= 0xDF)
                    {
                        byte_count = 2;
                        cp32 = ch & 0x1F;
                    }
                    else if (ch >= 0xE0 && ch <= 0xEF)
                    {
                        byte_count = 3;
                        cp32 = ch & 0x0F;
                    }
                    else if (ch >= 0xF0 && ch <= 0xF7)
                    {
                        byte_count = 4;
                        cp32 = ch & 0x07;
                    }
                    else if (ch >= 0xF8 && ch <= 0xFB)
                    {
                        byte_count = 5;
                        cp32 = ch & 0x03;
                    }
                    else if (ch >= 0xFC && ch <= 0xFD)
                    {
                        byte_count = 6;
                        cp32 = ch & 0x01;
                    }
                    */

                    if (0 == byte_count)
                    {
                        result_flag = false;
                        break;
                    }

                    byte_count--;
                }
                else
                {
                    result_flag = false;
                    break;
                }
            }
            else
            {
                // 非首字节码点字节掩码检查
                if (0x80 != (ch & 0xC0))
                {
                    result_flag = false;
                    break;
                }

                // 码点组合
                code_point = code_point << 6;
                code_point |= ch & 0x3F;

                byte_count--;

                // 统计字符计数, 忽略字节顺序标记(BOM)
                if ((0 == byte_count) && (0xFEFF != code_point))
                {
                    char_count++;
                }
            }
        }

        data_pos++;

        if (-1 != size_count)
        {
            size_count--;
        }
    }

    if (!result_flag)
    {
        return -1;
    }

    return char_count;
}

int32_t CStrUtils::GetUtf16Count(const void* data_ptr, size_t size/* = -1*/)
{
    const uint16_t* data_pos = (const uint16_t*)data_ptr;
    size_t size_count = size;
    uint32_t code_point = 0;
    uint16_t code_point_high = 0;
    uint16_t code_point_low = 0;
    uint16_t code_point_tmp = 0;
    int32_t byte_count = 0;
    int32_t char_count = 0;
    bool big_endian_flag = false;
    bool little_endian_flag = false;
    bool result_flag = true;

    if (-1 != size_count)
    {
        if ((size_count < 2) || (0 != (size_count % 2)))
        {
            return -1;
        }
    }

    while ((0 != *data_pos) && (0 != size_count))
    {
        code_point_tmp = *data_pos;

        // 检查字节序
        if (0xFFFE == code_point_tmp || 0xFEFF == code_point_tmp)
        {
            if (0 == byte_count)
            {
                // 小端序
                if (0xFFFE == code_point_tmp)
                {
                    big_endian_flag = true;
                }

                // 大端序
                if (0xFEFF == code_point_tmp)
                {
                    little_endian_flag = true;
                }
            }
            else
            {
                result_flag = false;
                break;
            }

            // 不可能同时存在小端序和大端序
            if (big_endian_flag && little_endian_flag)
            {
                result_flag = false;
                break;
            }

            data_pos++;

            if (-1 != size_count)
            {
                size_count -= 2;
            }

            continue;
        }

        //字节序转换
        if (big_endian_flag)
        {
            code_point_tmp = ((code_point_tmp >> 8) | (code_point_tmp << 8));
        }

        // 代理区检查
        if (!(code_point_tmp >= 0xD800 && code_point_tmp <= 0xDFFF))
        {
            if (code_point_high > 0)
            {
                result_flag = false;
                break;
            }

            code_point = code_point_tmp;
            char_count++;
        }
        else
        {
            if (0 == byte_count)
            {
                // 高位代理(高10位)
                if (code_point_tmp >= 0xD800 && code_point_tmp <= 0xDBFF)
                {
                    code_point_high = (code_point_tmp - 0xD800);
                    byte_count = 1;
                }
                else
                {
                    result_flag = false;
                    break;
                }
            }
            else
            {
                if (1 == byte_count)
                {
                    // 低位代理(低10位)
                    if ((code_point_tmp >= 0xDC00) && (code_point_tmp <= 0xDFFF))
                    {
                        code_point_low = (code_point_tmp - 0xDC00);
                        code_point = 0x10000 + ((uint32_t)code_point_high << 10 | code_point_low);
                        code_point_low = 0;
                        code_point_high = 0;
                    }
                    else
                    {
                        result_flag = false;
                        break;
                    }
                }

                byte_count--;

                if (0 == byte_count)
                {
                    char_count++;
                }
            }
        }

        data_pos++;

        if (-1 != size_count)
        {
            size_count -= 2;
        }
    }

    if (!result_flag)
    {
        return -1;
    }

    return char_count;
}

std::string CStrUtils::GetErrorMessageA(DWORD dwMessageId, DWORD dwLanguageId/* = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/)
{
    std::string strResult;

    LPSTR pBuffer = NULL;
    DWORD dwFlags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER |    //函数分配一个足够大的缓冲区来保存格式化消息，并将指针放置在 pBuffer 指定的地址处分配的缓冲区
        FORMAT_MESSAGE_FROM_SYSTEM |        //函数应在系统消息表资源搜索请求的消息
        FORMAT_MESSAGE_IGNORE_INSERTS |     //将忽略消息定义中的插入序列（如 %1）
        FORMAT_MESSAGE_MAX_WIDTH_MASK;      //函数忽略消息定义文本中的常规

    ::FormatMessageA(
        dwFlags,            //格式设置选项以及如何解释 lpSource 参数
        NULL,               //消息定义的位置。 此参数的类型取决于 dwFlags 参数中的设置
        dwMessageId,        //请求的消息的消息标识符。 如果 dwFlags 包含 FORMAT_MESSAGE_FROM_STRING，则忽略此参数
        dwLanguageId,       //所请求消息 的语言标识符
        (LPSTR)&pBuffer,    //指向缓冲区的指针
        MAX_PATH,           //如果未设置 FORMAT_MESSAGE_ALLOCATE_BUFFER 标志，则此参数以 TCHAR 为单位指定输出缓冲区的大小。 如果设置了 FORMAT_MESSAGE_ALLOCATE_BUFFER ，则此参数指定要为输出缓冲区分配的最小 TCHAR 数
        NULL                //一个值数组，这些值用作格式化消息中的插入值。 格式字符串中的 %1 指示 Arguments 数组中的第一个值;%2 指示第二个参数;等等
    );

    if (NULL != pBuffer)
    {
        strResult = pBuffer;
        ::LocalFree(pBuffer);
    }

    return strResult;
}

std::wstring CStrUtils::GetErrorMessageW(DWORD dwMessageId, DWORD dwLanguageId/* = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/)
{
    std::wstring strResult;

    LPWSTR pBuffer = NULL;
    DWORD dwFlags =
        FORMAT_MESSAGE_ALLOCATE_BUFFER |    //函数分配一个足够大的缓冲区来保存格式化消息，并将指针放置在 pBuffer 指定的地址处分配的缓冲区
        FORMAT_MESSAGE_FROM_SYSTEM |        //函数应在系统消息表资源搜索请求的消息
        FORMAT_MESSAGE_IGNORE_INSERTS |     //将忽略消息定义中的插入序列（如 %1）
        FORMAT_MESSAGE_MAX_WIDTH_MASK;      //函数忽略消息定义文本中的常规

    ::FormatMessageW(
        dwFlags,            //格式设置选项以及如何解释 lpSource 参数
        NULL,               //消息定义的位置。 此参数的类型取决于 dwFlags 参数中的设置
        dwMessageId,        //请求的消息的消息标识符。 如果 dwFlags 包含 FORMAT_MESSAGE_FROM_STRING，则忽略此参数
        dwLanguageId,       //所请求消息 的语言标识符
        (LPWSTR)&pBuffer,   //指向缓冲区的指针
        MAX_PATH,           //如果未设置 FORMAT_MESSAGE_ALLOCATE_BUFFER 标志，则此参数以 TCHAR 为单位指定输出缓冲区的大小。 如果设置了 FORMAT_MESSAGE_ALLOCATE_BUFFER ，则此参数指定要为输出缓冲区分配的最小 TCHAR 数
        NULL                //一个值数组，这些值用作格式化消息中的插入值。 格式字符串中的 %1 指示 Arguments 数组中的第一个值;%2 指示第二个参数;等等
    );

    if (NULL != pBuffer)
    {
        strResult = pBuffer;
        ::LocalFree(pBuffer);
    }

    return strResult;
}

_tstring CStrUtils::GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId/* = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)*/)
{
#ifdef _UNICODE
    return GetErrorMessageW(dwMessageId, dwLanguageId);
#else
    return GetErrorMessageA(dwMessageId, dwLanguageId);
#endif
}

DATA_UNIT_INFO CStrUtils::FormatByteSize(
    double nBytesSize,
    eUnitType eSrcUnit/* = eUnitType::eUT_Auto*/,
    eUnitType eDestUnit/* = eUnitType::eUT_Auto*/,
    bool fHasUnits/* = true*/,
    bool fSpace/* = true*/,
    int nInteger/* = 1*/,
    int nPrecision/* = 3*/
)
{
    TCHAR szFormatBuf[MAX_PATH] = { 0 };
    TCHAR szResultBuf[MAX_PATH] = { 0 };
    DATA_UNIT_INFO dataUnitInfo;
    bool fSrcBit = false;
    bool fDestBit = false;

    LPCTSTR strUnitByteName[] = {
        _T("B"),
        _T("KB"),
        _T("MB"),
        _T("GB"),
        _T("TB"),
        _T("PB"),
        _T("EB"),
        _T("ZB"),
        _T("YB"),
        _T("BB"),
        _T("NB"),
        _T("DB"),
        _T("CB"),
        _T("XB"),
    };

    LPCTSTR strUnitBitName[] = {
        _T("b"),
        _T("Kb"),
        _T("Mb"),
        _T("Gb"),
        _T("Tb"),
        _T("Pb"),
        _T("Eb"),
        _T("Zb"),
        _T("Yb"),
        _T("Bb"),
        _T("Nb"),
        _T("Db"),
        _T("Cb"),
        _T("Xb"),
    };

    // 原始单位 比特 -> 字节
    if (eSrcUnit >= eUnitType::eUT_b && eSrcUnit < eUnitType::eUT_B)
    {
        fSrcBit = true;
        eSrcUnit = (eUnitType)(eSrcUnit + (eUnitType::eUT_B - eUnitType::eUT_b));
    }

    // 目标单位 比特 -> 字节
    if (eDestUnit >= eUnitType::eUT_b && eDestUnit < eUnitType::eUT_B)
    {
        fDestBit = true;
        eDestUnit = (eUnitType)(eDestUnit + (eUnitType::eUT_B - eUnitType::eUT_b));
    }

    // 原始单位转换
    for (int i = eUnitType::eUT_B; i < eSrcUnit; i++)
    {
        nBytesSize *= 1024.0f;
    }

    // 自动
    int nUnitTypeIndex = eUnitType::eUT_B;
    if (eUnitType::eUT_Auto == eDestUnit)
    {
        double nCurUnitSize = 1.0f;
        double nNextUnitSize = 1024.0f;
        int nUnitTypeMaxIndex = eUnitType::eUT_Max - 1;
        for (int i = 0; i < _countof(strUnitByteName) && nUnitTypeIndex < nUnitTypeMaxIndex; i++)
        {
            if ((nBytesSize >= nCurUnitSize && nBytesSize < nNextUnitSize) || 0 == nNextUnitSize || 0 == nBytesSize)
            {
                break;
            }

            nCurUnitSize *= 1024.0f;
            nNextUnitSize *= 1024.0f;
            nUnitTypeIndex++;
        }
        eDestUnit = (eUnitType)nUnitTypeIndex;
    }

    {
        ::_stprintf_s(szFormatBuf, _countof(szFormatBuf), _T("%%%d.%dlf"), nInteger + nPrecision + 1, nPrecision);
        double fUnitSize = 1.0f;
        for (int i = eUnitType::eUT_B; i < eDestUnit; i++)
        {
            fUnitSize *= 1024.0f;
        }

        if (fSrcBit)
        {
            fUnitSize *= 8.0f;
        }

        if (fDestBit)
        {
            nBytesSize *= 8.0f;
        }

        double lfResult = nBytesSize / fUnitSize;
        ::_stprintf_s(szResultBuf, _countof(szResultBuf), szFormatBuf, lfResult);
        dataUnitInfo.strOutput = szResultBuf;
        dataUnitInfo.value = lfResult;

        if (fHasUnits)
        {
            if (fSpace)
            {
                dataUnitInfo.strOutput += _T(" ");
            }

            if (fDestBit)
            {
                dataUnitInfo.strOutput += strUnitBitName[eDestUnit - eUnitType::eUT_B];
                dataUnitInfo.strUnitStr = strUnitBitName[eDestUnit - eUnitType::eUT_B];
                dataUnitInfo.eUnit = (eUnitType)(eDestUnit + (eUnitType::eUT_B - eUnitType::eUT_b));
            }
            else
            {
                dataUnitInfo.strOutput += strUnitByteName[eDestUnit - eUnitType::eUT_B];
                dataUnitInfo.strUnitStr = strUnitByteName[eDestUnit - eUnitType::eUT_B];
                dataUnitInfo.eUnit = eDestUnit;
            }
        }
    }

    return dataUnitInfo;
}

void CStrUtils::ConsoleOutputA(LPCSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::string strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsnprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            ::WriteConsoleA(console, strResult.c_str(), nSize, NULL, NULL);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

void CStrUtils::ConsoleOutputW(LPCWSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    std::wstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsnwprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            ::WriteConsoleW(console, strResult.c_str(), nSize, NULL, NULL);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

void CStrUtils::ConsoleOutput(LPCTSTR pFormat, ...)
{
    size_t nCchCount = MAX_PATH;
    _tstring strResult(nCchCount, 0);
    va_list args;

    va_start(args, pFormat);

    do
    {
        //格式化输出字符串
        int nSize = _vsntprintf_s(&strResult[0], nCchCount, _TRUNCATE, pFormat, args);
        if (-1 != nSize)
        {
            HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
            ::WriteConsole(console, strResult.c_str(), nSize, NULL, NULL);
            break;
        }

        //缓冲大小超限终止
        if (nCchCount >= FORMAT_COUNT_MAX)
        {
            break;
        }

        //重新分配缓冲
        nCchCount *= 2;
        strResult.resize(nCchCount);

    } while (true);

    va_end(args);
}

double CStrUtils::StrToDoubleA(const std::string& str)
{
    return ::strtod(str.c_str(), nullptr);
}

double CStrUtils::StrToDoubleW(const std::wstring& str)
{
    return ::wcstod(str.c_str(), nullptr);
}

double CStrUtils::StrToDouble(const _tstring& str)
{
    return ::_tcstod(str.c_str(), nullptr);
}

std::string CStrUtils::DoubleToStrA(double val)
{
    char szBuf[MAX_PATH] = { 0 };
    ::sprintf_s(szBuf, _countof(szBuf), "%g", val);
    return szBuf;
}

std::wstring CStrUtils::DoubleToStrW(double val)
{
    wchar_t szBuf[MAX_PATH] = { 0 };
    ::swprintf_s(szBuf, _countof(szBuf), L"%g", val);
    return szBuf;
}

_tstring CStrUtils::DoubleToStr(double val)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::_stprintf_s(szBuf, _countof(szBuf), _T("%g"), val);
    return szBuf;
}

float CStrUtils::StrToFloatA(const std::string& str)
{
    return ::strtof(str.c_str(), nullptr);
}

float CStrUtils::StrToFloatW(const std::wstring& str)
{
    return ::wcstof(str.c_str(), nullptr);
}

float CStrUtils::StrToFloat(const _tstring& str)
{
    return ::_tcstof(str.c_str(), nullptr);
}

std::string CStrUtils::FloatToStrA(float val)
{
    char szBuf[MAX_PATH] = { 0 };
    ::sprintf_s(szBuf, _countof(szBuf), "%g", (double)val);
    return szBuf;
}

std::wstring CStrUtils::FloatToStrW(float val)
{
    wchar_t szBuf[MAX_PATH] = { 0 };
    ::swprintf_s(szBuf, _countof(szBuf), L"%g", (double)val);
    return szBuf;
}

_tstring CStrUtils::FloatToStr(float val)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::_stprintf_s(szBuf, _countof(szBuf), _T("%g"), (double)val);
    return szBuf;
}

int64_t CStrUtils::StrToIntA(const std::string& str, int radix/* = 10*/)
{
    return ::strtoll(str.c_str(), nullptr, radix);
}

int64_t CStrUtils::StrToIntW(const std::wstring& str, int radix/* = 10*/)
{
    return ::wcstoll(str.c_str(), nullptr, radix);
}

int64_t CStrUtils::StrToInt(const _tstring& str, int radix/* = 10*/)
{
    return ::_tcstoll(str.c_str(), nullptr, radix);
}

std::string CStrUtils::IntToStrA(int64_t val, int radix/* = 10*/)
{
    char szBuf[MAX_PATH] = { 0 };
    ::_i64toa_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}

std::wstring CStrUtils::IntToStrW(int64_t val, int radix/* = 10*/)
{
    wchar_t szBuf[MAX_PATH] = { 0 };
    ::_i64tow_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}

_tstring CStrUtils::IntToStr(int64_t val, int radix/* = 10*/)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::_i64tot_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}

uint64_t CStrUtils::StrToUIntA(const std::string& str, int radix/* = 10*/)
{
    return ::strtoull(str.c_str(), nullptr, radix);
}

uint64_t CStrUtils::StrToUIntW(const std::wstring& str, int radix/* = 10*/)
{
    return ::wcstoull(str.c_str(), nullptr, radix);
}

uint64_t CStrUtils::StrToUInt(const _tstring& str, int radix/* = 10*/)
{
    return ::_tcstoull(str.c_str(), nullptr, radix);
}

std::string CStrUtils::UIntToStrA(uint64_t val, int radix/* = 10*/)
{
    char szBuf[MAX_PATH] = { 0 };
    ::_ui64toa_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}

std::wstring CStrUtils::UIntToStrW(uint64_t val, int radix/* = 10*/)
{
    wchar_t szBuf[MAX_PATH] = { 0 };
    ::_ui64tow_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}

_tstring CStrUtils::UIntToStr(uint64_t val, int radix/* = 10*/)
{
    TCHAR szBuf[MAX_PATH] = { 0 };
    ::_ui64tot_s(val, szBuf, _countof(szBuf), radix);
    return szBuf;
}
