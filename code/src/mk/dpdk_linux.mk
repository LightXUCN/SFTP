
include $(TSC_ROOT)/src/mk/common-pre.mk 

ifeq ($(strip $(RTE_SDK)),)
$(error "Please define TSC_ROOT environment variable");
endif

RTE_TARGET ?= x86_64-native-linux-gcc
TSC_ARCH ?= x86

include $(RTE_SDK)/mk/rte.vars.mk

ifeq ($(NO_AUTOLIBS),)

LDLIBS += -Wl,--whole-archive

ifeq ($(CONFIG_RTE_LIBRTE_DISTRIBUTOR),y)
LDLIBS += -lrte_distributor
endif

ifeq ($(CONFIG_RTE_LIBRTE_KNI),y)
ifeq ($(CONFIG_RTE_EXEC_ENV_LINUXAPP),y)
LDLIBS += -lrte_kni
endif
endif

ifeq ($(CONFIG_RTE_LIBRTE_IVSHMEM),y)
ifeq ($(CONFIG_RTE_EXEC_ENV_LINUXAPP),y)
LDLIBS += -lrte_ivshmem
endif
endif

ifeq ($(CONFIG_RTE_LIBRTE_PIPELINE),y)
LDLIBS += -lrte_pipeline
endif

ifeq ($(CONFIG_RTE_LIBRTE_TABLE),y)
LDLIBS += -lrte_table
endif

ifeq ($(CONFIG_RTE_LIBRTE_PORT),y)
LDLIBS += -lrte_port
endif

ifeq ($(CONFIG_RTE_LIBRTE_TIMER),y)
LDLIBS += -lrte_timer
endif

ifeq ($(CONFIG_RTE_LIBRTE_HASH),y)
LDLIBS += -lrte_hash
endif

ifeq ($(CONFIG_RTE_LIBRTE_LPM),y)
LDLIBS += -lrte_lpm
endif

ifeq ($(CONFIG_RTE_LIBRTE_POWER),y)
LDLIBS += -lrte_power
endif

ifeq ($(CONFIG_RTE_LIBRTE_ACL),y)
LDLIBS += -lrte_acl
endif

ifeq ($(CONFIG_RTE_LIBRTE_METER),y)
LDLIBS += -lrte_meter
endif

ifeq ($(CONFIG_RTE_LIBRTE_SCHED),y)
LDLIBS += -lrte_sched
LDLIBS += -lm
LDLIBS += -lrt
endif

LDLIBS += -Wl,--start-group

ifeq ($(CONFIG_RTE_LIBRTE_KVARGS),y)
LDLIBS += -lrte_kvargs
endif

ifeq ($(CONFIG_RTE_LIBRTE_MBUF),y)
LDLIBS += -lrte_mbuf
endif

ifeq ($(CONFIG_RTE_LIBRTE_IP_FRAG),y)
LDLIBS += -lrte_ip_frag
endif

ifeq ($(CONFIG_RTE_LIBRTE_ETHER),y)
LDLIBS += -lethdev
endif

ifeq ($(CONFIG_RTE_LIBRTE_MALLOC),y)
LDLIBS += -lrte_malloc
endif

ifeq ($(CONFIG_RTE_LIBRTE_MEMPOOL),y)
LDLIBS += -lrte_mempool
endif

ifeq ($(CONFIG_RTE_LIBRTE_RING),y)
LDLIBS += -lrte_ring
endif

ifeq ($(CONFIG_RTE_LIBC),y)
LDLIBS += -lc
LDLIBS += -lm
endif

ifeq ($(CONFIG_RTE_LIBGLOSS),y)
LDLIBS += -lgloss
endif

ifeq ($(CONFIG_RTE_LIBRTE_EAL),y)
LDLIBS += -lrte_eal
endif

ifeq ($(CONFIG_RTE_LIBRTE_CMDLINE),y)
LDLIBS += -lrte_cmdline
endif

ifeq ($(CONFIG_RTE_LIBRTE_CFGFILE),y)
LDLIBS += -lrte_cfgfile
endif

ifeq ($(CONFIG_RTE_LIBRTE_PMD_BOND),y)
LDLIBS += -lrte_pmd_bond
endif

ifeq ($(CONFIG_RTE_LIBRTE_PMD_XENVIRT),y)
LDLIBS += -lrte_pmd_xenvirt
LDLIBS += -lxenstore
endif

ifeq ($(CONFIG_RTE_BUILD_SHARED_LIB),n)
# plugins (link only if static libraries)

ifeq ($(CONFIG_RTE_LIBRTE_VMXNET3_PMD),y)
LDLIBS += -lrte_pmd_vmxnet3_uio
endif

ifeq ($(CONFIG_RTE_LIBRTE_VIRTIO_PMD),y)
LDLIBS += -lrte_pmd_virtio_uio
endif

ifeq ($(CONFIG_RTE_LIBRTE_I40E_PMD),y)
LDLIBS += -lrte_pmd_i40e
endif

ifeq ($(CONFIG_RTE_LIBRTE_IXGBE_PMD),y)
LDLIBS += -lrte_pmd_ixgbe
endif

ifeq ($(CONFIG_RTE_LIBRTE_E1000_PMD),y)
LDLIBS += -lrte_pmd_e1000
endif

ifeq ($(CONFIG_RTE_LIBRTE_PMD_RING),y)
LDLIBS += -lrte_pmd_ring
endif

ifeq ($(CONFIG_RTE_LIBRTE_PMD_PCAP),y)
LDLIBS += -lrte_pmd_pcap -lpcap
endif

endif # plugins

LDLIBS += $(EXECENV_LDLIBS)

LDLIBS += -Wl,--end-group

LDLIBS += -Wl,--no-whole-archive

endif # ifeq ($(NO_AUTOLIBS),)
LDLIBS += $(CPU_LDLIBS)

TSC_LIB += $(LDLIBS) -L$(TSC_ROOT)/lib/x86
