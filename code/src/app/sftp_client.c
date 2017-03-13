
#include "tsc_hal.h"
#include "tsc_utils.h"
#include "tsc_sftp.h"
#include "tsc_ini_file.h"
#include "sftp_client_common.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h> 
#include <dirent.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>



tsc_sftp_cfg_t sftp_cfg;
char* tsc_split = ",";


void sigint_deal(int signo)
{
	pid_t pid;
	int waiter = 0;
	
	pid = waitpid(-1, &waiter, WNOHANG);
	tsc_info("ftp client recv stop command");
	sftp_cfg.run=0;
}

void tsc_format_dir(char* path)
{
	//tsc_debug("path=%s\n",path);
	size_t len=strlen(path);

	if(len==0)
	{
		strcpy(path,"./");
		return;
	}
	if(path[len-1]!='/')
		strcat(path,"/");
}

int tsc_get_split_ptr(const char* in_str, char* split, 
		char** field_array, uint32_t field_max_index, uint32_t field_size)
{
	uint32_t pos = 0;
	uint32_t ptr_len;
	int split_len;
	char* ptr;
	char* ptr_end;
	
	split_len = strlen(split);
	ptr = in_str;
	while(ptr && pos<field_max_index)
	{
		ptr_end = strstr(ptr,split);

		if(ptr_end)
		{
			ptr_len = ptr_end -ptr;
			field_array[pos] = malloc(field_size);
			if(field_array[pos] ==NULL)
				return -1;
			strncpy(field_array[pos], ptr, ptr_len);
			field_array[pos][ptr_len]=0;
			ptr_end[0]=0;
			ptr = ptr_end+split_len;
			pos++;
		}
		else
		{
			ptr_len=strlen(ptr);
			field_array[pos] = malloc(field_size);
			if(field_array[pos] ==NULL)
				return -1;
			strncpy(field_array[pos], ptr, ptr_len);
			field_array[pos][ptr_len]=0;
			pos++;
			break;
		}
	}
	
	return pos;
}

