all: urf_tool
clean: clean_urf_tool
urf_tool: urf_tool.cpp pleoarchive.cpp resource_list.cpp pleoarchive.h resource_list.h
	g++ -g urf_tool.cpp pleoarchive.cpp resource_list.cpp -o urf_tool
clean_urf_tool:
	rm -f *.o
	rm -f urf_tool
