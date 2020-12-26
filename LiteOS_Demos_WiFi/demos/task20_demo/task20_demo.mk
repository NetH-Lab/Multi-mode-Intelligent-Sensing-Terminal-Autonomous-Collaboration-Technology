################################################################################
# this is used for compile the task20 demo by rudy
################################################################################

# ha ha ha ha
	
rudy_task20_demo_src = ${wildcard $(TOP_DIR)/demos/task20_demo/api/*.c}	
rudy_task20_demo_inc = -I $(TOP_DIR)/demos/task20_demo/include
rudy_task20_demo_defs =
		 
C_SOURCES += $(rudy_task20_demo_src)
C_INCLUDES += $(rudy_task20_demo_inc)
C_DEFS += $(rudy_task20_demo_defs)
