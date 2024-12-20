#pragma once

#include <wtypesbase.h>
#include <string>
#include <vector>
#include <tchar.h>

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

// 数据单位
enum eUnitType
{
    eUT_Auto,   // 自动
    eUT_b,      // (2 ^ 000) Bit
    eUT_Kb,     // (2 ^ 010) Bit
    eUT_Mb,     // (2 ^ 020) Bit
    eUT_Gb,     // (2 ^ 030) Bit
    eUT_Tb,     // (2 ^ 040) Bit
    eUT_Pb,     // (2 ^ 050) Bit
    eUT_Eb,     // (2 ^ 060) Bit
    eUT_Zb,     // (2 ^ 070) Bit
    eUT_Yb,     // (2 ^ 080) Bit
    eUT_Bb,     // (2 ^ 090) Bit
    eUT_Nb,     // (2 ^ 100) Bit
    eUT_Db,     // (2 ^ 110) Bit
    eUT_Cb,     // (2 ^ 120) Bit
    eUT_Xb,     // (2 ^ 130) Bit

    eUT_B,      // Byte          2 ^ 000 Byte
    eUT_KB,     // Kilobyte      2 ^ 010 Byte 
    eUT_MB,     // Megabyte      2 ^ 020 Byte 
    eUT_GB,     // Gigabyte      2 ^ 030 Byte 
    eUT_TB,     // Terabyte      2 ^ 040 Byte 
    eUT_PB,     // Petabyte      2 ^ 050 Byte 
    eUT_EB,     // Exabyte       2 ^ 060 Byte 
    eUT_ZB,     // Zettabyte     2 ^ 070 Byte 
    eUT_YB,     // Yottabyte     2 ^ 080 Byte 
    eUT_BB,     // Brontobyte    2 ^ 090 Byte 
    eUT_NB,     // NonaByte      2 ^ 100 Byte 
    eUT_DB,     // DoggaByte     2 ^ 110 Byte 
    eUT_CB,     // corydonbyte   2 ^ 120 Byte 
    eUT_XB,     // Xerobyte      2 ^ 130 Byte
    eUT_Max
};

typedef struct _DATA_UNIT_INFO
{
    double value;           // 数值
    eUnitType eUnit;        // 单位
    _tstring strUnitStr;    // 单位字符串
    _tstring strOutput;     // 内容(数值 + 单位字符串)
}DATA_UNIT_INFO;

// 字符串转换实用类
class CStrUtils
{
public:

    // 从文件载入字符串
    static std::string FromFileA(const std::string& strFile);
    static std::wstring FromFileW(const std::wstring& strFile);
    static _tstring FromFile(const _tstring& strFile);

    // 字符串保存到文件
    static bool ToFileA(const std::string& str, const std::string& strFile);
    static bool ToFileW(const std::wstring& str, const std::wstring& strFile);
    static bool ToFile(const _tstring& str, const _tstring& strFile);

    // 转换大写
    static std::string ToUpperStrA(const std::string& str);
    static std::wstring ToUpperStrW(const std::wstring& str);
    static _tstring ToUpperStr(const _tstring& str);

    // 转换小写
    static std::string ToLowerStrA(const std::string& str);
    static std::wstring ToLowerStrW(const std::wstring& str);
    static _tstring ToLowerStr(const _tstring& str);

    // 分割字符串
    static std::vector<std::string> SplitStrA(const std::string& str, const std::string& delim);
    static std::vector<std::wstring> SplitStrW(const std::wstring& str, const std::wstring& delim);
    static std::vector<_tstring> SplitStr(const _tstring& str, const _tstring& delim);

    // 替换字符串
    static std::string& ReplaceA(std::string& strSrc, const std::string& strFind, const std::string& strReplace, bool bCase = false);
    static std::wstring& ReplaceW(std::wstring& strSrc, const std::wstring& strFind, const std::wstring& strReplace, bool bCase = false);
    static _tstring& Replace(_tstring& strSrc, const _tstring& strFind, const _tstring& strReplace, bool bCase = false);

    // 比较字符串
    static int CompareA(const std::string& strSrc, const std::string& strDest, bool bCase = true);
    static int CompareW(const std::wstring& strSrc, const std::wstring& strDest, bool bCase = true);
    static int Compare(const _tstring& strSrc, const _tstring& strDest, bool bCase = true);

    // 格式化字符串
    static std::string FormatA(LPCSTR pFormat, ...);
    static std::wstring FormatW(LPCWSTR pFormat, ...);
    static _tstring Format(LPCTSTR pFormat, ...);

