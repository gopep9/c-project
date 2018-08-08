#include <QDir>
#include <QFile>

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <QDebug>


char* G2U(const char* gb2312);
void G2UFILE(const char *fileName);
int TraversalDirectory(QString szPath);
bool is_str_utf8(const char* str);


int main(int argc, char *argv[])
{
    char fileName[1024]="d:\\blog";
    //    scanf("%s",fileName);
    
    TCHAR path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH,path);
    wprintf(path);
    
    //    QString pattern(".*txt$");
    //    QRegExp rx(pattern);
    
    //    bool match=rx.exactMatch("a.txt");
    //    qDebug()<<match;
    TraversalDirectory(fileName);
    //    G2UFILE(fileName);
    return 0;
}

int TraversalDirectory(QString szPath)
{
    QDir dir(szPath);
    //    QStringList nameFilters;
    //    nameFilters<<"*.txt";
    QFileInfoList fileList=dir.entryInfoList();
    for(int i=0;i<fileList.size();i++)
    {
        QFileInfo fileInfo=fileList[i];
        if(fileInfo.isDir()&&fileInfo.fileName()!=QString("..")&&fileInfo.fileName()!=QString("."))
        {
            TraversalDirectory(fileInfo.absoluteFilePath());
        }else if(fileInfo.isFile())
        {
            QString pattern(".*txt$");
            QRegExp rx(pattern);
            if(rx.exactMatch(fileInfo.fileName()))
            {
                //                printf("%s\n",fileInfo.absoluteFilePath().toLocal8Bit().data());
                G2UFILE(fileInfo.absoluteFilePath().toLocal8Bit().data());
            }
        }
    }
    return 0;
}

void G2UFILE(const char *fileName)
{
    FILE *pfile=fopen(fileName,"r+");
    
    fseek(pfile,0,SEEK_END);
    int size=ftell(pfile);
    //    printf("size:%d\n",size);
    rewind(pfile);
    
    char *in=new char[size+1]();
    fread(in,1,size,pfile);
    fclose(pfile);
    
    if(is_str_utf8(in))
        return;
    
    printf("file:%s size:%d\n",fileName,size);
    char *out=G2U(in);
    
    pfile=fopen(fileName,"w");
    fprintf(pfile,"%s",out);
    fclose(pfile);
    delete[]in;
    delete[]out;
}

//gb2312转utf8
char* G2U(const char* gb2312)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if(wstr)
    {
        delete[] wstr;
    }
    return str;
}

bool is_str_utf8(const char* str)
{
    unsigned int nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节
    unsigned char chr = *str;
    bool bAllAscii = true;
    for (unsigned int i = 0; str[i] != '\0'; ++i){
        chr = *(str + i);
        //判断是否ASCII编码,如果不是,说明有可能是UTF8,ASCII用7位编码,最高位标记为0,0xxxxxxx
        if (nBytes == 0 && (chr & 0x80) != 0){
            bAllAscii = false;
        }
        if (nBytes == 0) {
            //如果不是ASCII码,应该是多字节符,计算字节数
            if (chr >= 0x80) {
                if (chr >= 0xFC && chr <= 0xFD){
                    nBytes = 6;
                }
                else if (chr >= 0xF8){
                    nBytes = 5;
                }
                else if (chr >= 0xF0){
                    nBytes = 4;
                }
                else if (chr >= 0xE0){
                    nBytes = 3;
                }
                else if (chr >= 0xC0){
                    nBytes = 2;
                }
                else{
                    return false;
                }
                nBytes--;
            }
        }
        else{
            //多字节符的非首字节,应为 10xxxxxx
            if ((chr & 0xC0) != 0x80){
                return false;
            }
            //减到为零为止
            nBytes--;
        }
    }
    //违返UTF8编码规则
    if (nBytes != 0) {
        return false;
    }
    if (bAllAscii){ //如果全部都是ASCII, 也是UTF8
        return true;
    }
    return true;
}
