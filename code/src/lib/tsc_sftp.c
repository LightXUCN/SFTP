#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "libssh2.h"
#include "libssh2_sftp.h"

#include "tsc_sftp.h"

const char *keyfile1="~/.ssh/id_rsa.pub";
const char *keyfile2="~/.ssh/id_rsa";

typedef LIBSSH2_SESSION ssh2_session_t;
typedef LIBSSH2_SFTP sftp_session_t;
typedef LIBSSH2_SFTP_HANDLE sftp_handle_t;

typedef struct{
	int socket;
	ssh2_session_t* ssh2_session;
	sftp_session_t* sftp_session;
	sftp_handle_t* sftp_handle;
}tsc_sftp_handle_t;

enum STR_LEN{
	STR_LEN_16 = 16,
	STR_LEN_32 = 32,
	STR_LEN_64 = 64,
	STR_LEN_128 = 128,
	STR_LEN_256 = 256,
	STR_LEN_512 = 512,
	STR_LEN_1024 = 1024,
	STR_LEN_2048 = 2048,
	STR_LEN_3072 = 3072,
	STR_LEN_4096 = 4096,
	STR_LEN_8192 = 8192,
};

#define MAX_REWRITE_CNT 10


static void copy_attr(LIBSSH2_SFTP_ATTRIBUTES* _attr , tsc_sftp_attr_t* attr)
{
	_attr->atime = attr->atime;
	_attr->filesize= attr->filesize;
	_attr->uid = attr->uid;
	_attr->gid = attr->gid;
	_attr->permissions= attr->permissions;
	_attr->mtime= attr->mtime;
	_attr->flags= attr->flags;
}
void* tsc_sftp_client_create(const char* user, 
	const char* passwd, const char* host, int port, char* sftppath)
{
	uint32_t hostaddr;
	int i, ret;
	int auth_pw = 0;
	const char *fingerprint;
	char *userauthlist;
	struct sockaddr_in socketaddr;	
	tsc_sftp_handle_t* sftp_handle  = malloc(sizeof(tsc_sftp_handle_t));
	if(sftp_handle ==NULL)
		return NULL;
	
	hostaddr = inet_addr(host);

	//初始化ssh2库
	ret = libssh2_init (0);
	if (ret != 0) 
	{
		tsc_error("SFTP : libssh2 initialization failed (%d)\n", ret);
		return NULL;
	}

	//建立连接
	sftp_handle->socket= socket(AF_INET, SOCK_STREAM, 0);

	socketaddr.sin_family = AF_INET;
	socketaddr.sin_port = htons(port);
	socketaddr.sin_addr.s_addr = hostaddr;
	if (connect(sftp_handle->socket, (struct sockaddr*)(&socketaddr),
			sizeof(struct sockaddr_in)) != 0) 
	{
		tsc_error("SFTP : failed to connect!\n");
		return NULL;
	}
	
	//创建SSH2会话
	sftp_handle->ssh2_session = libssh2_session_init();
	if(!sftp_handle->ssh2_session)
	{
		tsc_error("SFTP : failed to create session!\n");
		return NULL;
	}
	
    /* 设置非阻塞 */
	libssh2_session_set_blocking(sftp_handle->ssh2_session, 1);
	//将socket与会话绑定
	ret = libssh2_session_handshake(sftp_handle->ssh2_session, sftp_handle->socket);
	if(ret) 
	{
		tsc_error("SFTP : Failure establishing SSH session: %d\n", ret);
		return NULL;
	}

    fingerprint = libssh2_hostkey_hash(sftp_handle->ssh2_session, 
		LIBSSH2_HOSTKEY_HASH_SHA1);
    tsc_info("Fingerprint: ");
    for(i = 0; i < 20; i++) {
        tsc_info("%02X ", (unsigned char)fingerprint[i]);
    }
	printf("\n");

	userauthlist = libssh2_userauth_list(sftp_handle->ssh2_session, user, strlen(user));
	tsc_info("Authentication methods: %s\n", userauthlist);
	if(strstr(userauthlist, "password") != NULL) 
	{
		auth_pw |= 1;
	}
	//预设用户名密码鉴权
	if (auth_pw & 1) 
	{
		tsc_info("SFTP : Authentication by password.\n");
		if (libssh2_userauth_password(sftp_handle->ssh2_session, user, passwd)) 
		{
			tsc_info("SFTP : Authentication by password failed.\n");
			
			libssh2_session_disconnect(sftp_handle->ssh2_session, "SFTP auth fail.");
			libssh2_session_free(sftp_handle->ssh2_session);
			close(sftp_handle->socket);
			libssh2_exit();
			return NULL;
		}
		else
		{
			tsc_info("SFTP : Authentication by password success.\n");
		}
		

	} 
	#if 0
	else if (auth_pw & 4) 
	{
        //通过公钥鉴权
		if (libssh2_userauth_publickey_fromfile(sftp_handle->ssh2_session, user, 
				keyfile1, keyfile2, passwd)) 
		{
			tsc_info("SFTP : Authentication by public key failed!\n");
			libssh2_session_disconnect(sftp_handle->ssh2_session, "SFTP auth fail.");
			libssh2_session_free(sftp_handle->ssh2_session);
			close(sftp_handle->socket);
			libssh2_exit();
			return NULL;
        } 
		else 
		{
            tsc_info("SFTP : Authentication by public key succeeded.\n");
        }
    } 
#endif /* if 0.2016-12-16 9:37:12 xianzhang.xu*/
	else 
	{
		tsc_error("SFTP : No supported authentication methods found!\n");
		libssh2_session_disconnect(sftp_handle->ssh2_session, "SFTP auth fail.");
		libssh2_session_free(sftp_handle->ssh2_session);
		close(sftp_handle->socket);
		libssh2_exit();
		return NULL;
	}

	//初始化sftp
    tsc_info("SFTP : init sftp!\n");
    sftp_handle->sftp_session = libssh2_sftp_init(sftp_handle->ssh2_session);

    if (sftp_handle->sftp_session == NULL) 
	{
		tsc_info("SFTP : Unable to init SFTP session\n");
		libssh2_session_disconnect(sftp_handle->ssh2_session, "SFTP init fail.");
		libssh2_session_free(sftp_handle->ssh2_session);
		close(sftp_handle->socket);
		libssh2_exit();
		return NULL;
	}
	//if(sftppath[0]!='\0')
	if(sftppath != NULL)
	{
	    sftp_handle =libssh2_sftp_open(sftp_handle->sftp_session, sftppath, LIBSSH2_FXF_READ, 0);

	    if(sftp_handle == NULL) 
		{
	        tsc_info("SFTP : Unable to open file with SFTP: %ld\n",
	                libssh2_sftp_last_error(sftp_handle->sftp_session));
			libssh2_session_disconnect(sftp_handle->ssh2_session, "Open file fail.");
			libssh2_session_free(sftp_handle->ssh2_session);
			close(sftp_handle->socket);
			libssh2_exit();
			return NULL;
		}
	}
	tsc_info("SFTP : sftp init success!\n");
    return (void*)sftp_handle;	
}