static int tsc_sftp_ini_init(char* ini_path)
{
	void* dict = NULL; 
	char client_path[STR_LEN_1024] = {0};
	char server_path[STR_LEN_1024] = {0};
	char upload_suffix[STR_LEN_512]={0};
	char download_suffix[STR_LEN_512]={0};
	
	dict = tsc_ini_init(ini_path, 1024, 1024*2048);
	if(dict == NULL)
	{
		return -1;
	}
	sftp_cfg.direction = tsc_ini_getint(dict, "main:direction", DIRECTION_NONE);
	
	//日志配置
	sftp_cfg.log_mode = tsc_ini_getint(dict, "log:log_mode", 0);
	sftp_cfg.level_mask = tsc_ini_getint(dict, "log:level_mask", 0xFF);
	sftp_cfg.log_file_size = tsc_ini_getint(dict, "log:log_file_size", 10);
	sftp_cfg.log_file_num = tsc_ini_getint(dict, "log:log_file_num", 1000);
	tsc_ini_getstr(dict, "log:log_dir", sftp_cfg.log_dir, STR_LEN_128, "");
	tsc_ini_getstr(dict, "log:log_prefix", sftp_cfg.log_prefix, STR_LEN_64, "");
	
	sftp_cfg.back_ground = tsc_ini_getint(dict, "main:back_ground", YES);
	
	tsc_ini_getstr(dict, "main:client_path", client_path, STR_LEN_1024, "");
	tsc_ini_getstr(dict, "main:server_path", server_path, STR_LEN_1024, "");

	sftp_cfg.client_path_cnt= tsc_get_split_ptr(client_path, 
											tsc_split, 
											sftp_cfg.client_path, 
											MAX_PATH_CNT,
											STR_LEN_64); 

  	sftp_cfg.server_path_cnt= tsc_get_split_ptr(server_path, 
										 tsc_split, 
										 sftp_cfg.server_path, 
										 MAX_PATH_CNT,
										 STR_LEN_64); 

	//鉴权信息
	tsc_ini_getstr(dict, "auth:server_ip", sftp_cfg.server_ip, STR_LEN_64, "");
	sftp_cfg.server_port = tsc_ini_getint(dict, "auth:server_port", 1);
	tsc_ini_getstr(dict, "auth:user_name", sftp_cfg.user_name, STR_LEN_64, "");
	tsc_ini_getstr(dict, "auth:password", sftp_cfg.password, STR_LEN_64, "");

	//上传配置
	tsc_ini_getstr(dict, "upload:upload_suffix",upload_suffix, STR_LEN_512, "");
	sftp_cfg.upload_suffix_cnt = tsc_get_split_ptr(upload_suffix, 
											   tsc_split, 
											   sftp_cfg.upload_suffix,
											   MAX_SUFFIX_CNT,
											   STR_LEN_16); 
	
	sftp_cfg.upload_confirm = tsc_ini_getint(dict, "upload:upload_confirm", YES);
	tsc_ini_getstr(dict, "upload:upload_confirm_suffix", sftp_cfg.upload_confirm_suffix, STR_LEN_16, "");
	sftp_cfg.upload_complete = tsc_ini_getint(dict, "upload:upload_complete", YES);
	tsc_ini_getstr(dict, "upload:upload_complete_suffix", sftp_cfg.upload_complete_suffix, STR_LEN_16, "");
	sftp_cfg.upload_remove = tsc_ini_getint(dict, "upload:upload_remove", YES);
	tsc_ini_getstr(dict, "upload:upload_complete_dir", sftp_cfg.upload_complete_dir, STR_LEN_64, "");
	
	//下载配置
	tsc_ini_getstr(dict, "download:download_suffix",download_suffix, STR_LEN_512, "");
	sftp_cfg.download_suffix_cnt = tsc_get_split_ptr(download_suffix, 
												 tsc_split, 
												 sftp_cfg.download_suffix,
												 MAX_SUFFIX_CNT,
												 STR_LEN_16); 
	
	sftp_cfg.download_confirm = tsc_ini_getint(dict, "download:download_confirm", YES);
	tsc_ini_getstr(dict, "download:download_confirm_suffix", sftp_cfg.download_confirm_suffix, STR_LEN_16, "");
	sftp_cfg.download_complete = tsc_ini_getint(dict, "download:download_complete", YES);
	tsc_ini_getstr(dict, "download:download_complete_suffix", sftp_cfg.download_complete_suffix, STR_LEN_16, "");
	sftp_cfg.download_remove = tsc_ini_getint(dict, "download:download_remove", YES);
	tsc_ini_getstr(dict, "download:download_complete_dir", sftp_cfg.download_complete_dir, STR_LEN_64, "");	
	return 0;
}

static int tsc_sftp_init(char* ini_path)
{
	int ret = 0;
	ret = tsc_sftp_ini_init(ini_path);
	if(ret < 0)
	{
		return -1;
	}
		
	ret = tsc_log_init(sftp_cfg.level_mask,
					   sftp_cfg.log_mode, 
					   sftp_cfg.log_dir, 
					   sftp_cfg.log_prefix, 
					   sftp_cfg.log_file_size, 
					   sftp_cfg.log_file_num);
	if(ret < 0)
	{
		return -1;
	}
	
	return 0;
}

static void tsc_usage()
{
	printf("[path] [config]\n");
	return;
}

static char* direction_val(uint8_t direction)
{
	switch(direction)
	{
	case DIRECTION_NONE:
		return (char*)"none";
	case DIRECTION_UPLOAD:
		return (char*)"upload";
	case DIRECTION_DOWNLOAD:
		return (char*)"download";
	default:
		return (char*)"unknown";
	}
}

static char* bool_val(uint8_t recursive)
{
	switch(recursive)
	{
	case NO:
		return (char*)"no";
	case YES:
		return (char*)"yes";
	default:
		return (char*)"unknown";
	}
}

static char* confirm_val(uint8_t confirm)
{
	switch(confirm)
	{
	case CONFIRM_NONE:
		return (char*)"none";
	case CONFIRM_AUXILIARY:
		return (char*)"auxiliary";
	case CONFIRM_TEMP:
		return (char*)"temp";
	default:
		return (char*)"unknown";
	}
}

