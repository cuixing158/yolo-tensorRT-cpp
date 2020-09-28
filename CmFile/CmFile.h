#pragma once
//#include "StdAfx.h"
#include <SDKDDKVer.h>
#include <stdio.h>
#include <tchar.h>
#include <io.h>

#include <shlobj.h>
#include <Commdlg.h>
#include <ShellAPI.h>
#include <WinBase.h>

#include <string>
#include <vector>
#include <omp.h>
#include <fstream>

#include <opencv2/opencv.hpp>

typedef const std::string CStr;
typedef std::vector<std::string> vecS;
typedef unsigned long       DWORD;
typedef int                 BOOL;

#define FALSE               0

#define MOVEFILE_REPLACE_EXISTING       0x00000001
#define MOVEFILE_COPY_ALLOWED           0x00000002
#define MOVEFILE_DELAY_UNTIL_REBOOT     0x00000004
#define MOVEFILE_WRITE_THROUGH          0x00000008

#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define _S(str) ((str).c_str())

//��Ա�������Ǿ�̬�ģ�����Ҫ����������˵���CmFile::GetName()
//���õ�һЩ����
//Copy ��ĳ���ļ����Ƶ���һ���ļ��У���Ҫ��srcȫ·��+���֣��ͣ�dstȫ·��+���֣�
//Move �ƶ��ļ�����һ���ļ��У���Ҫ��srcȫ·��+���֣��ͣ�dstȫ·��+���֣�
//RmFile ɾ���ļ�
//GetName ȥ��·������ȡ���֣�������׺��
//GetNameNE ��ȡ�޺�׺��·�������֣�NE��ʾNo Extention
//GetExtention ��ȡ��׺ �� .jpg  .txt
//GetFoler ����ͼƬ�������ļ���ȫ·��+���֣���ȡ�ַ�����ȡȫ·��
//GetFolderCur ����ȫ·�����ֻ�ȡ��ǰ���ļ�����
//FileExist �ж��ļ��Ƿ����
//FolderExist �ж��ļ����Ƿ����
//MkDir �����ļ���
//GetImageFromFolderAndSub ��ȡĳ���ļ����µ�����ָ����׺���ļ������������ӡ���...�ļ���
//BrowseFile �򿪴���ѡ���ļ�
struct CmFile
{
	static std::string BrowseFile(const char* strFilter = "Images (*.jpg;*.png)\0*.jpg;*.png\0All (*.*)\0*.*\0\0", bool isOpen = true);
	static std::string BrowseFolder(); 

	static inline std::string GetFolderCur(CStr& path);
	static inline std::string GetFolder(CStr& path);
	//static inline vecS GetFolders(CStr& rootPath);// �Ӹ�Ŀ¼path�»�ȡ�¸���Ŀ¼������ȫ·���ļ���
	static inline std::string GetName(CStr& path);
	static inline std::string GetNameNE(CStr& path);
	static inline std::string GetPathNE(CStr& path);

	// Get file names from a wildcard. Eg: GetNames("D:\\*.jpg", imgNames);
	static int GetNames(CStr &nameW, vecS &names, std::string &dir = std::string());
	static int GetNames(CStr& rootFolder, CStr &fileW, vecS &names);
	static int GetNamesNE(CStr& nameWC, vecS &names, std::string &dir = std::string(), std::string &ext = std::string());
	static int GetNamesNE(CStr& rootFolder, CStr &fileW, vecS &names);
	static inline std::string GetExtention(CStr name);

	static inline bool FileExist(CStr& filePath);
	static inline bool FilesExist(CStr& fileW);
	static inline bool FolderExist(CStr& strPath);

	static inline std::string GetWkDir();

	static BOOL MkDir(CStr&  path);

	// Eg: RenameImages("D:/DogImages/*.jpg", "F:/Images", "dog", ".jpg");
	static int Rename(CStr& srcNames, CStr& dstDir, const char* nameCommon, const char* nameExt);

	static inline void RmFile(CStr& fileW);
	static void RmFolder(CStr& dir);
	static void CleanFolder(CStr& dir, bool subFolder = false);

	static int GetSubFolders(CStr& folder, vecS& subFolders);

