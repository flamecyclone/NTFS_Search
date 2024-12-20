#pragma once
#include <windows.h>

// NTFS 文件搜索库
// FlameCyclone 2024.12.20

/*
#ifdef NTFSSEARCH_EXPORTS
#define NTFSSEARCH_API __declspec(dllexport)
#else
#define NTFSSEARCH_API __declspec(dllimport)
#endif
*/

#define NTFSSEARCH_API

typedef bool (WINAPI *NtfsSearchCallback)(LPVOID lpData, LPCTSTR lpPath);

extern "C"
{
	//
	// @brief: 初始化(驱动器掩码)
	// @param: dwDriveIndexMask     驱动器索引掩码(位组合: C: 0x01 D: 0x02 E: 0x04...)
	// @param: strDbPath            数据库文件路径
	// @param: fRebuildDb           是否重建数据库
	// @ret:   BOOL                 操作是否成功
	NTFSSEARCH_API BOOL NTFS_Search_Initialize_By_Drive_Mask(
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
	NTFSSEARCH_API BOOL NTFS_Search_Initialize_By_Drive_Letter(
		LPCTSTR lpDriveList,
		LPCTSTR lpDbPath,
		bool fRebuildDb
	);

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

	//
	// @brief: 获取当前文件总数
	// @ret:   size_t               当前文件总数
	NTFSSEARCH_API LONGLONG NTFS_Search_GetCount();

	//
	// @brief: 反初始化
	// @ret:   void                 操作是否成功
	NTFSSEARCH_API VOID NTFS_Search_Uninitialize();
}