static char* tsc_log_mask_val(uint16_t mask)
{
	switch(mask)
	{
	case TSC_LOG_NONE:
		return (char*)"none";
	case TSC_LOG_EMERG:
		return (char*)"System is unusable";
	case TSC_LOG_ALERT:
		return (char*)"Action must be taken immediately";
	case TSC_LOG_CRIT:
		return (char*)"Critical conditions";
	case TSC_LOG_ERROR:
		return (char*)"Error conditions";
	case TSC_LOG_WARNING:
		return (char*)"Warning conditions";
	case TSC_LOG_NOTICE:
		return (char*)"Normal but significant condition";
	case TSC_LOG_INFO:
		return (char*)"Infomational";
	case TSC_LOG_DEBUG:
		return (char*)"Debug-level message";
	case TSC_LOG_ALL:
		return (char*)"All log";
	default:
		return (char*)"unknown";
	}
}

static char* tsc_log_mode_val(uint16_t mode)
{
	switch(mode)
	{
	case LOG_MODE_TERMINAL:
		return (char*)"Terminal";
	case LOG_MODE_FILE:
		return (char*)"File";
	default:
		return (char*)"Unknown";
	}
}

static int tsc_create_local_dir(const char* file_path)
{
	uint32_t i;
		char tmp_file_path[256];
		uint32_t len=strlen(file_path);
	
		if(len+1 > 256)
			return -1;
		
		memcpy(tmp_file_path, file_path, len+1);
		if(tmp_file_path[strlen(tmp_file_path)-1]!='/')
		{
			strcat(tmp_file_path, "/");
		}
		for(i=0; i<=len; i++)
		{
			if(tmp_file_path[i]=='/' && i!=0)
			{
				tmp_file_path[i] = '\0';
				if(access(tmp_file_path, F_OK) != 0)
				{
					if(mkdir(tmp_file_path, 0755) == -1)
					{
						tsc_error("SFTP UPLOAD : create dir %s fail\n", tmp_file_path);
						return -1;
					}
				}
				
				tmp_file_path[i] = '/';
			}
		}
	
		return 0;
}

static int is_dir(const char *path)
{
	struct stat sb;

	if (stat(path, &sb) == -1)
		return(0);

	return(S_ISDIR(sb.st_mode));
}

