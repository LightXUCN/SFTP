#ifndef _TSC_SFTP_H_2016_12_15_
#define _TSC_SFTP_H_2016_12_15_

#include "tsc_hal.h"
#include "tsc_utils.h"

#define MAX_FILE_CNT	10000



typedef struct{
    uint32_t flags;		//���Ա�־λ����ӦΪΪ1ʱ��Ӧֵ����Ч
    uint64_t filesize;	//�ļ���С
    uint32_t uid, gid;	//����
    uint32_t permissions;	//�ļ����ͺ�Ȩ��
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

/* �ļ����� */
#define LIBSSH2_SFTP_S_IFMT         0170000     /* type of file mask */
#define LIBSSH2_SFTP_S_IFIFO        0010000     /* �����ܵ� */
#define LIBSSH2_SFTP_S_IFCHR        0020000     /* �ַ��豸 */
#define LIBSSH2_SFTP_S_IFDIR        0040000     /* Ŀ¼ */
#define LIBSSH2_SFTP_S_IFBLK        0060000     /* ���豸 */
#define LIBSSH2_SFTP_S_IFREG        0100000     /* ��ͨ�ļ� */
#define LIBSSH2_SFTP_S_IFLNK        0120000     /* �������� */
#define LIBSSH2_SFTP_S_IFSOCK       0140000     /* �׽��� */

/* Ȩ�� */
/* �����û���дȨ�� */
#define LIBSSH2_SFTP_S_IRWXU        0000700     /* ȫ��Ȩ�� */
#define LIBSSH2_SFTP_S_IRUSR        0000400     /* ֻ�� */
#define LIBSSH2_SFTP_S_IWUSR        0000200     /* ֻд */
#define LIBSSH2_SFTP_S_IXUSR        0000100     /* ������ */
/* ���û���дȨ�� */
#define LIBSSH2_SFTP_S_IRWXG        0000070     /* ȫ��Ȩ�� */
#define LIBSSH2_SFTP_S_IRGRP        0000040     /* ֻ�� */
#define LIBSSH2_SFTP_S_IWGRP        0000020     /* ֻд */
#define LIBSSH2_SFTP_S_IXGRP        0000010     /* ������ */
/* �����û�����Ȩ�� */
#define LIBSSH2_SFTP_S_IRWXO        0000007     /* ȫ��Ȩ�� */
#define LIBSSH2_SFTP_S_IROTH        0000004     /* ֻ�� */
#define LIBSSH2_SFTP_S_IWOTH        0000002     /* ֻд */
#define LIBSSH2_SFTP_S_IXOTH        0000001     /* ������ */

/*��·��ز���*/

/**************************************************
* �������� : tsc_sftp_client_connect
* �������� : ����SFTP������
* ��    �� :
			 user			�û���
			 password		����,�ݲ�֧���Զ���֤
			 host			�����IP
			 port			�����SSH�˿�
			 sftppath		Ĭ��·����������NULL
* ��    �� : 
* ��    �� : sftp���
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
void* tsc_sftp_client_create(const char* user, const char* passwd, 
	const char* host, int port,char* sftppath);

/**************************************************
* �������� : tsc_sftp_client_destpry
* �������� : �ͷ�sftp���
* ��    �� : handle	sftp���
* ��    �� : 
* ��    �� : 
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
void tsc_sftp_client_destory(void* handle);

/*�����Բ���*/
/**************************************************
* �������� : tsc_sftp_read_dir
* �������� : ��ȡĿ¼��Ϣ
* ��    �� : sftp_connect	sftp���
			 path			����ȡĿ¼·��
* ��    �� : dir_info		Ŀ¼��Ϣ
* ��    �� : 0�ɹ�
			 -1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_readdir(void* handle, const char* path,
	tsc_sftp_dir_info_t* dir_info);

/**************************************************
* �������� : tsc_sftp_realpath
* �������� : ��ѯ����·��
* ��    �� : sftp_connect	sftp���
			 path			���·��
			 max_len		���·���ַ�����
* ��    �� : realpath		����·��
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_realpath(void* handle, const char *path, 
	char* realpath, size_t max_len);

/**************************************************
* �������� : tsc_sftp_rm_file
* �������� : ɾ���ļ�
* ��    �� : sftp_connect	sftp���
			 path			��ɾ���ļ�·��
			 flag		
* ��    �� : 
* ��    �� : 0�ɹ� -1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_remove(void* handle, const char* path);
/**************************************************
* �������� : tsc_sftp_isdir
* �������� : �ж�Զ��·���Ƿ�ΪĿ¼
* ��    �� : handle			sftp���
			 path			���ж�Ŀ¼·��
* ��    �� : 
* ��    �� : 1Ŀ¼	0�ļ�	-1�ļ�������
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_isdir(void* handle, const char* path);

/**************************************************
* �������� : tsc_sftp_mk_dir
* �������� : ����Ŀ¼
* ��    �� : handle			sftp���
			 path			������Ŀ¼·��
			 mode			Ȩ��
* ��    �� : 
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_mkdir(void* handle, const char* dir, int mode);

/**************************************************
* �������� : tsc_sftp_rmdir
* �������� : ɾ��Ŀ¼
* ��    �� : sftp_connect	sftp���
			 path			��ɾ��Ŀ¼
* ��    �� : 
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_rmdir(void* handle, const char *path);

//ʹ�� tsc_sftp_remove

/**************************************************
* �������� : tsc_sftp_get_stat
* �������� : ��ȡ�ļ�����
* ��    �� : sftp_connect	sftp���
			 path			�ļ�·��
			 attr			�ļ�����
* ��    �� : 
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_get_attr(void* handle, const char *path, tsc_sftp_attr_t* attr);


/**************************************************
* �������� : tsc_sftp_set_stat
* �������� : �����ļ�����
* ��    �� : sftp_connect	sftp���
			 path			�ļ�·��
			 arrt			�ļ�����
* ��    �� : 
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_set_attr(void* handle, const char *path, tsc_sftp_attr_t* attr);

/**************************************************
* �������� : tsc_sftp_rename
* �������� : �������ļ�
* ��    �� : sftp_connect		sftp���
			 old_path			���������ļ�·��
			 new_path			���ļ�·��
* ��    �� : 
* ��    �� : 0�ɹ�	-1ʧ��
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_rename(void* handle, const char* old_path, 
	const char* new_path);


/**************************************************
* �������� : tsc_sftp_download
* �������� : �����ļ�
* ��    �� : sftp_connect		sftp���
			 remote_path		�������ļ�·��
			 local_path			�����ļ�·��
* ��    �� : 
* ��    �� : �����ļ���С
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
 **************************************************/
int tsc_sftp_download(void* handle, const char *remote_path, 
	const char *local_path);


/**************************************************
* �������� : tsc_sftp_upload
* �������� : �ϴ��ļ�
* ��    �� : sftp_connect		sftp���
			 local_path			���ϴ��ļ�·��
			 remote_path		����·��
* ��    �� : 
* ��    �� : �ϴ��ļ���С
* ��    �� : ������
* �������� : 2016/12/08
* ����˵�� : 
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
