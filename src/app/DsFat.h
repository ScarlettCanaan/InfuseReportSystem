#ifndef __DS_FAT_H
#define __DS_FAT_H

#define FAT_TRUE   1
#define FAT_FALSE  0

typedef unsigned long FAT_FILE_HANDLE;
typedef unsigned char FAT_BOOL;

#define FAT_FILE_HANDLE_NULL 0xFFFFFFFF
#define FAT_FILE_DESC_NUM  32

#define MAX_FILE_NAME_LEN  255  // 文件名的最大长度  
#define MIN_FILE_NAME_LEN  12   // 文件名最小长度

#define FAT_LIST_TYPE_READ      0x01	   //只读
#define FAT_LIST_TYPE_HIDE      0x02	   //隐藏
#define FAT_LIST_TYPE_SYS       0x04	   //系统
#define FAT_LIST_TYPE_LABLE     0x08	   //卷标
#define FAT_LIST_TYPE_DIR       0x10	   //子目录
#define FAT_LIST_TYPE_DOC       0x20	   //存档

#define FAT_FILE_OPEN_READ      0x01
#define FAT_FILE_OPEN_WRITE     0x02

typedef struct
{
    char * name;
    unsigned long nameLen;

    unsigned short year;
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    
    unsigned char type;
    
    unsigned long size;
}FAT_LIST_INFO;


//===========FAT文件系统的API编程接口=============================================================

extern FAT_BOOL fatInit(unsigned long flashSize);

// 得到FAT文件库的版本号
extern char * fatVersion(void);

// 对文件系统重新进行格式化
//参数 falshSize  单位:KB
extern void fatFormat(unsigned long flashSize);

// 获取文件系统的空间占用大小信息
//参数 allSize freeSize  单位:KB
extern void fatGetSize(unsigned long  *allSize,unsigned long  * freeSize);

// 得到当前目录的第一个项目（子目录或文件）信息
// 返回FALSE：为空目录，没有任何项目
// 返回TRUE：成功获取第1个项目。
extern FAT_BOOL fatGetDirectoryListFirst(FAT_LIST_INFO * listInfo);

// 得到当前目录的下一个项目（子目录或文件）信息
// 返回FALSE：已到结尾，没有下一个项目。
// 返回TRUE：成功获取下一个项目。
extern FAT_BOOL fatGetDirectoryListNext(FAT_LIST_INFO * listInfo);

// 在当前目录中创建子目录
// 返回FALSE：创建子目录失败。
// 返回TRUE：创建子目录成功。
extern FAT_BOOL fatMakeDirtory(char * subDirectoryName);

// 在当前目录中移除子目录
// 返回FALSE：移除子目录失败。
// 返回TRUE：移除子目录成功。
extern FAT_BOOL fatRemoveDirtory(char * subDirectoryName);

// 进入当前目录的子目录
// 返回FALSE：不存在该子目录。
// 返回TRUE：成功进入子目录。
extern FAT_BOOL fatEnterDirectory(char * subDirectoryName);

// 退回到根目录
extern void fatBackToRootDirectory(void);

// 在当前目录中创建文件
// 返回FALSE：创建文件失败。
// 返回TRUE：创建文件成功。
extern FAT_BOOL fatMakeFile(char * fileName);

// 在当前目录中移除文件
// 返回FALSE：移除文件失败。
// 返回TRUE：移除文件成功。
extern FAT_BOOL fatRemoveFile(char * fileName);

// 在当前目录中打开指定的文件
// 参数1：fileName 需要打开的文件名
// 参数2：openMode 打开文件的模式，读或写
// 返回打开文件的句柄
// 返回FAT_FILE_HANDLE_NULL：打开文件失败。 
FAT_FILE_HANDLE fatFileOpen(char * fileName, unsigned char openMode);

// 关闭文件。 
extern FAT_BOOL fatFileClose(FAT_FILE_HANDLE handle);

// 得到文件的长度
extern unsigned long fatFileGetLen(FAT_FILE_HANDLE handle);

// 从文件的当前位置读指定长度的内容
// 返回实际读到的内容长度（当文件当前位置到结尾的长度小于buffer空间时）
extern unsigned long fatFileRead(FAT_FILE_HANDLE handle, unsigned char * buffer, unsigned long size);

// 清除文件所有内容。
extern FAT_BOOL fatFileClear(FAT_FILE_HANDLE handle);

// 从文件的结尾追加指定长度的内容，
extern FAT_BOOL fatFileAppend(FAT_FILE_HANDLE handle, unsigned char * data, unsigned long len);

//==================FAT文件系统与物理存储器的底层接口===================================================================

// 对文件系统进行初始化，并确定文件系统能使用的FLASH空间的大小。
// flashSize最大容量1个G。
//参数flashSize 单位:KB
extern FAT_BOOL fatInit(unsigned long flashSize);

// 在FLASH的指定位置写入数据
extern void fatFlashWrite(unsigned long addr, const unsigned char * data, unsigned long len);

// 从FLASH的指定位置读出数据
extern void fatFlashRead(unsigned long addr, unsigned char * data, unsigned long len);

// 在FLASH的指定位置清除数据
extern void fatFlashClear(unsigned long addr, unsigned long len);

// FAT获取当前的时间信息
extern void fatGetDateTime(unsigned short * year, 
                           unsigned char * mon, 
                           unsigned char * day,
                           unsigned char * hour,
                           unsigned char * min,
                           unsigned char * sec);

// FAT调试输出信息
extern void fatDebugOut(char * str);


extern unsigned char * fatAllocMem(unsigned long size);

extern void fatFreeMem(unsigned char * mem);

#endif