/*SFTP上传*/
int tsc_sftp_upload_start(void* handle)
{
	uint8_t i,j,k;
	uint8_t is_first = 1;
	uint8_t add_flag = 0;
	size_t len = 0;
	int ret = 0;
	char path1[256];
	char path2[256];
	char path_tmp[256];
	char suffix[STR_LEN_32];
	char confirm_tmp[256];
	char* suffix_tmp;
	tsc_list_t  head;
	tsc_list_t* list;
	tsc_file_list_t* file_in;
	tsc_file_list_t* file_out;
	DIR* dir_info = NULL;
	struct dirent* read_dir = NULL;

	tsc_info("==================== begin upload ====================");
	while(sftp_cfg.run)
	{
		tsc_list_head_init(&head);
		
		//创建客户端及服务端目录
		for(i=0;sftp_cfg.run&&i<sftp_cfg.client_path_cnt && is_first == 1;i++)
		{	
			//创建目录
			if(sftp_cfg.upload_remove==NO &&
				sftp_cfg.upload_complete_dir!=NULL &&
				sftp_cfg.upload_complete_dir[0]!='\0')
			{
				sprintf(path1,"%s/%s",sftp_cfg.client_path[i],
					sftp_cfg.upload_complete_dir);
				
				if(tsc_create_local_dir(path1) != 0)
				{
					tsc_error("SFTP UPLOAD : create local dir %s failed!",
																	 path1);
				}
			}
			
			tsc_sftp_mkdir(handle,sftp_cfg.server_path[i], 
							LIBSSH2_SFTP_S_IRWXU|
							LIBSSH2_SFTP_S_IRWXG|
							LIBSSH2_SFTP_S_IRWXO);
			
		}
		is_first = 0;
		//遍历所有客户端路径,将符合条件的文件加入到文件链表
		for(i =0;i<sftp_cfg.client_path_cnt;i++)
		{			
			//读取文件列
			dir_info=opendir(sftp_cfg.client_path[i]);
			if(dir_info==NULL)
			{
				tsc_warn("SFTP UPLOAD : open dir %s fail\n",sftp_cfg.client_path[i]);
				continue;
			}
			//判断文件是否符合条件
			while((read_dir=readdir(dir_info)))
			{

				add_flag=0;
				
				for(j=0;sftp_cfg.run&&j<sftp_cfg.upload_suffix_cnt;j++)
				{
					//检查后缀
					sprintf(suffix, ".%s", sftp_cfg.download_suffix[j]);
					suffix_tmp = strstr(read_dir->d_name, suffix);
					if(suffix_tmp == NULL || suffix_tmp[strlen(suffix_tmp)] != '\0')
						continue;
					
					//检查辅助文件是否存在
					if(sftp_cfg.upload_confirm == CONFIRM_AUXILIARY)
					{
						len=sprintf(confirm_tmp,"%s/",sftp_cfg.client_path[i]);
						
						strncpy(&confirm_tmp[len],read_dir->d_name,
											suffix_tmp-read_dir->d_name);
						len += suffix_tmp-read_dir->d_name;
						sprintf(&confirm_tmp[len],".%s",
							sftp_cfg.upload_confirm_suffix);
						if(access(confirm_tmp,F_OK)!=0)
							continue;
					}
					add_flag=1;
					break;
				}
				//加入上传链表
				if(add_flag)
				{
					file_in = calloc(sizeof(tsc_file_list_t),1);
					if(file_in == NULL)
						continue;
					//printf("name : %s \n path : %s\n",read_dir->d_name, 
					//									sftp_cfg.client_path[i]);
					strcpy(file_in->file_name, read_dir->d_name);
					sprintf(file_in->src_path, "%s/%s", sftp_cfg.client_path[i] ,
														read_dir->d_name);
					
					sprintf(file_in->dst_path, "%s/%s", sftp_cfg.server_path[i], 
														read_dir->d_name);
					tsc_list_add_tail(&file_in->list, &head);
				}
			}
			closedir(dir_info);
		}
		
		//遍历链表，上传文件
		tsc_list_for_each(list, &head)
		{
			file_out = tsc_list_entry(list, tsc_file_list_t, list);
			tsc_info("SFTP UPLOAD : start upload %s to %s",
									file_out->src_path, 
									file_out->dst_path);
			
			ret = tsc_sftp_upload(handle, file_out->dst_path, file_out->src_path);
			if(ret > 0)
			{
				tsc_info("SFTP UPLOAD : upload %s success", file_out->src_path);
				
				//删除或移动已上传完的文件,避免再次上传
				if(sftp_cfg.upload_remove==YES)
				{
					if(remove(file_out->src_path)!=0)
						tsc_error("SFTP UPLOAD : remove file %s fail",
														file_out->src_path);
				}
				else if(sftp_cfg.upload_complete_dir!=NULL
					&& sftp_cfg.upload_complete_dir[0]!='\0')
				{
					len = strstr(file_out->src_path, file_out->file_name) - 
															file_out->src_path;
					strncpy(path_tmp, file_out->src_path, len);
					
					path_tmp[len-1] = 0;
					
					sprintf(path2, "%s/%s/%s", path_tmp, 
											sftp_cfg.download_complete_dir,
											file_out->file_name);
					if(rename(file_out->src_path,path2)!=0)
						tsc_error("SFTP UPLOAD : rename %s to %s fail",
													file_out->src_path,path2);
				}
			}
			else
			{
				tsc_error("SFTP UPLOAD : upload %s fail", file_out->src_path);
			}
		}
		
		//清空链表
		tsc_list_for_each(list, &head)
		{
			file_out = tsc_list_entry(list, tsc_file_list_t, list);
			if(file_out)
			{
				free(file_out);
				file_out = NULL;
			}
		}
		tsc_list_head_init(&head);

	}
	return 0;
}

