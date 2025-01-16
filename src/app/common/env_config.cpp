#include "./include/env_config.hpp"
#include <cstdlib>
#include <stdexcept>
#include <iostream>

int EnvConfig::getEnvAsInt(const char *key, int defaultValue)
{
    const char *value = std::getenv(key);
    return value ? std::stoi(value) : defaultValue;
    // if (value)
    // {
    //     std::cout << "Found env var " << key << "=" << value << std::endl;
    //     return std::stoi(value);
    // }
    // std::cout << "Using default value for " << key << "=" << defaultValue << std::endl;
    // return defaultValue;
}

std::string EnvConfig::getEnvAsString(const char *key, const std::string &defaultValue)
{
    const char *value = std::getenv(key);
    return value ? std::string(value) : defaultValue;
    // if (value)
    // {
    //     std::cout << "Found env var " << key << "=" << value << std::endl;
    //     return std::string(value);
    // }
    // std::cout << "Using default value for " << key << "=" << defaultValue << std::endl;
    // return defaultValue;
}

// Client-specific implementations
int EnvConfig::getClientPort()
{
    return getEnvAsInt("CLIENT_PORT", 6120);
}

std::string EnvConfig::getClientServerIP()
{
    return getEnvAsString("CLIENT_SERVER_IP", "127.0.0.1");
}

int EnvConfig::getClientBufferSize()
{
    return getEnvAsInt("CLIENT_BUFFER_SIZE", 4096);
}

// Server-specific implementations
int EnvConfig::getServerPort()
{
    return getEnvAsInt("SERVER_PORT", 6120);
}

int EnvConfig::getServerBufferSize()
{
    return getEnvAsInt("SERVER_BUFFER_SIZE", 4096);
}

int EnvConfig::getServerBacklogSize()
{
    return getEnvAsInt("SERVER_BACKLOG_SIZE", 20);
}