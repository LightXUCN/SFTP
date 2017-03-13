#ifndef _TSC_SFTP_H_2016_12_15_
#define _TSC_SFTP_H_2016_12_15_

#include "tsc_hal.h"
#include "tsc_utils.h"

#define MAX_FILE_CNT	10000



typedef struct{
    uint32_t flags;		//属性标志位，相应为为1时对应值才有效
    uint64_t filesize;	//文件大小
    uint32_t uid, gid;	//归属
    uint32_t permissions;	//文件类型和权限
    uint32_t atime, mtime;	
}tsc_sftp_attr_t;

typedef struct{
	char filename[128];
	char filepath[256];
	tsc_sftp_attr_t attr;
}tsc_sftp_file_info_t;

typedef struct{
	int file_cnt;
	tsc_sftp_file_info_t info[MAX_FILE_CNT]; 
}tsc_sftp_dir_info_t;

/* 文件类型 */
#define LIBSSH2_SFTP_S_IFMT         0170000     /* type of file mask */
#define LIBSSH2_SFTP_S_IFIFO        0010000     /* 有名管道 */
#define LIBSSH2_SFTP_S_IFCHR        0020000     /* 字符设备 */
#define LIBSSH2_SFTP_S_IFDIR        0040000     /* 目录 */
#define LIBSSH2_SFTP_S_IFBLK        0060000     /* 块设备 */
#define LIBSSH2_SFTP_S_IFREG        0100000     /* 普通文件 */
#define LIBSSH2_SFTP_S_IFLNK        0120000     /* 符号链接 */
#define LIBSSH2_SFTP_S_IFSOCK       0140000     /* 套接字 */

/* 权限 */
/* 归属用户读写权限 */
#define LIBSSH2_SFTP_S_IRWXU        0000700     /* 全部权限 */
#define LIBSSH2_SFTP_S_IRUSR        0000400     /* 只读 */
#define LIBSSH2_SFTP_S_IWUSR        0000200     /* 只写 */
#define LIBSSH2_SFTP_S_IXUSR        0000100     /* 可运行 */
/* 组用户读写权限 */
#define LIBSSH2_SFTP_S_IRWXG        0000070     /* 全部权限 */
#define LIBSSH2_SFTP_S_IRGRP        0000040     /* 只读 */
#define LIBSSH2_SFTP_S_IWGRP        0000020     /* 只写 */
#define LIBSSH2_SFTP_S_IXGRP        0000010     /* 可运行 */
/* 其他用户归属权限 */
#define LIBSSH2_SFTP_S_IRWXO        0000007     /* 全部权限 */
#define LIBSSH2_SFTP_S_IROTH        0000004     /* 只读 */
#define LIBSSH2_SFTP_S_IWOTH        0000002     /* 只写 */
#define LIBSSH2_SFTP_S_IXOTH        0000001     /* 可运行 */

/*链路相关操作*/