/*SFTP下载*/
static int tsc_sftp_download_start(void* handle)
{
	uint8_t i,j,k;
	uint8_t add_flag = 0;
	uint8_t is_first = 1;
	size_t len = 0;
	int ret=0;
	char path1[256];
	char path2[256];
	char path_tmp[256];
	char* suffix_tmp;
	char confirm_tmp[256];
	char confirm_name[128];
	char suffix[STR_LEN_32];
	tsc_list_t head;
	tsc_list_t* list;
	int file_cnt = 0;
	tsc_file_list_t* file_in;
	tsc_file_list_t* file_out;
	tsc_sftp_dir_info_t dir_info;
	
	tsc_list_head_init(&head);
	
	tsc_info("==================== begin download ====================");
	while(sftp_cfg.run)
	{
		memset(&dir_info, 0, sizeof(tsc_sftp_dir_info_t));
		
		//计算并保存所有服务端路径与对应客户端路径
		for(i=0;sftp_cfg.run&&i<sftp_cfg.server_path_cnt && is_first==1;i++)
		{
			//创建目录
			if(sftp_cfg.download_remove==NO
			&& sftp_cfg.download_complete_dir!=NULL
			&& sftp_cfg.download_complete_dir[0]!='\0')
			{
				sprintf(path1,"%s/%s",sftp_cfg.server_path[i],
					sftp_cfg.download_complete_dir);
				if(tsc_sftp_mkdir(handle, path1, LIBSSH2_SFTP_S_IRWXU |
										     	  LIBSSH2_SFTP_S_IRWXG |
										     	  LIBSSH2_SFTP_S_IRWXO) != 0)
				{
					tsc_error("SFTP DOWNLOAD : create remote dir %s failed!",
																		path1);
				}
			}
			if(tsc_create_local_dir(sftp_cfg.client_path[i]) != 0)
			{
				tsc_error("SFTP DOWNLOAD : create local dir %s failed!",
													sftp_cfg.client_path[i]);
			}
		}
		is_first = 0;
		//遍历文件列表,并加入到链表中
		for(i=0;sftp_cfg.run&&i<sftp_cfg.server_path_cnt;i++)
		{
			file_cnt = tsc_sftp_readdir(handle, sftp_cfg.server_path[i], &dir_info);
			for(j = 0;j<file_cnt;j++)
			{
				//检查是否为文件
				if(tsc_sftp_isdir(handle, dir_info.info[j].filepath) != 0)
					continue;
				
				add_flag = 0;
				//检查后缀
				for(k=0;k<sftp_cfg.download_suffix_cnt;k++)
				{
					sprintf(suffix, ".%s", sftp_cfg.download_suffix[k]);
					suffix_tmp = strstr(dir_info.info[j].filename, suffix);
					if(suffix_tmp != NULL && suffix_tmp[strlen(suffix)] == '\0')
					{
						//检查辅助文件
						if(sftp_cfg.download_confirm == CONFIRM_AUXILIARY)
						{
							
							len = sprintf(confirm_tmp,"%s/", sftp_cfg.server_path[i]);
							
							strncpy(&confirm_tmp[len],dir_info.info[j].filename, 
											suffix_tmp-dir_info.info[j].filename);
							
							strncpy(confirm_name,dir_info.info[j].filename, 
											suffix_tmp-dir_info.info[j].filename);
							sprintf(&confirm_name[suffix_tmp-dir_info.info[j].filename], 
												".%s", 
												sftp_cfg.download_confirm_suffix);
							len += suffix_tmp - dir_info.info[j].filename;
							sprintf(&confirm_tmp[len],".%s", sftp_cfg.download_confirm_suffix);
							ret = tsc_sftp_isdir(handle, confirm_tmp);
							if(ret == 0)
							{
								add_flag = 1;
								break;
							}
							else
								break;
						}
						else
						{
							add_flag = 1;
							break;
						}
					}
				}
				//加入上传链表
				if(add_flag == 1)
				{
					file_in = calloc(sizeof(tsc_file_list_t),1);
					if(file_in == NULL)
						continue;
					
					strcpy(file_in->file_name, dir_info.info[j].filename);
					strcpy(file_in->src_path, dir_info.info[j].filepath);
					if(sftp_cfg.download_confirm == CONFIRM_AUXILIARY)
					{
						strcpy(file_in->confirm_path, confirm_tmp);
						
						strcpy(file_in->confirm_name, confirm_name);
					}
					sprintf(file_in->dst_path, "%s/%s", sftp_cfg.client_path[i], 
														dir_info.info[j].filename);
					tsc_list_add_tail(&file_in->list, &head);
				}
			}
		}

		//遍历链表，下载文件
		tsc_list_for_each(list, &head)
		{
			file_out = tsc_list_entry(list, tsc_file_list_t, list);
			tsc_info("SFTP DOWNLOAD : start download %s to %s",
									file_out->src_path, 
									file_out->dst_path);
			
			ret = tsc_sftp_download(handle, file_out->src_path, file_out->dst_path);
			if(ret >= 0)
			{
				//上传成功后处理远端文件和辅助文件
				tsc_info("SFTP DOWNLOAD : download %s success", file_out->src_path);
				if(!sftp_cfg.download_remove)
				{
					len = strstr(file_out->src_path, file_out->file_name)-file_out->src_path;
					strncpy(path_tmp, file_out->src_path, len);
					
					path_tmp[len-1] = 0;
					
					sprintf(path2, "%s/%s/%s", path_tmp, 
											sftp_cfg.download_complete_dir,
											file_out->file_name);
					ret = tsc_sftp_rename(handle, file_out->src_path, path2);
					if(ret < 0)
					{
						tsc_error("SFTP DOWNLOAD : remove %s to %s fail!", 
													file_out->src_path, path2);
					}
				}
				
				if(file_out->confirm_path[0] != 0)
				{
					len = strstr(file_out->confirm_path, file_out->confirm_name)-file_out->confirm_path;
					strncpy(path_tmp, file_out->confirm_path, len);
					
					path_tmp[len-1] = 0;
					
					sprintf(path2, "%s/%s/%s", path_tmp, 
											sftp_cfg.download_complete_dir,
											file_out->confirm_name);
					ret = tsc_sftp_rename(handle, file_out->confirm_path, path2);
					if(ret < 0)
					{
						tsc_error("SFTP DOWNLOAD : remove %s fail!", 
													file_out->confirm_path);
					}
				}
			}
			else
			{
				tsc_error("SFTP DOWNLOAD : download %s fail", file_out->src_path);
			}
			
		}
		
		//清空链表
		tsc_list_for_each(list, &head)
		{
			file_out = tsc_list_entry(list, tsc_file_list_t, list);
			if(file_out)
			{
				free(file_out);
				file_out = NULL;
			}
		}
		tsc_list_head_init(&head);
	}
	return 0;
}