    // 输出调试字符串
    static void OutputDebugA(LPCSTR pFormat, ...);
    static void OutputDebugW(LPCWSTR pFormat, ...);
    static void OutputDebug(LPCTSTR pFormat, ...);

    // 查找字符串位置
    static size_t FindNoCaseA(const std::string& str1, const std::string& str2, size_t offset = 0);
    static size_t FindNoCaseW(const std::wstring& str1, const std::wstring& str2, size_t offset = 0);
    static size_t FindNoCase(const _tstring& str1, const _tstring& str2, size_t offset = 0);

    // 宽字符串转换
    static std::string WStrToU8Str(const std::wstring& str);
    static std::string WStrToAStr(const std::wstring& str);
    static _tstring WStrToTStr(const std::wstring& str);

    // ANSI字符转换
    static std::wstring AStrToWStr(const std::string& str);
    static std::string AStrToU8Str(const std::string& str);
    static _tstring AStrToTStr(const std::string& str);

    // UTF-8字符串转换
    static std::wstring U8StrToWStr(const std::string& str);
    static std::string U8StrToAStr(const std::string& str);
    static _tstring U8StrToTStr(const std::string& str);

    // 中立字符串转换
    static std::string TStrToAStr(const _tstring& str);
    static std::wstring TStrToWStr(const _tstring& str);
    static std::string TStrToU8Str(const _tstring& str);

    // 获取字符个数
    static int GetUtf8Count(const std::string& str);
    static int GetUtf16Count(const std::wstring& str);
    static int GetUtf8Count(const void* data_ptr, size_t size = -1);
    static int GetUtf16Count(const void* data_ptr, size_t size = -1);

    // 获取GetLastError()的错误信息文本
    static std::string GetErrorMessageA(DWORD dwMessageId, DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
    static std::wstring GetErrorMessageW(DWORD dwMessageId, DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));
    static _tstring GetErrorMessage(DWORD dwMessageId, DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT));

    // 数据单位转换
    static DATA_UNIT_INFO FormatByteSize(
        double nBytesSize,                                      // 输入值
        eUnitType eSrcUnit = eUnitType::eUT_Auto,               // 原始单位
        eUnitType eDestUnit = eUnitType::eUT_Auto,              // 目标单位
        bool fHasUnits = true,                                  // 结果字符串数值添加单位
        bool fSpace = true,                                     // 结果字符串数值与单位之间添加空格
        int nInteger = 1,                                       // 整数部分长度
        int nPrecision = 3                                      // 小数部分长度
    );

    // 控制台输出
    static void ConsoleOutputA(LPCSTR pFormat, ...);
    static void ConsoleOutputW(LPCWSTR pFormat, ...);
    static void ConsoleOutput(LPCTSTR pFormat, ...);

    // 浮点转换
    static double StrToDoubleA(const std::string& str);
    static double StrToDoubleW(const std::wstring& str);
    static double StrToDouble(const _tstring& str);
    static std::string DoubleToStrA(double val);
    static std::wstring DoubleToStrW(double val);
    static _tstring DoubleToStr(double val);

    // 浮点转换
    static float StrToFloatA(const std::string& str);
    static float StrToFloatW(const std::wstring& str);
    static float StrToFloat(const _tstring& str);
    static std::string FloatToStrA(float val);
    static std::wstring FloatToStrW(float val);
    static _tstring FloatToStr(float val);

    // 整数转换
    static int64_t StrToIntA(const std::string& str, int radix = 10);
    static int64_t StrToIntW(const std::wstring& str, int radix = 10);
    static int64_t StrToInt(const _tstring& str, int radix = 10);
    static std::string IntToStrA(int64_t val, int radix = 10);
    static std::wstring IntToStrW(int64_t val, int radix = 10);
    static _tstring IntToStr(int64_t val, int radix = 10);

    // 整数转换
    static uint64_t StrToUIntA(const std::string& str, int radix = 10);
    static uint64_t StrToUIntW(const std::wstring& str, int radix = 10);
    static uint64_t StrToUInt(const _tstring& str, int radix = 10);
    static std::string UIntToStrA(uint64_t val, int radix = 10);
    static std::wstring UIntToStrW(uint64_t val, int radix = 10);
    static _tstring UIntToStr(uint64_t val, int radix = 10);

private:

    // 字符串查找
    template<typename T>
    static size_t _FindNoCase(const T& str1, const T& str2, size_t offset);

    // 宽字符转其他
    static std::string _WStrToMultiStr(UINT CodePage, const std::wstring& str);

    // 其他转宽字符
    static std::wstring _MultiStrToWStr(UINT CodePage, const std::string& str);
};