void tsc_sftp_client_destory(void* handle)
{
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;

	//释放sftp句柄
	libssh2_sftp_close(sftp_handle->sftp_handle);
	//释放SFTP会话
	libssh2_sftp_shutdown(sftp_handle->sftp_session);
	//断开SSH2会话
	libssh2_session_disconnect(sftp_handle->ssh2_session, "Normal Shutdown.");
	//释放SSH2会话
	libssh2_session_free(sftp_handle->ssh2_session);
	//关闭连接
	close(sftp_handle->socket);
	libssh2_exit();
	free(handle);
	return;
}

int tsc_sftp_download(void* handle, const char *remote_path, 
	const char *local_path )
{
	int ret = 0,download_len = 0; 
	size_t len = 0;
	char buf[STR_LEN_1024 * 512] = {0};
	FILE* local_fp;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle; 
	sftp_handle_t* remote_fp;
	
	//创建本地文件
	local_fp = fopen(local_path , "wb");
	if(local_fp == NULL)
	{
		return -1;
	}
	//打开远端文件
	remote_fp = libssh2_sftp_open(sftp_handle->sftp_session, remote_path, 
								LIBSSH2_FXF_READ, 0);
	if(remote_fp == NULL)
	{
		return -1;
	}
	//tsc_info("SFTP :download %s to %s!\n" , remote_path , local_path);
	//开始下载
	while (1)
	{
		ret = libssh2_sftp_read(remote_fp, buf, sizeof(buf));
 
		if (ret > 0) 
		{
			len = fwrite(buf, 1, ret, local_fp);
			download_len += ret;
		} 
		else 
		{
			break;
        }
    }
	fclose(local_fp);
	libssh2_sftp_close(remote_fp);
	
	return download_len;
}


