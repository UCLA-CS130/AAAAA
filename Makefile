LDFLAGS=-pthread -lboost_system
CXXFLAGS=-Wall -Werror -std=c++0x -fprofile-arcs -ftest-coverage

# directory locations
CP_LOC=config_parser/
GTEST_DIR=googletest/googletest
GMOCK_DIR=googletest/googlemock

# test locations
SESSION_TEST=test/session_test.cpp session.cpp
UTILS_TEST=test/utils_test.cpp 
SERVER_TEST=test/server_test.cpp
PARSER_TEST=test/config_parser_test.cc

all: server.o session.o main.o config_parser.o utils.o HttpRequest.o
	g++ -o web-server main.o server.o session.o config_parser.o utils.o HttpRequest.o $(LDFLAGS) $(CXXFLAGS)


server.o: server.cpp server.h
	g++ -c server.cpp $(LDFLAGS) $(CXXFLAGS)

session.o: session.cpp session.h
	g++ -c session.cpp $(LDFLAGS) $(CXXFLAGS)

config_parser.o: $(CP_LOC)config_parser.cc $(CP_LOC)config_parser.h
	g++ -c $(CP_LOC)config_parser.cc -g $(CXXFLAGS)

utils.o: utils.cpp utils.h
	g++ -c utils.cpp $(LDFLAGS) $(CXXFLAGS)

main.o: main.cpp server.h session.h $(CP_LOC)config_parser.h utils.h
	g++ -c main.cpp $(LDFLAGS) $(CXXFLAGS)

HttpRequest.o: HttpRequest.h HttpRequest.cpp
	g++ -c HttpRequest.cpp $(LDFLAGS) $(CXXFLAGS)

.PHONY: clean, all, test

test: 
	g++ -c $(CP_LOC)config_parser.cc -g $(CXXFLAGS)
	g++ -c utils.cpp $(LDFLAGS) $(CXXFLAGS)
	g++ -std=c++0x -isystem ${GTEST_DIR}/include -I${GTEST_DIR} -pthread -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	g++ -isystem ${GTEST_DIR}/include ${SESSION_TEST} ${GTEST_DIR}/src/gtest_main.cc libgtest.a utils.o config_parser.o -o session_test ${LDFLAGS} ${CXXFLAGS}
	g++ -isystem ${GTEST_DIR}/include ${UTILS_TEST} ${GTEST_DIR}/src/gtest_main.cc libgtest.a utils.o config_parser.o -o utils_test ${LDFLAGS} ${CXXFLAGS}
	g++ -isystem ${GTEST_DIR}/include ${PARSER_TEST} ${GTEST_DIR}/src/gtest_main.cc libgtest.a utils.o config_parser.o -o config_parser_test ${LDFLAGS} ${CXXFLAGS}
	g++ -isystem ${GTEST_DIR}/include ${SERVER_TEST} ${GTEST_DIR}/src/gtest_main.cc libgtest.a utils.o config_parser.o -o server_test ${LDFLAGS} ${CXXFLAGS}
	./config_parser_test
	./server_test
	./session_test
	./utils_test

clean:
	rm -f *.o *.gcno *.gcov *.gcda
