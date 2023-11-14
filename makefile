# This makefile template is from https://makefiletutorial.com/
TARGET_EXEC := DictFind
BUILD_DIR := ./build
SRC_DIRS := ./source
CXX=g++
CXXFLAGS = -std=c++11 -Wall -pedantic

# Find all the C and C++ files we want to compile
# Note the single quotes around the * expressions. The shell will incorrectly expand these otherwise, but we want to send the * directly to the find command.
SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')

# Prepends BUILD_DIR and appends .o to every src file
# As an example, ./your_dir/hello.cpp turns into ./build/./your_dir/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -MMD -MP

# additional source files
OTHER_SRC := $(SRC_DIRS)/dict.txt
OTHER_DST := $(BUILD_DIR)/dict.txt

# The final build step.
$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS) $(OTHER_DST)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# rule to copy dict.txt
$(BUILD_DIR)/dict.txt : $(SRC_DIRS)/dict.txt
	cp $< $@

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf DictFind

run:	$(BUILD_DIR)/$(TARGET_EXEC)

	cd $(BUILD_DIR) && ./$(TARGET_EXEC)

memCheck:
	cd $(BUILD_DIR) && leaks -quiet -atExit -- ./$(TARGET_EXEC)

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)