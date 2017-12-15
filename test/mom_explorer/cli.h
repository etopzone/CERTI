#ifndef CLI_H
#define CLI_H

#include <functional>
#include <map>
#include <vector>

class cli {
public:
    cli();
    
    void execute() const;
    
    static std::string requestArgument(const std::string& arg_name);
    
    static std::vector<std::string> generateAliases(const std::string& command);
    
protected:
    void registerCommand(const std::string& identifier, std::function<void()> callback);
    
    void registerAliasedCommand(std::vector<std::string> identifiers, std::function<void()> callback);
    void registerSimpleCommand(const std::string& identifier, std::function<void()> callback);
    void registerAlias(const std::string& identifier, const std::string& alias);
    
private:
    void help() const;
    
    std::map<std::string, std::function<void()>> my_commands;
    std::map<std::string, std::string> my_aliases;
};

#endif // CLI_H
