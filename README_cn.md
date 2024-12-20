# NTFS 文件搜索库

[English](README.md) | 中文

一个快速搜索NTFS卷文件的库



## 特性

- 快速扫描 NTFS 格式驱动器上的所有文件
- 实时快速同步文件变更(创建, 更名, 删除)
- 支持通配符查询文件名或文件路径
- 重启自动更新文件变动, 无需重新进行全盘扫描



## API描述

- 初始化并指定你关注的驱动器

  ```C++
  //
  // @brief: 初始化(驱动器掩码)
  // @param: dwDriveIndexMask     驱动器索引掩码(位组合: C: 0x01 D: 0x02 E: 0x04...)
  // @param: strDbPath            数据库文件路径
  // @param: fRebuildDb           是否重建数据库
  // @ret:   BOOL                 操作是否成功
  BOOL NTFS_Search_Initialize_By_Drive_Mask(
      DWORD dwDriveIndexMask, 
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  
  //
  // @brief: 初始化(驱动器盘符)
  // @param: strDriveList         驱动器列表, 如: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  // @param: strDbPath            数据库文件路径
  // @param: fRebuildDb           是否重建数据库
  // @ret:   BOOL                 操作是否成功
  BOOL NTFS_Search_Initialize_By_Drive_Letter(
      LPCTSTR lpDriveList,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  ```

- 重置并指定你关注的驱动器

  ```C++
  
  //
  // @brief: 重置(驱动器掩码)
  // @param: dwDriveIndexMask     驱动器索引掩码(位组合: C: 0x01 D: 0x02 E: 0x04...)
  // @param: strDbPath            数据库文件路径
  // @param: fRebuildDb           是否重建数据库
  // @ret:   BOOL                 操作是否成功
  NTFSSEARCH_API BOOL NTFS_Search_Reset_By_Drive_Mask(
      DWORD dwDriveIndexMask,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  
  //
  // @brief: 重置(驱动器盘符)
  // @param: strDriveList         驱动器列表, 如: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  // @param: strDbPath            数据库文件路径
  // @param: fRebuildDb           是否重建数据库
  // @ret:   BOOL                 操作是否成功
  NTFSSEARCH_API BOOL NTFS_Search_Reset_By_Drive_Letter(
      LPCTSTR lpDriveList,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  ```

  

- 获取当前文件总数

  ```C++
  //
  // @brief: 获取当前文件总数
  // @ret:   LONGLONG               当前文件总数
  NTFSSEARCH_API LONGLONG NTFS_Search_GetCount();
  ```

- 查询文件, 支持通配符(*    匹配 0 个或多个字符 ?    匹配 1 个字符)

  ```C++
  //
  // @brief: 查询文件路径
  // @param: lpKeyword            关键字, 如: "C:\*.zip"
  // @param: cb                   查询回调函数(返回FALSE终止结果枚举)
  // @param: lpData               回调函数附加参数
  // @ret:   BOOL                 操作是否成功
  NTFSSEARCH_API BOOL NTFS_Search_Query(
      LPCTSTR lpKeyword,
      NtfsSearchCallback cb,
      LPVOID lpData
  );
  ```

- 反初始化

  ```C++
  //
  // @brief: 反初始化
  // @ret:   void                 操作是否成功
  NTFSSEARCH_API VOID NTFS_Search_Uninitialize();
  ```

  

## 性能描述

- 拥有较好的性能
  - 全盘扫描 77 万文件耗时约 13秒
  - 搜索 * 耗时 1.3秒
  - 搜索 *.zip 耗时 0.2秒



- 内存占用小
  - 77 万 文件数内存占用仅 100MB 左右



- 数据库文件占用
  - 77 万 文件数据库存储占用 300+ MB 磁盘空间



## 使用例子

