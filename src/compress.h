#ifndef COMPRESS_H_INCLUDED
#define COMPRESS_H_INCLUDED
#include<string.h>
#include<queue>
#include"Tree.h"
using namespace std;
class BaseCompression
{protected:
    int CountTable[256];//记录每个字符出现的次数
    string HuffmanCode[256];//记录哈夫曼编码
    priority_queue<TreeNode> Q;//使用优先队列生成哈夫曼树
    string PATH_IN;
    string databuffer;//这里太占用空间了
 public:
    void create_Tree();
    void get_Huffman_Code(TreeNode* ,string);
};

class Compression : public BaseCompression
{
    public:
    unsigned long long before;//记录未压缩前文件长度，用于后续计算压缩率
    Compression(string);
    void save_data(uint64_t);
    string Compress();
};

class DeCompression : public BaseCompression
{
    public:
    DeCompression(string);
    string DeCompress();
};

#endif