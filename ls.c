//用不同的函数块实现不同的需求,为确保可以组合使用参数，考虑通过在list中增加其他函数是否执行的判定来完成组
//合，而不是分别在各个参数对应的函数块完整的实现对应的目标
//---------------------------------------------------------------
//-a: 显示隐藏文件 决定list在循环获取文件名输出时是否取消检测首字符的环节
//-l：显示详细信息 用stat获取详细信息，在输出文件名的循环前加入判定，若有则输出stat的信息
//-R：递归显示
//-t：按时间显示，最新最靠前
//-r：对目录反向排序（以目录的每个首字符所对应的ASCII值进行大到小排序）
//-i：输出i节点索引信息
//-s：在文件名后输出文件大小
#include<stdio.h>
#include<dirent.h>//提供readdir,opendir,closedir,包含dirent结构体
#include<sys/stat.h>//提供stat
#include <unistd.h>//getcwd
#include<stdlib.h>//exit...?

#define purple "\033[35m"//为后面分颜色输出用宏定义包装对应颜色的代码
#define blue "\033[34m"
#define red "\033[31m"
#define cyan "\033[36m"
#define reset "\033[0m"

int aflag,lflag,Rflag,tflag,rflag,iflag,sflag;

void list(int aflag,int lflag,int Rflag,int tflag,int rflag,int iflag,int sflag){
    char cpathname[999];
    struct dirent* content;
    if(getcwd(cpathname,9999)==NULL){
        printf("error when trying to get current directory");
        exit(EXIT_FAILURE);
    }
    DIR* cdspointer=opendir(cpathname);
    while(1){
        content=readdir(cdspointer);
        if(content==NULL){
            break;
        }
        if(content->d_name[0]!='.'||aflag==1){
            printf("%s\n",content->d_name);
        }

    }


}

int main(){
    char lscheck[9230];
    char extra;
    scanf("%s",lscheck);
    if(lscheck[0]!='l'||lscheck[1]!='s'){
        printf(red"wrong input!"reset);
        return 0;
    }
    while((extra=getchar())!='\n'){
        switch(extra){
            case 'a':aflag=1;break;
            case 'l':lflag=1;break;
            case 'R':Rflag=1;break;
            case 't':tflag=1;break;
            case 'r':rflag=1;break;
            case 'i':iflag=1;break;
            case 's':sflag=1;break;
        }
    }
    list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag);
    return 0;


}   