int tsc_sftp_upload(void* handle, const char *remote_path, 
	const char *local_path)
{
	int ret = 0;
	size_t filesize = 0;
	size_t nread = 0, upload_size = 0;
	char* ptr;
	char buf[STR_LEN_1024 * 512] = {0};
	FILE* local_fp;
	sftp_handle_t* remote_fp;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle; 

	local_fp = fopen(local_path , "rb");
	if(local_fp == NULL)
	{
		return -1;
	}
	
	remote_fp = libssh2_sftp_open(sftp_handle->sftp_session, remote_path, 
					LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
					LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
					LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
	if(remote_fp == NULL)
	{
		return -1;
	}
	//取待上传文件整个size.
	fseek(local_fp, 0, SEEK_END);
	filesize = ftell(local_fp);
	fseek(local_fp, 0, SEEK_SET);
	
	//tsc_info("SFTP :upload %s to %s!\n" , local_path , remote_path);
	do{
		nread = fread(buf, 1, sizeof(buf), local_fp);
		if (nread <= 0) {
			break;
		}
		ptr = buf;
		do {
			// 向服务器写数据，直到数据写完毕
			ret = libssh2_sftp_write(remote_fp, ptr, nread);
			if (ret < 0)
				break;
			ptr += ret; upload_size += ret;
			nread -= ret;

		} while (nread);

	} while (ret > 0);

	libssh2_sftp_close(remote_fp);
	return upload_size;
}

int tsc_sftp_rename(void* handle, const char* old_path, 
	const char* new_path)
{
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;

	ret = libssh2_sftp_rename(sftp_handle->sftp_session, old_path, new_path);
	return ret;
}

int tsc_sftp_isdir(void* handle, const char* path)
{
	LIBSSH2_SFTP_ATTRIBUTES attr;
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;
	sftp_handle_t* _handle = NULL; 

	ret =  libssh2_sftp_stat_ex(sftp_handle->sftp_session, path, strlen(path), 
				LIBSSH2_SFTP_STAT, &attr);
	if(ret < 0)
		return -1;

	if(LIBSSH2_SFTP_S_ISDIR(attr.permissions))
		return 1;
	else
		return 0;
}

int tsc_sftp_rmdir(void* handle, const char *path)
{
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;
	
	ret = libssh2_sftp_rmdir(sftp_handle->sftp_session, path);
	return ret;
}

int tsc_sftp_get_attr(void* handle, const char *path, tsc_sftp_attr_t* attr)
{
	
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;
	LIBSSH2_SFTP_ATTRIBUTES _attr;
	ret =  libssh2_sftp_stat_ex(sftp_handle->sftp_session, path, strlen(path), 
				LIBSSH2_SFTP_STAT, &_attr);
	copy_attr(attr, &_attr);
	return ret;
}

int tsc_sftp_set_attr(void* handle, const char *path , tsc_sftp_attr_t* attr)
{
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;
	LIBSSH2_SFTP_ATTRIBUTES _attr;
	copy_attr(&_attr, attr);
	ret = libssh2_sftp_setstat(sftp_handle->sftp_session, path, &_attr);
	
	return ret;
}

int tsc_sftp_mkdir(void* handle, const char* dir, int mode)
{
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;

	ret = libssh2_sftp_mkdir(sftp_handle->sftp_session, dir, mode);
		
	return ret;
}
int tsc_sftp_remove(void* handle, const char* path)
{
	
	return 0;
}

int tsc_sftp_realpath(void* handle, const char *path, 
	char* realpath, size_t max_len)
{
	int ret = 0;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;

	ret = libssh2_sftp_realpath(sftp_handle->sftp_session, path,realpath, max_len);
		
	return ret;
}

int tsc_sftp_readdir(void* handle, const char* path,tsc_sftp_dir_info_t* dir_info)
{
	int ret = 0;
	int cnt = 0;
	int len = 0;
	int flag_sprintf = 0;
	char name[512];
	char longentry[512];
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	tsc_sftp_handle_t* sftp_handle = (tsc_sftp_handle_t*)handle;
	memset(&attrs , 0 , sizeof(tsc_sftp_attr_t));
	memset(name , 0 , sizeof(name));
	memset(longentry , 0 , sizeof(longentry));
	
	if(path[strlen(path)-1]=='/')
		flag_sprintf = 1;
	sftp_handle_t* _handle = libssh2_sftp_opendir(sftp_handle->sftp_session, path);
	
	if (!_handle)
	{
		tsc_error("Unable to open dir with SFTP\n");
		return -1;
    }	
	while((ret = libssh2_sftp_readdir_ex(_handle, name, sizeof(name), longentry, 
											sizeof(longentry),&attrs)) > 0)
	{
		dir_info->info[cnt].attr.flags = attrs.flags;
		dir_info->info[cnt].attr.filesize= attrs.filesize;
		dir_info->info[cnt].attr.uid = attrs.uid;
		dir_info->info[cnt].attr.gid = attrs.gid;
		dir_info->info[cnt].attr.permissions= attrs.permissions;
		dir_info->info[cnt].attr.atime= attrs.atime;
		dir_info->info[cnt].attr.mtime= attrs.mtime;

		strncpy(dir_info->info[cnt].filename, name, ret);
		if(flag_sprintf)
		{
			len = sprintf(dir_info->info[cnt].filepath, "%s%s", path, name);
			dir_info->info[cnt].filepath[len] = 0;
		}
		else
		{
			len = sprintf(dir_info->info[cnt].filepath, "%s/%s", path, name);
			dir_info->info[cnt].filepath[len] = 0;
		}
		cnt++;
    }
	dir_info->file_cnt = cnt;
	libssh2_sftp_closedir(_handle);
	return cnt;
}


