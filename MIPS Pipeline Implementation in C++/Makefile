# Specify the compiler to use
CXX = g++
# Specify the flags to use for compiling
# CXXFLAGS = -Wall -Wextra -Werror -std=c++11
# Specify the source files for your project
SRCS = 5stage.cpp 5stage_bypass.cpp 79stage.cpp 79stage_bypass.cpp
# Specify the name for the generated executable
EXECS = 5stage 5stage_bypass 79stage 79stage_bypass
# Target for compiling your program
compile: $(EXECS)

$(EXECS): %: %.cpp BranchPredictor.hpp
	$(CXX) -o $@ $<
# Target for running the 5-stage pipeline simulator
run_5stage: 5stage
	./5stage input.asm
# Target for running the 5-stage pipeline simulator with forwarding and bypassing
run_5stage_bypass: 5stage_bypass
	./5stage_bypass input.asm
# Target for running the 7 and 9-stage pipeline simulator
run_79stage: 79stage
	./79stage input.asm
# Target for running the 7 and 9-stage pipeline simulator with forwarding and bypassing
run_79stage_bypass: 79stage_bypass
	./79stage_bypass input.asm
# Target for cleaning up generated files
clean:
	rm -f $(EXECS)