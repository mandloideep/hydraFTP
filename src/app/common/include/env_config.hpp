#ifndef ENV_CONFIG_HPP
#define ENV_CONFIG_HPP

#include <string>

class EnvConfig {
public:
    // Client-specific getters
    static int getClientPort();
    static std::string getClientServerIP();
    static int getClientBufferSize();
    
    // Server-specific getters
    static int getServerPort();
    static int getServerBufferSize();
    static int getServerBacklogSize();
    
private:
    // Helper methods
    static int getEnvAsInt(const char* key, int defaultValue);
    static std::string getEnvAsString(const char* key, const std::string& defaultValue);
};

#endif // ENV_CONFIG_HP