static int tsc_sftp_client_run()
{
	char str[1024];
	int ret = 0;
	void* handle = NULL;
	tsc_info("=============== sftp client run ==============");
	while(sftp_cfg.run)
	{
		//与sftp服务端创建连接
		handle = tsc_sftp_client_create(sftp_cfg.user_name, 
								 		sftp_cfg.password, 
										sftp_cfg.server_ip, 
								 		sftp_cfg.server_port, 
								 		NULL);
		if(handle == NULL )
		{
			tsc_info("SFTP CLIENT : CTRATE HANDLE FAIL!");
			return -1;
		}
		
		tsc_info("SFTP CLIENT : user %s@%s login success",sftp_cfg.user_name ,
															sftp_cfg.server_ip);
		//判断上下行
		switch(sftp_cfg.direction)
		{
			case DIRECTION_UPLOAD:
				//开始上传
				tsc_sftp_upload_start(handle);
				break;
			case DIRECTION_DOWNLOAD:
				//开始下载
				tsc_sftp_download_start(handle);
				break;
			default:
				tsc_error("SFTP CLIENT : unknown direction");
				tsc_sftp_client_destory(handle);
				return -1;
		}
		tsc_sftp_client_destory(handle);
	}
	tsc_info("SFTP CLIENT : log out!");
	return 0;
}


/*拉起线程*/
static void pull_up(int signo)
{
	pid_t pid;
	int waiter = 0;
	
	pid = waitpid(-1, &waiter, WNOHANG);
	if(fork()==0)
	{
		tsc_warn("SFTP : check child process died,pull up");
		tsc_sftp_client_run();	
	}
}

