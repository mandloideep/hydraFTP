# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra

# Directories
SRC_CLIENT_DIR := src/app/client
SRC_SERVER_DIR := src/app/server
DIST_DIR := dist
BIN_DIR := dist/bin

# Find all .cpp files in source directories
CLIENT_SRCS := $(wildcard $(SRC_CLIENT_DIR)/*.cpp)
SERVER_SRCS := $(wildcard $(SRC_SERVER_DIR)/*.cpp)

# Generate object file names
CLIENT_OBJS := $(patsubst $(SRC_CLIENT_DIR)/%.cpp,$(DIST_DIR)/client/%.o,$(CLIENT_SRCS))
SERVER_OBJS := $(patsubst $(SRC_SERVER_DIR)/%.cpp,$(DIST_DIR)/server/%.o,$(SERVER_SRCS))

# Binary names
CLIENT_BIN := $(BIN_DIR)/client
SERVER_BIN := $(BIN_DIR)/server

# Default target
all: dirs $(CLIENT_BIN) $(SERVER_BIN)

# Create necessary directories
dirs:
	@mkdir -p $(DIST_DIR)/client
	@mkdir -p $(DIST_DIR)/server
	@mkdir -p $(BIN_DIR)

# Compile client object files
$(DIST_DIR)/client/%.o: $(SRC_CLIENT_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile server object files
$(DIST_DIR)/server/%.o: $(SRC_SERVER_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link client binary
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o $@

# Link server binary
$(SERVER_BIN): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o $@

# Clean build files
clean:
	rm -rf $(DIST_DIR)

# Phony targets
.PHONY: all clean dirs