  
include $(TSC_ROOT)/src/mk/common-pre.mk 

TSC_ARCH ?= x86

TSC_LIB += $(LDLIBS) -L$(TSC_ROOT)/lib/x86