/*输出SFTP配置到日志*/
static void tsc_sftp_cfg()
{
	uint8_t i;
	tsc_info("==================== config info =====================");
	tsc_info("direction                : %s",direction_val(sftp_cfg.direction));	
	tsc_info("back_ground              : %s",bool_val(sftp_cfg.back_ground));
	
	tsc_info("log mode                 : %s",tsc_log_mode_val(sftp_cfg.log_mode));
	tsc_info("level mask               : %s",tsc_log_mask_val(sftp_cfg.level_mask));
	tsc_info("log_prefix               : %s",sftp_cfg.log_prefix);
	tsc_info("log_dir                  : %s",sftp_cfg.log_dir==NULL?"":sftp_cfg.log_dir);	
	tsc_info("log_file_size            : %u M",sftp_cfg.log_file_size);
	tsc_info("log_file_num             : %u ",sftp_cfg.log_file_num);

	tsc_info("server_ip                : %s",sftp_cfg.server_ip==NULL?"":sftp_cfg.server_ip);
	tsc_info("server_port              : %u",sftp_cfg.server_port);
	tsc_info("user_name                : %s",sftp_cfg.user_name==NULL?"":sftp_cfg.user_name);
	tsc_info("password                 : %s",sftp_cfg.password==NULL?"":sftp_cfg.password);
	
	tsc_info("client_path_cnt          : %u",sftp_cfg.client_path_cnt);
	for(i=0;i<sftp_cfg.client_path_cnt;i++)
		tsc_info("[%02u]                     : %s",i,sftp_cfg.client_path[i]);
	tsc_info("server_path_cnt          : %u",sftp_cfg.server_path_cnt);
	for(i=0;i<sftp_cfg.server_path_cnt;i++)
		tsc_info("[%02u]                     : %s",i,sftp_cfg.server_path[i]);
	
	tsc_info("====================== download =======================");
	tsc_info("download_remove          : %s",bool_val(sftp_cfg.download_remove));
	tsc_info("download_confirm         : %s",confirm_val(sftp_cfg.download_confirm));
	tsc_info("download_confirm_suffix  : %s",sftp_cfg.download_confirm_suffix==NULL?"":sftp_cfg.download_confirm_suffix);
	tsc_info("download_complete_dir    : %s",sftp_cfg.download_complete_dir==NULL?"":sftp_cfg.download_complete_dir);
	tsc_info("download_suffix_cnt      : %u",sftp_cfg.download_suffix_cnt);
	for(i=0;i<sftp_cfg.download_suffix_cnt;i++)
		tsc_info("[%02u]                     : %s",i,sftp_cfg.download_suffix[i]);
	
	tsc_info("======================== upload =========================");
	tsc_info("upload_remove            : %s",bool_val(sftp_cfg.upload_remove));
	tsc_info("upload_confirm           : %s",confirm_val(sftp_cfg.upload_confirm));
	tsc_info("upload_confirm_suffix    : %s",sftp_cfg.upload_confirm_suffix==NULL?"":sftp_cfg.upload_confirm_suffix);
	tsc_info("upload_complete_dir      : %s",sftp_cfg.upload_complete_dir==NULL?"":sftp_cfg.upload_complete_dir);
	tsc_info("upload_suffix_cnt        : %u",sftp_cfg.upload_suffix_cnt);
	for(i=0;i<sftp_cfg.upload_suffix_cnt;i++)
		tsc_info("[%02u]                     : %s",i,sftp_cfg.upload_suffix[i]);



}


int main(int argc , char** argv)
{
	int ret = 0;
	void* handle;
	if(argc < 2)
	{
		tsc_usage();
		return -1;
	}
	//初始化
	ret = tsc_sftp_init(argv[1]);
	if(ret < 0)
	{
		printf("INIT ERR : log init fail!\n");
		return -1;
	}
	tsc_sftp_cfg();
	
	signal(SIGINT,sigint_deal);
	sftp_cfg.run = 1;
	
	//启动线程
	if(sftp_cfg.back_ground==YES)
	{
		if(fork())
			exit(0);
		signal(SIGCHLD, pull_up);
		
		if(fork()==0)
		{
			tsc_sftp_client_run();	
		}
		else
		{
			tsc_info("SFTP CLIENT : sftp client running in background");
			while(sftp_cfg.run);
		}
	}
	else
	{
		tsc_sftp_client_run();	
	}
	
	tsc_info("SFTP : sftp client stop success");
	
	return 0;
}

