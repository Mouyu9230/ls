//用不同的函数块实现不同的需求,为确保可以组合使用参数，考虑通过在list中增加其他函数是否执行的判定来完成组
//合，而不是分别在各个参数对应的函数块完整的实现对应的目标
//---------------------------------------------------------------
//-a: 显示隐藏文件 
//-l：显示详细信息 
//-R：递归显示
//-t：按时间显示，最新最靠前
//-r：对目录反向排序（以目录的每个首字符所对应的ASCII值进行大到小排序）
//-i：输出i节点索引信息
//-s：在文件名后输出文件大小
#include<stdio.h>
#include<dirent.h>//提供readdir,opendir,closedir,包含dirent结构体
#include<sys/stat.h>//提供stat
#include<sys/types.h>//stat
#include <unistd.h>//stat,getcwd
#include<stdlib.h>//exit...?
#include<string.h>
#include<pwd.h>//getpwduid与返回结构体
#include<grp.h>//getgrgid与返回结构体
#include<time.h>//ctime函数


#define purple "\033[35m"//为后面分颜色输出用宏定义包装对应颜色的代码
#define blue "\033[34m"
#define green "\033[32m"
#define yellow "\033[33m"
#define red "\033[31m"
#define cyan "\033[36m"
#define reset "\033[0m"

int aflag,lflag,Rflag,tflag,rflag,iflag,sflag;

int dcheck(struct stat stats){
    if(S_ISREG(stats.st_mode)!=0){
        return 0;
    }
    return 1;
}

void timesort(){

}

void showinformation(struct stat stats){
    if(S_ISREG(stats.st_mode)!=0){//文件类型（未完成
        printf("-");
    }else{
        printf("d");
    }
    if((stats.st_mode&S_IRUSR)!=0)printf("r");else{printf("-");}//权限
    if((stats.st_mode&S_IWUSR)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXUSR)!=0)printf("x");else{printf("-");}
    if((stats.st_mode&S_IRGRP)!=0)printf("r");else{printf("-");}
    if((stats.st_mode&S_IWGRP)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXGRP)!=0)printf("x");else{printf("-");}
    if((stats.st_mode&S_IROTH)!=0)printf("r");else{printf("-");}
    if((stats.st_mode&S_IWOTH)!=0)printf("w");else{printf("-");}
    if((stats.st_mode&S_IXOTH)!=0)printf("x ");else{printf("- ");}
    printf("%d ",(int)stats.st_nlink);
    struct passwd *pw = getpwuid(stats.st_uid);
    struct group  *gr = getgrgid(stats.st_gid);
    printf("%s %s ",pw->pw_name,gr->gr_name);
    printf("%lld ",(long long int)stats.st_size);
    printf("%s",ctime(&stats.st_mtime));
}

void list(int aflag,int lflag,int Rflag,int tflag,int rflag,int iflag,int sflag,char* path){
    struct dirent contents[9999];//存所有dirent结构体以供排序
    int readcount=0;
    int tcount=0;
    int listcount=0;
    char pathname[9999];
    struct dirent* content;
    if(path==NULL){//当前或指定
        getcwd(pathname,9999);
    }else{
        strcpy(pathname,path);
    }
    if(pathname==NULL){
        printf("error when trying to get current directory");
        exit(EXIT_FAILURE);
    }
    DIR* cdspointer=opendir(pathname);
    if(cdspointer==NULL){ 
        printf("error when trying to open directory");
        exit(EXIT_FAILURE);       
    }
    while(1){//read循环

        content=readdir(cdspointer);
        if(content==NULL){
            break;
        }
        contents[readcount]=*content;
        readcount++;
    }
    if(tflag==1){
        struct stat timesssssssss[99999];
        struct stat aiodoawj;
        while((listcount++)!=readcount-1){
            char wholepath[9999];
            struct stat stats;
            snprintf(wholepath,sizeof(wholepath),"%s/%s",pathname,contents[tcount].d_name);//拼接
            stat(wholepath,&stats);
            timesssssssss[tcount]=stats;
            tcount++;
        }
        listcount=0;
        struct dirent temp;
        for(int i=0;i<readcount-2;i++){
		for(int j=0;j<readcount-2-i;j++){
			if(timesssssssss[j].st_mtime>timesssssssss[j+1].st_mtime){
				temp=contents[j];
				contents[j]=contents[j+1];
				contents[j+1]=temp;
                aiodoawj=timesssssssss[j];
                timesssssssss[j]=timesssssssss[j+1];
                timesssssssss[j+1]=aiodoawj;


			}
		}
	}
    }
    while((listcount++)!=readcount-1){//输出循环  ！！未完成
    char wholepath[9999];
    struct stat stats;
    snprintf(wholepath,sizeof(wholepath),"%s/%s",pathname,content->d_name);//拼接
    stat(wholepath,&stats);
        if(content->d_name[0]!='.'||aflag==1){//-a判定
            if(lflag==1){//-l判定
                showinformation(stats);
            }//根据各种条件分颜色输出
            if(iflag==1){
                printf(blue"%lld "reset,(long long)stats.st_ino);
            }
            if(sflag==1){
                printf(purple"%lld "reset,(long long)stats.st_blocks/2);//文件占用磁盘块数，系统默认1kb一块，stat里面512b算一块，除以二换算*
            }
            if(dcheck(stats)){
                printf(red"%s\n"reset,content->d_name);
            }else if(content->d_name[0]=='.'){
                printf(yellow"%s\n"reset,content->d_name);
            }else{
                printf(cyan"%s\n"reset,content->d_name);
            }

                
        }

    }
    closedir(cdspointer);


}

int main(int argc,char* argv[]){
    int extraposition[10];
    int extrastrcount=0;
    int findflag=0;//遍历寻找参数串并标记以供后续switch解析参数
    int continueflag=0;//遍历输出命令行参数中各个路径对应结果时用来标记，跳过参数字符串
    for(int i=1;i<argc;i++){//标记参数串位置
        if(argv[i][0]=='-'){
            extraposition[extrastrcount]=i;
            extrastrcount++;
        }
    }
    if(extrastrcount!=0){
        for(int s=1;s<argc;s++){
            for(int j=0;j<extrastrcount;j++){
                if(s==extraposition[j]){
                    findflag=1;
                    break;
                }
            }
            if(findflag==1){
    for(int i=1;i<strlen(argv[s]);i++){
          switch(argv[s][i]){
            case 'a':aflag=1;break;
            case 'l':lflag=1;break;
            case 'R':Rflag=1;break;
            case 't':tflag=1;break;
            case 'r':rflag=1;break;
            case 'i':iflag=1;break;
            case 's':sflag=1;break;
        }            
        }
            }
            findflag=0;
        }

    }
    if(extrastrcount==argc-1||argc==1){//没输入路径就getcwd输出当前目录内容
        list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag,NULL);
    }else{//有的话就遍历argv输出，跳过参数字符串
         for(int i=1;i<argc;i++){
            for(int j=0;j<extrastrcount;j++){
                if(i==extraposition[j]){
                    continueflag=1;
                    break;
                }
            }
            if(continueflag==1){
                continueflag=0;
                continue;
            }

        list(aflag,lflag,Rflag,tflag,rflag,iflag,sflag,argv[i]);

    }
    }
    return 0;

}

