#ifndef TREENODE_H_INCLUDED
#define TREENODE_H_INCLUDED
#include<stdint.h>
class TreeNode
{
public:
    uint8_t data;
    int count;          //出现的次数
    TreeNode* lchild;
    TreeNode* rchild;
    TreeNode() {};
    TreeNode(uint8_t d,int f,TreeNode* l=nullptr,TreeNode* r=nullptr)
    :data(d), count(f), lchild(l), rchild(r){}
    friend bool operator < (TreeNode A,TreeNode B){
        return A.count > B.count;}//这里把小于号重载为大于号是为了后面使用默认的优先队列
};


#endif