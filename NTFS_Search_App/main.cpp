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

int _tmain_cn(int argc, LPCTSTR argv[]);
int _tmain_en(int argc, LPCTSTR argv[]);

int _tmain(int argc, LPCTSTR argv[])
{
    return _tmain_cn(argc, argv);
}

int _tmain_cn(int argc, LPCTSTR argv[])
{
    setlocale(LC_ALL, "");

    clock_t tmBegin = ::clock();
    clock_t tmEnd = ::clock();

    _tstring strDriveList = _T("");
    _tstring strDbPath = _T("");

    // ��ʼ��
    NTFS_Search_Initialize_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), false);
    if (0 == NTFS_Search_GetCount())
    {
        _tprintf(_T("ɨ��ָ��������: %s\n"), strDriveList.c_str());
        tmBegin = ::clock();
        NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
        tmEnd = ::clock();
        _tprintf(_T("ɨ���ʱ: %d����\n"), tmEnd - tmBegin);
    }


    _tstring strKey;

    while (true)
    {
        char szBuf[MAX_PATH] = { 0 };

        _tprintf(_T("�ļ���: %llu\n"), NTFS_Search_GetCount());
        _tprintf(_T("����: \n"));
        _tprintf(_T("    :r  ����ɨ��, ��: :r\n"));
        _tprintf(_T("    ::  ����ɨ��ָ��������, ��: ::CDEF\n"));
        _tprintf(_T("    :q  �˳�, ��: :q\n"));
        _tprintf(_T("���ҹؼ���: "));

        strKey.clear();
        while (strKey.empty())
        {
            gets_s(szBuf, sizeof(szBuf));
            strKey = AStrToTStr(szBuf);
        }

        if (0 == _strnicmp(szBuf, "::", 2))
        {
            strDriveList = strKey.substr(2);
            _tprintf(_T("����ɨ��ָ��������: %s\n"), strDriveList.c_str());
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("�ܹ���ʱ: %d����\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":r"))
        {
            _tprintf(_T("����ɨ��\n"));
            tmBegin = ::clock();
            NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
            tmEnd = ::clock();
            _tprintf(_T("�ܹ���ʱ: %d����\n"), tmEnd - tmBegin);
            continue;
        }

        if (0 == _stricmp(szBuf, ":q"))
        {
            _tprintf(_T("�˳�\n"));
            break;
        }

        _tprintf(_T(R"(��ѯ��...)"));
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
        _tprintf(_T("��ʱ: %g�� ���ҽ��: %d \n"), (double)((tmEnd - tmBegin)) / 1000.0f, (int)fileList.size());
    }

    NTFS_Search_Uninitialize();

    return 0;
};

int _tmain_en(int argc, LPCTSTR argv[])
{
    setlocale(LC_ALL, "");

    clock_t tmBegin = ::clock();
    clock_t tmEnd = ::clock();

    _tstring strDriveList = _T("");
    _tstring strDbPath = _T("");

    // Initialize
    NTFS_Search_Initialize_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), false);
    if (0 == NTFS_Search_GetCount())
    {
        _tprintf(_T("Scan the specified drive: %s\n"), strDriveList.c_str());
        tmBegin = ::clock();
        NTFS_Search_Reset_By_Drive_Letter(strDriveList.c_str(), strDbPath.c_str(), true);
        tmEnd = ::clock();
        _tprintf(_T("Scanning Time Taken: %dms\n"), tmEnd - tmBegin);
    }

    _tstring strKey;

    while (true)
    {
        char szBuf[MAX_PATH] = { 0 };

        _tprintf(_T("File count: %llu\n"), NTFS_Search_GetCount());
        _tprintf(_T("Command: \n"));
        _tprintf(_T("    :r  Rescan, such as: :r\n"));
        _tprintf(_T("    ::  Rescan the specified drive, such as: ::CDEF\n"));
        _tprintf(_T("    :q  Quit, such as: :q\n"));
        _tprintf(_T("Find keyword: "));

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
        _tprintf(_T("Time taken: %gs Number of files: %d \n"), (double)((tmEnd - tmBegin)) / 1000.0f, (int)fileList.size());
    }

    // Uninitialize
    NTFS_Search_Uninitialize();

    return 0;
}
