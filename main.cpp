#include "gvzork.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>


Game::Game() {
    // Define main commands
    commands["drink"] = std::bind(&Game::drink, this, std::placeholders::_1);
    commands["help"] = std::bind(&Game::showHelp, this, std::placeholders::_1);
    commands["look"] = std::bind(&Game::look, this, std::placeholders::_1);
    commands["move"] = std::bind(&Game::move, this, std::placeholders::_1);
    commands["quit"] = std::bind(&Game::quit, this, std::placeholders::_1);
    commands["talk"] = std::bind(&Game::talk, this, std::placeholders::_1);

    // Define aliases for commands
    commandAliases["go"] = "move";
    commandAliases["walk"] = "move";
    commandAliases["run"] = "move";
    commandAliases["exit"] = "quit";
    commandAliases["speak"] = "talk";
    commandAliases["chat"] = "talk";
    commandAliases["view"] = "look";
    commandAliases["observe"] = "look";
    commandAliases["consume"] = "drink";
    commandAliases["chug"] = "drink";

    createWorld();
    drunkness = 0;
}

void Game::normalizeCommand(std::string& command) {
    // Convert command to lowercase (optional for case insensitivity)
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    // Check if command has an alias
    if (commandAliases.find(command) != commandAliases.end()) {
        command = commandAliases[command];  // Normalize to the main command
    }
}

void Game::executeCommand(std::string command, std::vector<std::string> args) {
    normalizeCommand(command);  // Convert aliases to proper commands

    if (commands.find(command) != commands.end()) {
        commands[command](args);
    } else {
        std::cout << "Unknown command! Type 'help' for a list of commands." << std::endl;
    }
}

// Dummy implementation lambdas for now
void Game::drink(std::vector<std::string> args) { std::cout << "You take a sip... or maybe chug it all!" << std::endl; }
void Game::look(std::vector<std::string> args) { std::cout << "Looking around..." << std::endl; }
void Game::move(std::vector<std::string> args) { std::cout << "You move to a new location." << std::endl; }
void Game::createWorld() { std::cout << "Creating the world..." << std::endl; }
void Game::quit(std::vector<std::string> args) { std::cout << "Quitting game..." << std::endl; exit(0); }
void Game::showHelp(std::vector<std::string> args) {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}
void Game::talk(std::vector<std::string> args) { std::cout << "You start a conversation..." << std::endl; }

void Game::play() {
    std::cout << "Starting the game..." << std::endl;

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) continue;

        // Split input into command and arguments
        std::vector<std::string> args;
        std::string command;
        size_t pos = input.find(' ');
        if (pos != std::string::npos) {
            command = input.substr(0, pos);
            std::string remaining = input.substr(pos + 1);

            size_t argPos;
            while ((argPos = remaining.find(' ')) != std::string::npos) {
                args.push_back(remaining.substr(0, argPos));
                remaining = remaining.substr(argPos + 1);
            }
            if (!remaining.empty()) args.push_back(remaining);
        } else {
            command = input;
        }

        executeCommand(command, args);
    }
}

int main() {
    Game game;
    game.play();
    return 0;
}