```c++
#include <windows.h>
#include <tchar.h>
#include <locale>
#include <string>
#include <vector>
#include "../NTFS_Search_Lib/include/NTFS_Search_Api.h"

#ifdef _UNICODE
using _tstring = std::wstring;
#else
using _tstring = std::string;
#endif

#ifndef _DEBUG

#ifdef _UNICODE

#ifdef _WIN64
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x64_W.lib")
#else
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x86_W.lib")
#endif

#else

#ifdef _WIN64
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x64_A.lib")
#else
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x86_A.lib")
#endif

#endif

#else
#ifdef _UNICODE

#ifdef _WIN64
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x64_WD.lib")
#else
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x86_WD.lib")
#endif

#else

#ifdef _WIN64
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x64_AD.lib")
#else
#pragma comment(lib, "../NTFS_Search_lib/lib/NTFS_Search_Lib_x86_AD.lib")
#endif

#endif

#endif

std::wstring _MultiStrToWStr(UINT CodePage, const std::string& str)
{
    int cchWideChar = ::MultiByteToWideChar(CodePage, 0, str.c_str(), -1, NULL, 0);
    std::wstring strResult(cchWideChar, 0);
    size_t nConverted = ::MultiByteToWideChar(CodePage, 0, str.c_str(), (int)str.size(), &strResult[0], (int)strResult.size());
    strResult.resize(nConverted);
    return strResult;
}

_tstring AStrToTStr(const std::string& str)
{
#ifdef _UNICODE
    return _MultiStrToWStr(CP_ACP, str);
#else
    return str;
#endif
}

int _tmain(int argc, LPCTSTR argv[])
{
    setlocale(LC_ALL, "");

    clock_t tmBegin = ::clock();
    clock_t tmEnd = ::clock();

    _tstring strDriveList = _T("");
    _tstring strDbPath = _T("");
    tmBegin = ::clock();

    // 初始化
    NTFS_Search_Initialize_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), false);
    if (0 == NTFS_Search_GetCount())
    {
        _tprintf(_T("扫描指定驱动器: %s\n"), strDriveList.c_str());
        NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
        _tprintf(_T("扫描耗时: %d毫秒\n"), tmEnd - tmBegin);
    }

    tmEnd = ::clock();

    _tstring strKey;

    while (true)
    {
        char szBuf[MAX_PATH] = { 0 };

        _tprintf(_T("文件数: %llu\n"), NTFS_Search_GetCount());
        _tprintf(_T("命令: \n"));
        _tprintf(_T("    :r  重新扫描, 如: :r\n"));
        _tprintf(_T("    ::  重新扫描指定驱动器, 如: ::CDEF\n"));
        _tprintf(_T("    :q  退出, 如: :q\n"));
        _tprintf(_T("查找关键字: "));

        strKey.clear();
        while (strKey.empty())
        {
            gets_s(szBuf, sizeof(szBuf));
            strKey = AStrToTStr(szBuf);
        }

        if (0 == _strnicmp(szBuf, "::", 2))
        {
            strDriveList = strKey.substr(2);
            _tprintf(_T("重新扫描指定驱动器: %s\n"), strDriveList.c_str());
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("总共耗时: %d毫秒\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":r"))
        {
            _tprintf(_T("重新扫描\n"));
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("总共耗时: %d毫秒\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":q"))
        {
            _tprintf(_T("退出\n"));
            break;
        }

        _tprintf(_T(R"(查询中...)"));
        _tprintf(_T("\n"));

        std::vector<_tstring> fileList;
        tmBegin = ::clock();
        int nRes = NTFS_Search_Query(strKey.c_str(), [](LPVOID lpData, LPCTSTR lpPath) -> bool {

            std::vector<_tstring>* pList = (std::vector<_tstring>*)lpData;
            pList->push_back(lpPath);
            return true;

            },

            &fileList
        );
        tmEnd = ::clock();

        int nIndex = 0;
        for (const auto& item : fileList)
        {
            _tprintf(_T("%d: %s\r\n"), ++nIndex, item.c_str());
            if (nIndex >= 100)
            {
                break;
            }
        }
        _tprintf(_T("\n"));
        _tprintf(_T("耗时: %g秒 查找结果: %d \n"), (double)((tmEnd - tmBegin)) / 1000.0f, (int)fileList.size());
    }

    NTFS_Search_Uninitialize();

    return 0;
};

```