/**************************************************
* 函数名称 : tsc_sftp_client_connect
* 函数功能 : 连接SFTP服务器
* 输    入 :
			 user			用户名
			 password		密码,暂不支持自动验证
			 host			服务端IP
			 port			服务端SSH端口
			 sftppath		默认路径，可以填NULL
* 输    出 : 
* 返    回 : sftp句柄
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
void* tsc_sftp_client_create(const char* user, const char* passwd, 
	const char* host, int port,char* sftppath);

/**************************************************
* 函数名称 : tsc_sftp_client_destpry
* 函数功能 : 释放sftp句柄
* 输    入 : handle	sftp句柄
* 输    出 : 
* 返    回 : 
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
void tsc_sftp_client_destory(void* handle);

/*功能性操作*/
/**************************************************
* 函数名称 : tsc_sftp_read_dir
* 函数功能 : 读取目录信息
* 输    入 : sftp_connect	sftp句柄
			 path			待读取目录路径
* 输    出 : dir_info		目录信息
* 返    回 : 0成功
			 -1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_readdir(void* handle, const char* path,
	tsc_sftp_dir_info_t* dir_info);

/**************************************************
* 函数名称 : tsc_sftp_realpath
* 函数功能 : 查询绝对路径
* 输    入 : sftp_connect	sftp句柄
			 path			相对路径
			 max_len		输出路径字符长度
* 输    出 : realpath		绝对路径
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_realpath(void* handle, const char *path, 
	char* realpath, size_t max_len);

/**************************************************
* 函数名称 : tsc_sftp_rm_file
* 函数功能 : 删除文件
* 输    入 : sftp_connect	sftp句柄
			 path			待删除文件路径
			 flag		
* 输    出 : 
* 返    回 : 0成功 -1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_remove(void* handle, const char* path);
/**************************************************
* 函数名称 : tsc_sftp_isdir
* 函数功能 : 判断远端路径是否为目录
* 输    入 : handle			sftp句柄
			 path			待判断目录路径
* 输    出 : 
* 返    回 : 1目录	0文件	-1文件不存在
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_isdir(void* handle, const char* path);

/**************************************************
* 函数名称 : tsc_sftp_mk_dir
* 函数功能 : 创建目录
* 输    入 : handle			sftp句柄
			 path			待创建目录路径
			 mode			权限
* 输    出 : 
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_mkdir(void* handle, const char* dir, int mode);

/**************************************************
* 函数名称 : tsc_sftp_rmdir
* 函数功能 : 删除目录
* 输    入 : sftp_connect	sftp句柄
			 path			待删除目录
* 输    出 : 
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_rmdir(void* handle, const char *path);

//使用 tsc_sftp_remove

/**************************************************
* 函数名称 : tsc_sftp_get_stat
* 函数功能 : 获取文件属性
* 输    入 : sftp_connect	sftp句柄
			 path			文件路径
			 attr			文件属性
* 输    出 : 
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_get_attr(void* handle, const char *path, tsc_sftp_attr_t* attr);


/**************************************************
* 函数名称 : tsc_sftp_set_stat
* 函数功能 : 设置文件属性
* 输    入 : sftp_connect	sftp句柄
			 path			文件路径
			 arrt			文件属性
* 输    出 : 
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_set_attr(void* handle, const char *path, tsc_sftp_attr_t* attr);

/**************************************************
* 函数名称 : tsc_sftp_rename
* 函数功能 : 重命名文件
* 输    入 : sftp_connect		sftp句柄
			 old_path			待重命名文件路径
			 new_path			新文件路径
* 输    出 : 
* 返    回 : 0成功	-1失败
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_rename(void* handle, const char* old_path, 
	const char* new_path);


/**************************************************
* 函数名称 : tsc_sftp_download
* 函数功能 : 下载文件
* 输    入 : sftp_connect		sftp句柄
			 remote_path		待下载文件路径
			 local_path			保存文件路径
* 输    出 : 
* 返    回 : 下载文件大小
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_download(void* handle, const char *remote_path, 
	const char *local_path);


/**************************************************
* 函数名称 : tsc_sftp_upload
* 函数功能 : 上传文件
* 输    入 : sftp_connect		sftp句柄
			 local_path			待上传文件路径
			 remote_path		保存路径
* 输    出 : 
* 返    回 : 上传文件大小
* 作    者 : 徐宪章
* 创建日期 : 2016/12/08
* 其他说明 : 
 **************************************************/
int tsc_sftp_upload(void* handle, const char *remote_path,
	const char *local_path);


#if 0
int main(int argc , char** argv)
{
	int ret = 0;
	int i = 0;
	void* handle;
	tsc_sftp_dir_info_t dir_info;
	char* path = "/home/xxz/libssh.a";
	memset(&dir_info , 0 ,sizeof(tsc_sftp_dir_info_t));
	
	tsc_log_init(0xff,0,NULL,NULL, 10, 1);
	handle = tsc_sftp_client_create("root", "tescomm", "10.95.3.153", 22, NULL);
	if(!handle)
	{
		tsc_error("client create fail!");
		return -1;
	}
	tsc_info("client created!");
	
	ret = tsc_sftp_isdir(handle, path);
	if(ret == 1)
	{
		printf("%s is dir!\n",path);

		ret = tsc_sftp_readdir(handle, path, &dir_info);
		if(ret < 0)
		{
			tsc_error("readdir fail!");		
			tsc_sftp_client_destory(handle);
			return -1;
		}
		for(i = 0;i<ret;i++)
		{
			tsc_info("file         : %s" , dir_info.info[i].filename);
			tsc_info("path         : %s" , dir_info.info[i].filepath);
			tsc_info("filesize     : %u" , dir_info.info[i].attr.filesize);
			tsc_info("permissions  : %u" , dir_info.info[i].attr.permissions);
			tsc_info("uid          : %u" , dir_info.info[i].attr.uid);
			tsc_info("gid          : %u" , dir_info.info[i].attr.gid);
			tsc_info("flags        : %u" , dir_info.info[i].attr.flags);
			
		}
	}
	else if(ret == 0)
	{
		printf("%s is file!\n",path);
		ret = tsc_sftp_download(handle, path, "/home/xxz/123123.a");
		if(ret <0)
			tsc_info("down load fail!");
		else	
			tsc_info("download file_size : %u\n",ret);

		ret = tsc_sftp_upload(handle,"/home/xxz/787878.a", "/home/xxz/123123.a");
		if(ret < 0)
			tsc_info("up load fail!");
		else
			tsc_info("upload file_size : %u\n",ret);
	}
	else
	{
		printf("error!\n");
	}


	tsc_sftp_client_destory(handle);
	
	return 0;
}
#endif /* if 0.2016-12-20 15:29:10 xianzhang.xu*/

#endif   /* ifndef _TSC_SFTP_H_2016_12_15_*/
