#include<iostream>
#include<fstream>
#include<sstream>
#include"compress.h"
using namespace std;

void BaseCompression::create_Tree(){
    for(int i=0; i<256; i++){
        if(CountTable[i]!=0){
            TreeNode temp(i, CountTable[i]);
            Q.push(temp);
        }
    }
    while(Q.size()>1){
        TreeNode* templeft=new TreeNode(Q.top());
        Q.pop();
        TreeNode* tempright=new TreeNode(Q.top());
        Q.pop();
        TreeNode root(0, templeft->count+tempright->count,templeft,tempright);
        Q.push(root);
    }

}

void BaseCompression::get_Huffman_Code(TreeNode* root, string temp){
    if(root->lchild==NULL && root->rchild==NULL){
        HuffmanCode[root->data]=temp;
        return;
    }
    else if(root->lchild&&root->rchild){
        get_Huffman_Code(root->lchild, temp+"0");
        get_Huffman_Code(root->rchild, temp+"1");
    }
}

Compression::Compression(string path){
    PATH_IN=path;
    for(int i=0; i<256; i++){
        CountTable[i]=0;
        HuffmanCode[i]="";
    }
}

void Compression::save_data(uint64_t file_length){
    ifstream sourcefile(PATH_IN, ios::binary);//读
    stringstream buffer;
    buffer.write((char *)&file_length,8);
    for(int i=0; i<256; i++)
        buffer.write((char *)&CountTable[i],4);
    uint8_t data=sourcefile.get();
    int index=0;
    uint8_t temp;
    while(!sourcefile.eof()){
        int length=HuffmanCode[data].length();
        for(int i=0; i<length; i++){
            if(HuffmanCode[data][i]=='1')
                temp=temp|(1<<index);
            index++;
            if(index==8){
                buffer.write((char*)&temp,1);
                index=0;
                temp=0;
            }
        }
        data=sourcefile.get();
    }
    if(index>0)
        buffer.write((char*)&temp,1);
    sourcefile.close();
    databuffer=buffer.str();
}

string Compression::Compress(){
    ifstream sourcefile(PATH_IN, ios::binary);
    uint8_t data;
    uint64_t file_length=0;//记录原文件长度
    data=sourcefile.get();//八位八位地读取文件
    while(!sourcefile.eof()){
        file_length++;
        CountTable[data]++;
        data=sourcefile.get();
    }
    sourcefile.close();
    before=file_length;
    if(file_length==0) return databuffer;
    create_Tree();
    get_Huffman_Code(new TreeNode(Q.top()), "");
    save_data(file_length);
    return databuffer;
}

DeCompression::DeCompression(string buf){
    databuffer=buf;
    for(int i=0; i<256; i++){
        CountTable[i]=0;
        HuffmanCode[i]="";
    }
}

string DeCompression::DeCompress(){
    stringstream out,sourcefile;
    sourcefile<<databuffer;;
    uint64_t file_length;
    sourcefile.read((char*)&file_length,8);
    for(int i=0; i<256; i++)
        sourcefile.read((char *)&CountTable[i],4);
    if(file_length==0) return "";
    create_Tree();
    get_Huffman_Code(new TreeNode(Q.top()), "");
    //写入数据
    uint8_t curdata=0;
    int index=0;
    sourcefile.read((char*)&curdata, 1);
    while(file_length--){
        TreeNode record=Q.top();
        while(true){
            if(record.lchild==NULL && record.rchild==NULL){
                out.write((char*)&record.data, 1);
                record=Q.top();
                break;
            }
            if(curdata&(1<<index)) {record=*record.rchild;}
            else {record=*record.lchild;}
            index++;
            if(index==8){
                sourcefile.read((char*)&curdata, 1);
                index=0;
            }
        }
    }
    return out.str();
}