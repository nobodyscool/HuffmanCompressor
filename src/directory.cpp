#include<iostream>
#include<fstream>
#include"directory.h"

int BaseDirectory::is_Dir(string path){
    //返回0是文件，返回1是文件夹，返回-1是目录不存在
    struct _finddata_t fileinfo;
    long long int hFile=0;
    hFile=_findfirst((path).c_str(), &fileinfo);
    if(hFile!=-1){
        bool temp=fileinfo.attrib& _A_SUBDIR;//为0时是文件，非零时是文件夹
        _findclose(hFile);
        return temp;}
    //hFile==-1时目录不存在，后续处理
    return -1;
}

void BaseDirectory::get_Dir(string path, vector<string>&dir, vector<string>&file){
    struct _finddata_t fileinfo;
    long long int hFile=0;
    hFile=_findfirst((path+"\\*").c_str(), &fileinfo);//当前路径下第一个文件or文件夹
    if(hFile!=-1){
        while(true){//这个死循环写的不太好可以用for循环改写
            if((fileinfo.attrib& _A_SUBDIR)){
				if(strcmp(fileinfo.name, ".") != 0&& strcmp(fileinfo.name, "..") != 0) {
					dir.push_back(fileinfo.name);
				}//该if为假时表示打开的目录没有子文件or子文件夹
			}
			else{file.push_back(fileinfo.name);}
            if(_findnext(hFile, &fileinfo)!=0) break;
        }
        _findclose(hFile);
    }
}

void DirPackage::Create_File_Node(string path,string name, int u){
    vector<string> dirs,files;
    get_Dir(path,dirs,files);
    fileNode[u].isDir=1;
    fileNode[u].name=name;
    for(string filename:files){
        fileCount++;
        int v=fileCount;
        relation[u].push_back(v);
        stringstream temp;
        Compression a(path+"\\"+filename);
        temp<<a.Compress();
        before+=a.before;
        fileNode[v].isDir=0;
	    fileNode[v].name=filename;
	    fileNode[v].st=file_data.length();
	    fileNode[v].len=temp.str().length();
        file_data+=temp.str();//将文件内容写入类里的缓冲区
    }
    for(string dirname:dirs){
        fileCount++;
        int v=fileCount;
        relation[u].push_back(v);
        Create_File_Node(path+"\\"+dirname,dirname,v);
    }

}

void DirPackage::single_file_Pack(string path,string setname){
    string PATH_OUT=path;
    PATH_OUT.replace(PATH_OUT.find_last_of('\\')+1,std::string::npos, setname+".hfmp");
    if(is_Dir(PATH_OUT)!=-1){
        cout<<"当前目录下存在"<<PATH_OUT<<"  是否要覆盖该文件（夹）的内容？"<<endl<<"覆盖请输入y，中止请输入n：";
        char op;
        cin>>op;
        if(op!='y') {cout<<"中止成功"<<endl;return;}
        }
    Compression a(path);
    stringstream temp;
    temp<<114514<<endl;
    temp<<0<<endl;
    temp<<path.substr(path.find_last_of('\\')+1)<<endl;//记录原文件名
    temp<<a.Compress();
    ofstream targetfile(PATH_OUT,ios::binary);
    targetfile<<temp.str();
    unsigned long long after=temp.str().length();
    unsigned long long before=a.before;
    targetfile.close();
    cout<<"压缩完成"<<endl;
    if(before!=0)
        cout<<"压缩率："<<(100*after)/before<<"%"<<endl;
    else  cout<<"压缩率："<<100<<"%(空文件)"<<endl;
    return;
}

void DirPackage::Pack(string path,string setname){
    if(is_Dir(path)==-1){cout<<"路径不存在，请检查输入"<<endl;return;}
    if(is_Dir(path)==0){single_file_Pack(path,setname);return;}
    stringstream buffer;
    string PATH_OUT=path;
    PATH_OUT.replace(PATH_OUT.find_last_of('\\')+1,std::string::npos, setname+".hfmp");//设置输出路径
    if(is_Dir(PATH_OUT)!=-1){
        cout<<"当前目录下存在"<<PATH_OUT<<"  是否要覆盖该文件（夹）的内容？"<<endl<<"覆盖请输入y，中止请输入n：";
        char op;
        cin>>op;
        if(op!='y') {cout<<"中止成功"<<endl;return;}
    }
    fileCount=1;
    Create_File_Node(path,path.substr(path.find_last_of('\\')+1),fileCount);//fileNode[0]浪费掉
    buffer<<114514<<endl;//验证码,检测文件来源
    buffer<<1<<endl;//bool isdir=1
    buffer<<fileCount<<endl;
    for(int i=1;i<=fileCount;i++){
        buffer<<fileNode[i].isDir<<" "<<fileNode[i].st<<" "<<fileNode[i].len<<" "<<fileNode[i].name<<endl; 
    }
    for(int i=1;i<=fileCount;i++){
        for(int v:relation[i]){
            buffer<<i<<" "<<v<<endl;
        }
    }
    buffer<<file_data;
    unsigned long long after=buffer.str().length();//压缩后文件大小
    ofstream targetfile(PATH_OUT,ios::binary);
    targetfile<<buffer.str();
    file_data.clear();
    targetfile.close();
    cout<<"压缩完成"<<endl;
    if(before!=0)
        cout<<"压缩率："<<(100*after)/before<<"%"<<endl;
    else cout<<"压缩率："<<100<<"%(空文件夹)"<<endl;
    return;
}

