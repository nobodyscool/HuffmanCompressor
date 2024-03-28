#ifndef DIRECTORY_H_INCLUDED
#define DIRECTORY_H_INCLUDED
#include<fstream>
#include<iostream>
#include<sstream>
#include<vector>
#include<io.h>
#include <direct.h>
#include"compress.h"
using namespace std;

class FileInfomation
{public:
    bool isDir=0;//是否是个文件夹
	string name;//名字
	int st,len;//在字符串中的起始位置，与长度
};

class BaseDirectory
{protected:
    FileInfomation fileNode[5400];//存储文件和文件夹的信息
    vector<int>relation[5400];//存储文件与文件夹之间的从属关系
    int fileCount=0;//记录文件or文件夹的个数
    string file_data;//信息字符串

 public:
    int is_Dir(string path);//判断路径是一个文件夹还是一个文件
    void get_Dir(string path, vector<string>&dir, vector<string>&file);
    //遍历整个文件夹，读出其中子文件夹放在dir中，读出其中文件放在file中
};

class DirPackage:public BaseDirectory
{public:
    unsigned long long before=0;//记录压缩前所有文件的大小之和
    void Create_File_Node(string path,string name,int u);
    void single_file_Pack(string path,string setname);
    void Pack(string path,string setname);
};

class DirUnPackage:public BaseDirectory
{public:
    int get_File_Node(string PATH_IN,bool ispreview);//读取文件头信息，返回情况：0:单文件 1:多文件 2:未知来源文件 3:预览 
    void single_file_UnPack(string path);
    void unpack_DFS(string path,int u);
    void UnPack(string path);
    void dfsPrtFile(vector<int> &dir,string prefix);
    void preview(string path);
};

#endif