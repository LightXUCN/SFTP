#工程文件的根目录
TSC_ROOT = ../..
TSC_RELEASE = ../../../release
#目标文件类型,可为: app、lib(.a)、so(.dll)
TEMPLATE = app

#目标文件名
TARGET = sftp_client

include $(TSC_ROOT)/src/mk/x86.mk
#
#源文件
SOURCES = 	sftp_client.c

#宏定义
DEFINE =	

#编译选项
CFLAGS = -g -O0

#链接选项
LDFLAGS =

#依赖库
LIBS += -L$(TSC_ROOT)/$(TSC_RELEASE)/ini/lib/x86		-ltsc_ini_io\
		-L$(TSC_ROOT)/$(TSC_RELEASE)/sftp/lib/x86		-ltsc_sftp\
		-L$(TSC_ROOT)/$(TSC_RELEASE)/utils/lib/x86		-ltsc_utils\
		-L$(TSC_ROOT)/$(TSC_RELEASE)/hal/lib/x86		-ltsc_hal\
		-L$(TSC_ROOT)/$(TSC_RELEASE)/log4c/lib 			-llog4c\
		-L$(TSC_ROOT)/$(TSC_RELEASE)/ssh2/lib			-lssh2\
		-lssl\
		-lcrypto\
		-lz\
		-lrt
		
#包含文件头路径
INCLUDE +=	-I$(TSC_ROOT)/include\
			-I$(TSC_ROOT)/$(TSC_RELEASE)/ssh2/include\
			-I$(TSC_ROOT)/$(TSC_RELEASE)/ini/include\
			-I$(TSC_ROOT)/$(TSC_RELEASE)/hal/include\
			-I$(TSC_ROOT)/$(TSC_RELEASE)/utils/include\
			-I$(TSC_ROOT)/$(TSC_RELEASE)/log4c/include

include $(TSC_ROOT)/src/mk/common.mk