int DirUnPackage::get_File_Node(string PATH_IN,bool ispreview){
    ifstream sourcefile(PATH_IN,ios::binary);
    int check,isdir;
    sourcefile>>check;
    if(check!=114514){
        sourcefile.close();
        cout<<"文件不是来自我们的压缩软件，请检查输入"<<endl;
        return 2;
    }
    sourcefile>>isdir;
    if(!isdir) {sourcefile.close(); return 0;}//单文件情况
    sourcefile>>fileCount;//读取文件和文件夹总数
    for(int i=1;i<=fileCount;i++){
        sourcefile>>fileNode[i].isDir>>fileNode[i].st>>fileNode[i].len;
        sourcefile.ignore();//忽略分隔符
        getline(sourcefile,fileNode[i].name);
    }
    for(int i=1;i<=fileCount-1;i++){
        int u,v;
        sourcefile>>u>>v;
        relation[u].push_back(v);
    }
    if(ispreview){sourcefile.close();return 3;}
    sourcefile.ignore();
    stringstream buffer;
    buffer<<sourcefile.rdbuf();
    file_data=buffer.str();
    sourcefile.close();
    return 1;
}

void DirUnPackage::single_file_UnPack(string path){
    string PATH_OUT=path,name;
    stringstream buffer;
    ifstream sourcefile(path,ios::binary);
    int check,isdir;
    sourcefile>>check;
    if(check!=114514){cout<<"文件不是来自我们的压缩软件，请检查输入"<<endl;;return;}
    sourcefile>>isdir;
    sourcefile>>name;
    sourcefile.ignore();
    buffer<<sourcefile.rdbuf();
    PATH_OUT.replace(PATH_OUT.find_last_of('\\')+1,std::string::npos, name);
    if(is_Dir(PATH_OUT)!=-1){
        cout<<"当前目录下存在"<<PATH_OUT<<"  是否要覆盖该文件（夹）的内容？"<<endl<<"覆盖请输入y，中止请输入n：";
        char op;
        cin>>op;
        if(op!='y') {cout<<"中止成功"<<endl;return;}
    }
    ofstream targetfile(PATH_OUT,ios::binary);
    DeCompression a(buffer.str());
    targetfile<<a.DeCompress();
    cout<<"解压完成"<<endl;
    targetfile.close();
    sourcefile.close();
}

void DirUnPackage::unpack_DFS(string path,int u){
    mkdir(path.c_str());
    for(int v:relation[u]){
        if(fileNode[v].isDir)
            unpack_DFS(path+"\\"+fileNode[v].name,v);
        else{
            string buffer=file_data.substr(fileNode[v].st,fileNode[v].len);
            ofstream targetfile(path+"\\"+fileNode[v].name,ios::binary);
            DeCompression a(buffer);
            targetfile<<a.DeCompress();
            targetfile.close();
        }
    }


}

void DirUnPackage::UnPack(string path){
    if(is_Dir(path)==-1){cout<<"路径不存在，请检查输入"<<endl;return;}
    int whatcase=get_File_Node(path,0);
    if(whatcase==2){return;}
    if(whatcase==0){single_file_UnPack(path);return;}
    string PATH_OUT=path;
    PATH_OUT.replace(PATH_OUT.find_last_of('\\')+1,std::string::npos, fileNode[1].name);
    if(is_Dir(PATH_OUT)!=-1){
        cout<<"当前目录下存在"<<PATH_OUT<<"  是否要覆盖该文件（夹）的内容？"<<endl<<"覆盖请输入y，中止请输入n：";
        char op;
        cin>>op;
        if(op!='y') {cout<<"中止成功"<<endl;return;}
    }
    unpack_DFS(PATH_OUT,1);
    cout<<"解压完成"<<endl;
    return;
}

void DirUnPackage::preview(string path){
    int whatcase=get_File_Node(path,1);
    cout<<"——————————————————————————"<<endl;

    //单个文件情况
    if(whatcase==2){return;}
    if(whatcase==0){
    ifstream sourcefile(path,ios::binary);
    string name;
    sourcefile>>name;
    sourcefile>>name;//略过前两行信息
    sourcefile>>name;
    sourcefile.close();
    cout<<"单文件："<<name<<endl;
    cout<<"——————————————————————————"<<endl;
    return;
    }

    //文件夹情况 循环输出树状结构
    cout<<fileNode[1].name<<endl;
    dfsPrtFile(relation[1],"├──");
    cout<<endl;
    cout<<"——————————————————————————"<<endl;
    return;
}

void DirUnPackage:: dfsPrtFile(vector<int> &dir,string prefix){
    for(int i=0;i<dir.size();i++){
        if(i==dir.size()-1){
            prefix.erase(prefix.size()-9);
            prefix=prefix+"└──";
        }
        cout<<prefix+fileNode[dir[i]].name<<endl;
        if(fileNode[dir[i]].isDir) {
            string nextprefix=prefix;
            size_t pos1=nextprefix.find("├──");
            while(pos1!=string::npos){
                nextprefix.replace(pos1,9,"│  ");
                pos1=nextprefix.find("├──");
            }
            size_t pos2=nextprefix.find("└──");
            while(pos2!=string::npos){
                nextprefix.replace(pos2,9,"   ");
                pos2=nextprefix.find("└──");
            }
            nextprefix=nextprefix+"├──";
            dfsPrtFile(relation[dir[i]],nextprefix);
        }
    }
    return;
}
//为当前目录最后一个文件时"├──"变成"└──"
//递归层数变深时修改前缀,前缀的"├──"变成"│  ";前缀的"└──"变成"   "