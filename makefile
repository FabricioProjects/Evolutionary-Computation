opt: opt.o defs.o funcs.o chromo.o ga.o io-dodd.o pugixml.o services.o
	g++ $(CFLAGS) -o opt opt.o defs.o funcs.o chromo.o ga.o io-dodd.o pugixml.o services.o -I .
opt.o: opt.cc defs.h funcs.h chromo.h ga.h io-dodd.h services.h
	g++ $(CFLAGS) -c opt.cc -I .
ga.o: ga.cc chromo.h defs.h funcs.h io-dodd.h pugixml.hpp services.h
	g++ $(CFLAGS) -c ga.cc -I .
io-dodd.o: io-dodd.cc io-dodd.h defs.h
	g++ $(CFLAGS) -c io-dodd.cc -I . 
chromo.o: chromo.cc chromo.h defs.h
	g++ $(CFLAGS) -c chromo.cc -I .
defs.o: defs.cc defs.h
	g++ $(CFLAGS) -c defs.cc -I .
funcs.o: funcs.cc funcs.h
	g++ $(CFLAGS) -c funcs.cc -I .
services.o: services.cc services.h
	g++ $(CFLAGS) -c services.cc -I .
pugixml.o: pugixml.cpp pugixml.hpp pugiconfig.hpp
	g++ $(CFLAGS) -c pugixml.cpp -I .
clean:
	rm -f *.o opt *.h~ *.cc~
