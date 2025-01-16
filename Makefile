# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

# Directories
SRC_CLIENT_DIR := src/app/client
SRC_SERVER_DIR := src/app/server
SRC_COMMON_DIR := src/app/common
DIST_DIR := dist
BIN_DIR := dist/bin

# Include directories
INCLUDES := -I$(SRC_CLIENT_DIR)/include -I$(SRC_SERVER_DIR)/include -I$(SRC_COMMON_DIR)/include

# Update CXXFLAGS to include the include directories
CXXFLAGS += $(INCLUDES)

# Find all .cpp files in source directories
CLIENT_SRCS := $(wildcard $(SRC_CLIENT_DIR)/*.cpp)
SERVER_SRCS := $(wildcard $(SRC_SERVER_DIR)/*.cpp)
COMMON_SRCS := $(wildcard $(SRC_COMMON_DIR)/*.cpp)

# Generate object file names
CLIENT_OBJS := $(patsubst $(SRC_CLIENT_DIR)/%.cpp,$(DIST_DIR)/client/%.o,$(CLIENT_SRCS))
SERVER_OBJS := $(patsubst $(SRC_SERVER_DIR)/%.cpp,$(DIST_DIR)/server/%.o,$(SERVER_SRCS))
COMMON_OBJS := $(patsubst $(SRC_COMMON_DIR)/%.cpp,$(DIST_DIR)/common/%.o,$(COMMON_SRCS))

# Binary names
CLIENT_BIN := $(BIN_DIR)/client
SERVER_BIN := $(BIN_DIR)/server

# Load environment variables
include .env
export

# Default target
all: dirs $(CLIENT_BIN) $(SERVER_BIN)

# Create necessary directories
dirs:
	@mkdir -p $(DIST_DIR)/client
	@mkdir -p $(DIST_DIR)/server
	@mkdir -p $(DIST_DIR)/common
	@mkdir -p $(BIN_DIR)

# Compile client object files
$(DIST_DIR)/client/%.o: $(SRC_CLIENT_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile server object files
$(DIST_DIR)/server/%.o: $(SRC_SERVER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile common object files
$(DIST_DIR)/common/%.o: $(SRC_COMMON_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link client binary
$(CLIENT_BIN): $(CLIENT_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) $(COMMON_OBJS) -o $@

# Link server binary
$(SERVER_BIN): $(SERVER_OBJS) $(COMMON_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) $(COMMON_OBJS) -o $@



# Run targets
run-client: $(CLIENT_BIN)
	@( . ./.env && ./$(CLIENT_BIN) )

run-server: $(SERVER_BIN)
	@( . ./.env && ./$(SERVER_BIN) )

# Debug targets
run-client-debug: $(CLIENT_BIN)
	gdb ./$(CLIENT_BIN)

run-server-debug: $(SERVER_BIN)
	gdb ./$(SERVER_BIN)

run-both: $(CLIENT_BIN) $(SERVER_BIN)
	./$(SERVER_BIN) & ./$(CLIENT_BIN)

run-both-debug: $(CLIENT_BIN) $(SERVER_BIN)
	gdb -ex run ./$(SERVER_BIN) & gdb -ex run ./$(CLIENT_BIN)

# Clean build files
clean:
	rm -rf $(DIST_DIR)

# Phony targets
.PHONY: all clean dirs run-client run-server