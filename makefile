PROGRAM = eracalc
CXX = g++ -std=c++11
LIBS =
INCLUDEDIRS =
COPTS = $(INCLUDEDIRS)$(LIBS)-fpermissive
CMAKE_COMMAND = /usr/bin/cmake

OBJS = Main.o

.SUFFIXES: .cpp .o

$(PROGRAM):$(OBJS)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red "Creating executable.." 
	$(CXX) $(OBJS) -o $(PROGRAM) $(COPTS)
	
%.o:	
	
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling source file: $<" 
	$(CXX) $(COPTS) -c $<

Main.o: Main.cpp EraCalc.h

clean : 
	rm -f $(PROGRAM) *.o

run : $(PROGRAM)
	./$(PROGRAM)
