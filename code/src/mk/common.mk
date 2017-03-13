
include $(TSC_ROOT)/src/mk/common-pre.mk 

#common toolchain
MKDIR = mkdir -p
DEL = rm -rf

#common function
#convert source to obj file
C2OBJ = $(strip $(patsubst %.c,$(1)/%.o,$(patsubst %.C,$(1)/%.o,$(2))))
S2OBJ = $(strip $(patsubst %.s,$(1)/%.o,$(patsubst %.S,$(1)/%.o,$(2))))

TSC_BUILD_PATH = $(TSC_ROOT)/.output

#environment variable
TSC_SOURCES += $(SOURCES) 
TSC_INCLUDE += $(INCLUDE) -I$(TSC_ROOT)/include
TSC_DEFINE += $(DEFINE)
TSC_CFLAGS +=  $(CFLAGS) -W -Wall -Wno-unused-parameter
TSC_LDFLAGS += $(LDFLAGS)
TSC_LIB += $(LIBS)
TSC_ARCH = x86

ifeq ($(strip $(TSC_ARCH)),x86)
TSC_DEFINE += -DTSC_X86
endif

TSC_OBJS_PATH = $(TSC_BUILD_PATH)/obj/$(TSC_ARCH)/$(notdir $(TSC_TARGET))
TSC_OBJECTS = $(call C2OBJ,$(TSC_OBJS_PATH),$(call S2OBJ,$(TSC_OBJS_PATH),$(TSC_SOURCES)))

#first target(app)
ifeq ($(strip $(TEMPLATE)),app)
TSC_TARGET = $(TSC_ROOT)/bin/$(TSC_ARCH)/$(TARGET)
all:$(TSC_TARGET)
$(TSC_TARGET):$(TSC_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(CC) $(TSC_OBJECTS) $(TSC_LDFLAGS) $(TSC_LIB) -o $(TSC_TARGET)
	$(DEL) $(TSC_BUILD_PATH)
endif

#first target(.a)
ifeq ($(strip $(TEMPLATE)),lib)
ifeq ($(strip $(suffix $(TARGET))),a)
TSC_TARGET = $(TSC_ROOT)/lib/$(TSC_ARCH)/$(TARGET)
else
TSC_TARGET = $(TSC_ROOT)/lib/$(TSC_ARCH)/lib$(TARGET).a
endif
all:$(TSC_TARGET)
$(TSC_TARGET):$(TSC_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(AR) rc $(TSC_TARGET) $(TSC_OBJECTS)
	$(DEL) $(TSC_BUILD_PATH)
endif

#first target(.so)
ifeq ($(strip $(TEMPLATE)),so)
ifeq ($(strip $(suffix $(TARGET))),so)
TSC_TARGET = $(TSC_ROOT)/lib/$(TSC_ARCH)/$(TARGET)
else
TSC_TARGET = $(TSC_ROOT)/lib/$(TSC_ARCH)/lib$(TARGET).so
endif
ifeq ($(strip $(TSC_CFLAGS)),-fPIC)
else
TSC_CFLAGS += -fPIC
endif
all:$(TSC_TARGET)
$(TSC_TARGET):$(TSC_OBJECTS)
	@$(MKDIR) $(dir $@)
	$(CC) $(TSC_LDFLAGS) -shared -o $@ $(TSC_OBJECTS) $(TSC_LIB)
	$(DEL) $(TSC_BUILD_PATH)
endif

ifeq ($(strip $(TSC_TARGET)),)
$(error "Unknown TEMPLATE type(app/so/lib):$(TEMPLATE)")
endif	

$(TSC_OBJS_PATH)/%.o:%.c
	@$(MKDIR) $(dir $@)	
	$(CC) $(TSC_INCLUDE) $(TSC_DEFINE) $(TSC_CFLAGS) -o $@ -c $<

$(TSC_OBJS_PATH)/%.o:%.C
	@$(MKDIR) $(dir $@)	
	$(CC) $(TSC_INCLUDE) $(TSC_DEFINE) $(TSC_CFLAGS) -o $@ -c $<

$(TSC_OBJS_PATH)/%.o:%.s
	@$(MKDIR) $(dir $@)	
	$(CC) $(TSC_INCLUDE) $(TSC_DEFINE) $(TSC_CFLAGS) -o $@ -c $<

$(TSC_OBJS_PATH)/%.o:%.S
	@$(MKDIR) $(dir $@)	
	$(CC) $(TSC_INCLUDE) $(TSC_DEFINE) $(TSC_CFLAGS) -o $@ -c $<

clean:
	$(DEL) $(TSC_OBJECTS) 
	$(DEL) $(TSC_TARGET)

