CXX=g++

default: vanish

vanish: vanish.o utilities.o background.o redirection.o piping.o
	$(CXX) -o vanish vanish.o utilities.o background.o redirection.o piping.o

vanish.o: vanish.cpp utilities.cpp background.cpp redirection.cpp piping.cpp vanish.hpp utilities.hpp redirection.hpp background.hpp piping.hpp
	$(CXX) -c vanish.cpp utilities.cpp background.cpp redirection.cpp piping.cpp

utilities.o: utilities.cpp
	$(CXX) -c utilities.cpp

background.o: background.cpp
	$(CXX) -c background.cpp

redirection.o: redirection.cpp
	$(CXX) -c redirection.cpp

piping.o: piping.cpp
	$(CXX) -c piping.cpp