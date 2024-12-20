# NTFS File Search Library

英文 | [中文](README_cn.md)



## Features

- Quickly scan all files on NTFS formatted drives
- Real-time quick synchronization of file changes (creation, renaming, deletion)
- Support for wildcard queries for file names or file paths
- Automatic file update upon restart, no need to perform a full disk scan again



## API Description

- Initialize and specify the drive you are concerned with.

  ```C++
  //
  // @brief: Initialize(Drive Mask)
  // @param: dwDriveIndexMask     Drive index mask.(Bit combination: C: 0x01 D: 0x02 E: 0x04...)
  // @param: strDbPath            Database file path
  // @param: fRebuildDb           Whether to rebuild the database
  // @ret:   BOOL                 Whether the operation was successful.
  BOOL NTFS_Search_Initialize_By_Drive_Mask(
      DWORD dwDriveIndexMask, 
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  
  //
  // @brief: Initialize(Drive Letter)
  // @param: strDriveList         Drive list, Such as: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  // @param: strDbPath            Database file path
  // @param: fRebuildDb           Whether to rebuild the database
  // @ret:   BOOL                 Whether the operation was successful.
  BOOL NTFS_Search_Initialize_By_Drive_Letter(
      LPCTSTR lpDriveList,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  ```

- Reset and specify the drive you are concerned with.

  ```C++
  
  //
  // @brief: Reset(Drive Mask)
  // @param: dwDriveIndexMask     Drive index mask.(Bit combination: C: 0x01 D: 0x02 E: 0x04...)
  // @param: strDbPath            Database file path
  // @param: fRebuildDb           Whether to rebuild the database
  // @ret:   BOOL                 Whether the operation was successful.
  NTFSSEARCH_API BOOL NTFS_Search_Reset_By_Drive_Mask(
      DWORD dwDriveIndexMask,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  
  //
  // @brief: Reset(Drive Letter)
  // @param: strDriveList         Drive list, Such as: "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
  // @param: strDbPath            Database file path
  // @param: fRebuildDb           Whether to rebuild the database
  // @ret:   BOOL                 Whether the operation was successful.
  NTFSSEARCH_API BOOL NTFS_Search_Reset_By_Drive_Letter(
      LPCTSTR lpDriveList,
      LPCTSTR lpDbPath,
      bool fRebuildDb
  );
  ```

  

- Get the current total number of files.

  ```C++
  //
  // @brief: Get the current total number of files.
  // @ret:   LONGLONG               Current total number of files
  NTFSSEARCH_API LONGLONG NTFS_Search_GetCount();
  ```

- Search for files, supporting wildcards (* matches 0 or more characters, ? matches 1 character).

  ```C++
  //
  // @brief: Query file path.
  // @param: lpKeyword            Keyword, such as: "C:\*.zip"
  // @param: cb                   Query callback function (return FALSE to terminate result enumeration).
  // @param: lpData               Callback function additional parameters
  // @ret:   BOOL                 Whether the operation was successful.
  NTFSSEARCH_API BOOL NTFS_Search_Query(
      LPCTSTR lpKeyword,
      NtfsSearchCallback cb,
      LPVOID lpData
  );
  ```

- Uninitialize

  ```C++
  //
  // @brief: Uninitialize
  // @ret:   void                 Whether the operation was successful.
  NTFSSEARCH_API VOID NTFS_Search_Uninitialize();
  ```

  

## Performance Description

- Good performance
  - Full disk scan of 770,000 files takes about 13 seconds
  - Searching for * takes 1.3 seconds
  - Searching for *.zip takes 0.2 seconds



- Low memory usage
  - Memory usage for 770,000 files is only around 100MB



- Database file occupancy
  - Database storage for 770,000 files occupies over 300MB of disk space



## Usage Examples

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

    // Initialize
    NTFS_Search_Initialize_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), false);
    if (0 == NTFS_Search_GetCount())
    {
        _tprintf(_T("Scan the specified drive: %s\n"), strDriveList.c_str());
        NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
        _tprintf(_T("Scanning Time Taken: %dms\n"), tmEnd - tmBegin);
    }

    tmEnd = ::clock();

    _tstring strKey;

    while (true)
    {
        char szBuf[MAX_PATH] = { 0 };

        _tprintf(_T("File count: %llu\n"), NTFS_Search_GetCount());
        _tprintf(_T("Command: \n"));
        _tprintf(_T("    :r  Rescan, such as: :r\n"));
        _tprintf(_T("    ::  Rescan the specified drive, such as: ::CDEF\n"));
        _tprintf(_T("    :q  Quit, such as: :q\n"));
        _tprintf(_T("Find keyword:
: "));

        strKey.clear();
        while (strKey.empty())
        {
            gets_s(szBuf, sizeof(szBuf));
            strKey = AStrToTStr(szBuf);
        }

        if (0 == _strnicmp(szBuf, "::", 2))
        {
            strDriveList = strKey.substr(2);
            _tprintf(_T("Rescan the specified drive: %s\n"), strDriveList.c_str());
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("Total time taken: %dms\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":r"))
        {
            _tprintf(_T("Rescan\n"));
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("Total time taken: %dms\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":q"))
        {
            _tprintf(_T("Quit\n"));
            break;
        }

        _tprintf(_T(R"(Searching...)"));
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
        _tprintf(_T("Time taken: %gseconds Number of files: %d \n"), (double)((tmEnd - tmBegin)) / 1000.0f, (int)fileList.size());
    }

    // Uninitialize
    NTFS_Search_Uninitialize();

    return 0;
};

```

