################################################################################
# this is used for compile the kernel demo by rudy
################################################################################

# ha ha ha ha
	
rudy_kernel_demo_src = ${wildcard $(TOP_DIR)/demos/kernel_demo/api/*.c}	
rudy_kernel_demo_inc = -I $(TOP_DIR)/demos/kernel_demo/include
rudy_kernel_demo_defs =
		 
C_SOURCES += $(rudy_kernel_demo_src)
C_INCLUDES += $(rudy_kernel_demo_inc)
C_DEFS += $(rudy_kernel_demo_defs)
