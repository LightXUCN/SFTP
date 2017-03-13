
ifeq ($(strip $(TSC_ROOT)),)
$(error "Please define TSC_ROOT environment variable")
endif

ifeq ($(strip $(TARGET)),)
$(error "Please define TARGET environment variable")
endif

ifeq ($(strip $(TEMPLATE)),)
$(error "Please define TEMPLATE environment variable")
endif

