#include "cli.h"

#include <cassert>
#include <iterator>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <iostream>
#include <string>

std::ostream& nl(std::ostream& out)
{
    return out << "\n";
}

std::ostream& tab(std::ostream& out)
{
    return out << "\t";
}

cli::cli()
{
    registerCommand("help", std::bind(&cli::help, this));
    registerCommand("quit", std::bind(&exit, EXIT_SUCCESS));
}

void cli::execute() const
{
    std::string input;

    std::cout << std::endl << "# " << std::flush;

    while (getline(std::cin, input)) {
        auto command = my_commands.find(input);

        if (command == end(my_commands)) {
            // no command, trying an alias
            auto alias = my_aliases.find(input);
            if (alias != end(my_aliases)) {
                command = my_commands.find(alias->second);
            }
        }

        if (command == end(my_commands)) {
            // no command nor alias
            std::cout << "**Unknown command <" << input << ">" << nl << tab
                      << "type command <help> for a list of available commands";
        }
        else {
            try {
                command->second();
            }
            catch(...) {
                std::cerr << "**Caught exception while executing command <" << input << ">" << std::endl;
                auto e = std::current_exception();
            }
        }

        std::cout << std::endl << "# " << std::flush;
    }
}

std::string cli::requestArgument(const std::string& arg_name)
{
    std::cout << "  Please provide argument <" << arg_name << "> : " << std::flush;
    std::string buffer;
	std::getline(std::cin, buffer);
    return buffer;
}

std::vector<std::string> cli::generateAliases(const std::string& command)
{
    std::vector<std::string> result;

    result.push_back(command);

    auto spacePos = command.find(' ');
    if (spacePos != std::string::npos) {
        // add first word
//         result.push_back(command.substr(0, spacePos));

        // add shortcut (first letter of each word)
        std::string shortcut;
        auto buffer = command;
        shortcut.push_back(buffer.front());

        while(spacePos != std::string::npos) {
            buffer = buffer.substr(spacePos+1);
            shortcut.push_back(buffer.front());
            spacePos = buffer.find(' ');
        }
        
        result.push_back(shortcut);
    }
    else {
        // single word, add first letter
        result.push_back({command.front()});
    }

    return result;
}

void cli::registerCommand(const std::string& identifier, std::function<void()> callback)
{
    registerAliasedCommand(generateAliases(identifier), callback);
}

void cli::registerAliasedCommand(const std::vector<std::string> identifiers, std::function<void()> callback)
{
    auto identifier = identifiers.front();

    registerSimpleCommand(identifier, callback);

    auto aliases = identifiers;
    aliases.erase(begin(aliases));

    for (const auto& alias : aliases) {
        try {
            registerAlias(identifier, alias);
        }
        catch(std::logic_error& e) {
            // we cannot register this alias, so to avoid ambiguity we also remove previous alias
            my_aliases.erase(alias);
        }
    }
}

void cli::registerSimpleCommand(const std::string& identifier, std::function<void()> callback)
{
    if (my_commands.find(identifier) != end(my_commands)) {
        throw std::logic_error("Cannot register identifier <" + identifier + "> twice");
    }

    my_commands[identifier] = callback;
}

void cli::registerAlias(const std::string& identifier, const std::string& alias)
{
    if (my_commands.find(identifier) == end(my_commands)) {
        throw std::logic_error("Cannot register alias to unknown identifier <" + identifier + ">");
    }

    if (my_commands.find(alias) != end(my_commands)) {
        throw std::logic_error("Cannot register alias <" + alias + "> as a command already has this name.");
    }

    if (my_aliases.find(alias) != end(my_aliases)) {
        throw std::logic_error("Cannot register alias <" + alias + "> twice");
    }

    my_aliases[alias] = identifier;
}

void cli::help() const
{
    std::vector<std::string> command_list;
    std::map<std::string, std::vector<std::string>> aliases;

    std::transform(begin(my_commands),
                   end(my_commands),
                   std::back_inserter(command_list),
                   [](const decltype(my_commands)::value_type& kv) { return kv.first; });
    std::sort(begin(command_list), end(command_list));

    for (const auto& kv : my_aliases) {
        aliases[kv.second].push_back(kv.first);
    }

    std::cout << "**Available commands:" << nl;
    for (const auto& command : command_list) {
        std::cout << tab << "- <" << command << ">";
        auto aliasesIt = aliases.find(command);
        if (aliasesIt != end(aliases)) {
            std::cout << " alias";
            for (const auto& alias : aliasesIt->second) {
                std::cout << " <" << alias << ">";
            }
        }
        std::cout << nl;
    }
}
