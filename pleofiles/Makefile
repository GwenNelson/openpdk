all: pleoarchive.py

clean: clean_pleoarchive_py

pleoarchive.o: pleoarchive.cpp pleoarchive.h resource_list.h
	g++ -g -c pleoarchive.cpp -o pleoarchive.o

resource_list.o: resource_list.cpp resource_list.h
	g++ -g -c resource_list.cpp -o resource_list.o

pleoarchive.py: pleoarchive.i pleoarchive.o resource_list.o
	swig -classic -python -c++ pleoarchive.i
	gcc -fPIC -g -c pleoarchive_wrap.cxx -o pleoarchive_wrap.o -I/usr/include/python2.5
	g++ -g -lm -shared resource_list.o pleoarchive.o pleoarchive_wrap.o -o _pleoarchive.so

clean_pleoarchive_py:
	rm -f pleoarchive.o
	rm -f pleoarchive.py
	rm -f pleoarchive_wrap.cxx
	rm -f _pleoarchive.so
