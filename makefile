CXX=g++

default: vanish

vanish: vanish.o utilities.o
	$(CXX) -o vanish vanish.o utilities.o

vanish.o: vanish.cpp utilities.cpp vanish.hpp utilities.hpp
	$(CXX) -c vanish.cpp utilities.cpp

utilities.o: utilities.cpp
	$(CXX) -c utilities.cpp