	inline static BOOL Copy(CStr &src, CStr &dst, BOOL failIfExist = FALSE);
	inline static BOOL Move(CStr &src, CStr &dst, DWORD dwFlags = MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
	static BOOL Move2Dir(CStr &srcW, CStr dstDir);
	static BOOL Copy2Dir(CStr &srcW, CStr dstDir);

	//Load mask image and threshold thus noisy by compression can be removed
	static cv::Mat LoadMask(CStr& fileName);

	static void WriteNullFile(CStr& fileName) {FILE *f = fopen(_S(fileName), "w"); fclose(f);}

	static void ChkImgs(CStr &imgW);

	static void RunProgram(CStr &fileName, CStr &parameters = "", bool waiteF = false, bool showW = true);

	static void SegOmpThrdNum(double ratio = 0.8);

	// Copy files and add suffix. e.g. copyAddSuffix("./*.jpg", "./Imgs/", "_Img.jpg")
	static void copyAddSuffix(CStr &srcW, CStr &dstDir, CStr &dstSuffix);

	static vecS loadStrList(CStr &fName);
	static bool writeStrList(CStr &fName, const vecS &strs);
	//��ȡrootPath�ļ����¼����ӡ��������ļ��������еĺ�׺��suffix���ļ���·�������浽imagesPath�������ܸ���
	static int GetImageFromFolderAndSub(CStr& rootPath, vecS& imagesPath, std::string suffix = ".jpg");

	static std::string CheckPath(CStr& path)
	{
		std::string temp = path;
		if (path.find_last_of("\\/") != path.length() - 1)
		{
			temp += '/';
		}
		return temp;
	}
};

/************************************************************************/
/* Implementation of inline functions                                   */
/************************************************************************/
std::string CmFile::GetFolder(CStr& path)
{
	// ԭ���� return path.substr(0, path.find_last_of("\\/")+1);
	return path.substr(0, path.find_last_of("\\/"));
}

std::string CmFile::GetFolderCur(CStr& path)
{
	std::string pathWhole = path.substr(0, path.find_last_of("\\/")+1);
	pathWhole = pathWhole.substr(0, pathWhole.find_last_not_of("\\/")+1);
	return pathWhole.substr(pathWhole.find_last_of("\\/")+1);
}

std::string CmFile::GetName(CStr& path)
{
	int start = path.find_last_of("\\/")+1;
	int end = path.find_last_not_of(' ')+1;
	return path.substr(start, end - start);
}

std::string CmFile::GetNameNE(CStr& path)
{
	int start = path.find_last_of("\\/")+1;
	int end = path.find_last_of('.');
	if (end >= 0)
		return path.substr(start, end - start);
	else
		return path.substr(start,  path.find_last_not_of(' ')+1 - start);
}

std::string CmFile::GetPathNE(CStr& path)
{
	int end = path.find_last_of('.');
	if (end >= 0)
		return path.substr(0, end);
	else
		return path.substr(0,  path.find_last_not_of(' ') + 1);
}

std::string CmFile::GetExtention(CStr name)
{
	int dot = name.find_last_of('.');
	if (dot >= 0)
		return name.substr(name.find_last_of('.'));
	else
		return "";
}

BOOL CmFile::Copy(CStr &src, CStr &dst, BOOL failIfExist)
{
	return CopyFileA(src.c_str(), dst.c_str(), failIfExist);
}

BOOL CmFile::Move(CStr &src, CStr &dst, DWORD dwFlags)
{
	return MoveFileExA(src.c_str(), dst.c_str(), dwFlags);
}

void CmFile::RmFile(CStr& fileW)
{ 
	vecS names;
	std::string dir;
	int fNum = CmFile::GetNames(fileW, names, dir);
	for (int i = 0; i < fNum; i++)
		::DeleteFileA(_S(dir + names[i]));
}


// Test whether a file exist
bool CmFile::FileExist(CStr& filePath)
{
	if (filePath.size() == 0)
		return false;

	return  GetFileAttributesA(_S(filePath)) != INVALID_FILE_ATTRIBUTES; // ||  GetLastError() != ERROR_FILE_NOT_FOUND;
}

bool CmFile::FilesExist(CStr& fileW)
{
	vecS names;
	int fNum = GetNames(fileW, names);
	return fNum > 0;
}

std::string CmFile::GetWkDir()
{	
	std::string wd;
	wd.resize(1024);
	DWORD len = GetCurrentDirectoryA(1024, &wd[0]);
	wd.resize(len);
	return wd;
}

bool CmFile::FolderExist(CStr& strPath)
{
	int i = (int)strPath.size() - 1;
	for (; i >= 0 && (strPath[i] == '\\' || strPath[i] == '/'); i--)
		;
	std::string str = strPath.substr(0, i+1);

	WIN32_FIND_DATAA  wfd;
	HANDLE hFind = FindFirstFileA(_S(str), &wfd);
	bool rValue = (hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);   
	FindClose(hFind);
	return rValue;
}

/************************************************************************/
/*                   Implementations                                    */
/************************************************************************/


