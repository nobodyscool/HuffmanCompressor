#include<iostream>
#include<fstream>
#include<chrono>
#include"Tree.h"
#include"directory.h"
using namespace std;
using namespace chrono;

void prttime(time_point<high_resolution_clock>& start,time_point<high_resolution_clock>& end){
    auto duration=duration_cast<milliseconds>(end - start);
    int minutes=duration_cast<chrono::minutes>(duration).count();
    duration-=chrono::minutes(minutes);
    int seconds=duration_cast<chrono::seconds>(duration).count();
    duration-=chrono::seconds(seconds);
    int milliseconds=duration.count();
    cout<<"用时: "<<minutes<<"分 "<<seconds<<"秒 " <<milliseconds<<"毫秒"<<endl;
}

int main(){
    cout<<"----Huffman编码压缩程序----"<<endl;
    cout<<"操作表："<<endl;
    cout<<"1.压缩文件"<<endl<<"2.解压文件"<<endl<<"3.预览"<<endl<<"4.退出程序"<<endl;
    int op;
    string path,name;
    while(1){   
        op=0;
        cout<<"\n\n\n"<<"请输入操作码："<<endl;
        cin>>op;
        if(op==1){
            cout<<"输入待压缩文件路径：\n";
            cin>>path;
            cout<<"请指定压缩文件名称：\n";
            cin>>name;
            auto start=high_resolution_clock::now();
            DirPackage process;
            process.Pack(path,name);
            auto end=high_resolution_clock::now();
            prttime(start,end);
        }
        else if(op==2){
            cout<<"输入待解压缩文件路径：\n";
            cin>>path;
            auto start=high_resolution_clock::now();
            DirUnPackage process;
            process.UnPack(path);
            auto end=high_resolution_clock::now();
            prttime(start,end);
        }
        else if(op==3){
            cout<<"输入待预览的文件路径：\n";
            cin>>path;
            DirUnPackage process;
            process.preview(path);
        }
        else if(op==4) break;
        else cout<<"无效操作码"<<endl;
    }
    return 0;
}