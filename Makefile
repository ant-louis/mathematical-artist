CXX = g++
CXXFLAGS = -W -Wall -ansi -pedantic --std=c++11

.PHONY : clean martist

martist: martist.o parser.o colorExpression.o
	ar rcu libmartist.a $^

martist.o: martist.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

colorExpression.o: colorExpression.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

parser.o: parser.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf *.o
