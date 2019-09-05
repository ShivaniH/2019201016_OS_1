CXX=g++

default: vanish

vanish: vanish.o utilities.o background.o redirection.o
	$(CXX) -o vanish vanish.o utilities.o background.o redirection.o

vanish.o: vanish.cpp utilities.cpp background.cpp redirection.cpp vanish.hpp utilities.hpp redirection.hpp background.hpp
	$(CXX) -c vanish.cpp utilities.cpp background.cpp redirection.cpp

utilities.o: utilities.cpp
	$(CXX) -c utilities.cpp

background.o: background.cpp
	$(CXX) -c background.cpp

redirection.o: redirection.cpp
	$(CXX) -c redirection.cpp
