#include "gvzork.h"
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>


Item::Item(const std::string& name, const std::string& description, int calories, float weight) {
    if (name.empty()) throw std::invalid_argument("Name cannot be blank.");
    if (description.empty()) throw std::invalid_argument("Description cannot be blank.");
    if (calories < 0 || calories > 1000) throw std::invalid_argument("Calories must be between 0 and 1000.");
    if (weight < 0 || weight > 500) throw std::invalid_argument("Weight must be between 0 and 500.");

    this->name = name;
    this->description = description;
    this->calories = calories;
    this->weight = weight;
}

std::string Item::getName() const { return name; }
std::string Item::getDescription() const { return description; }
int Item::getCalories() const { return calories; }
float Item::getWeight() const { return weight; }

std::ostream& operator<<(std::ostream& os, const Item& item) {
    os << item.name << " (" << item.calories << " calories)- " << item.weight << " lb- " << item.description;
    return os;
}


NPC::NPC(const std::string& name, const std::string& description) {
    if (name.empty() || description.empty()) {
        throw std::invalid_argument("Name and description cannot be blank.");
    }

    this->name = name;
    this->description = description;
    this->messageNumber = 0;
    }
    
    std::string NPC::getName() const { return name; }
    
    std::string NPC::getDescription() const { return description; }

    void NPC::addMessage(std::string& message) {
        messages.push_back(message);
    }

    std::string NPC::getMessage() {
        if (messages.empty()) {
            return ("This NPC has no messages.");
        }
        std::string message = messages[messageNumber];
        messageNumber = (messageNumber + 1) % messages.size();
        return message;
    }

    std::ostream& operator<<(std::ostream& os, const NPC& npc) {
        os << npc.name;
        return os;
    }

Location::Location(const std::string& name, const std::string& description) {
    if (name.empty() || description.empty()) throw std::invalid_argument("Name and description cannot be blank.");
    this->name = name;
    this->description = description;
    this->visited = false;
    }
    
    std::map<std::string, Location*> Location::get_locations() const { return neighbors;}

    void Location::add_location(std::string& direction, Location* location){
        if (direction.empty()){
            throw std::invalid_argument("Direction cannot be empty.");
        }
        if (neighbors.count(direction) > 0){
            throw std::invalid_argument("That direction is already mapped for this location.");
        }
        neighbors[direction] = location;
    }

    void Location::add_npc(NPC npc){
        npcs.push_back(npc);
    }

    std::vector<NPC> Location::get_npcs() const { return npcs;}

    void Location::add_item(Item& item){
        items.push_back(item);
    }

    std::vector<Item> Location::get_items() const { return items;}

    void Location::set_visited(){
        visited = true;
    }

    bool Location::get_visited() const {
        if (this->visited) {
            return true;
        }
        return false;
    }

    std::ostream& operator<<(std::ostream& os, const Location& location) {
        // Location name and description
        os << location.name << "- " << location.description << "\n\n";

        // List NPCs
        os << "You see the following NPCs:\n";
        if (location.npcs.empty()) {
            os << "- None\n";
        } else {
            for (const NPC& npc : location.npcs) {
                os << "- " << npc << "\n";
            }
        }

        // List items
        os << "\nYou see the following Items:\n";
        if (location.items.empty()) {
            os << "- None\n";
        } else {
            for (const Item& item : location.items) {
                os << "- " << item.getName() << " (" << item.getCalories() << " calories) - "
                   << item.getWeight() << " lb- " << item.getDescription() << "\n";
            }
        }

        // List directions
        os << "\nYou can go in the following Directions:\n";
        if (location.neighbors.empty()) {
            os << "- None\n";
        } else {
            for (const auto& neighbor : location.neighbors) {
                os << "- " << neighbor.first << "- "
                   << (neighbor.second->get_visited() ? neighbor.second->name : "Unknown")
                   << (neighbor.second->get_visited() ? " (Visited)" : "") << "\n";
            }
        }

        return os;
    }



Game::Game() {
    // Define main commands
    // commands["drink"] = std::bind(&Game::drink, this, std::placeholders::_1);
    // commands["help"] = std::bind(&Game::showHelp, this, std::placeholders::_1);
    // commands["look"] = std::bind(&Game::look, this, std::placeholders::_1);
    // commands["move"] = std::bind(&Game::move, this, std::placeholders::_1);
    // commands["quit"] = std::bind(&Game::quit, this, std::placeholders::_1);
    // commands["talk"] = std::bind(&Game::talk, this, std::placeholders::_1);

    // // Define aliases for commands
    // commandAliases["go"] = "move";
    // commandAliases["walk"] = "move";
    // commandAliases["run"] = "move";
    // commandAliases["exit"] = "quit";
    // commandAliases["speak"] = "talk";
    // commandAliases["chat"] = "talk";
    // commandAliases["view"] = "look";
    // commandAliases["observe"] = "look";
    // commandAliases["consume"] = "drink";
    // commandAliases["chug"] = "drink";
    commands = setup_commands();
    createWorld();
    currentWeight = 0;
    caloriesNeeded = 500;
    inProgress = true;
    currentLocation = randomLocation();
    drunkness = 0;
    }

    std::map<std::string, void(*)(std::vector<std::string>)> setup_commands(){
        std::map<std::string, std::function<void(*)(std::vector<std::string>)>> commands;
        commands["help"] = [this](std::vector<std::string> target) { showHelp(target); };
        commands["talk"] = [this](std::vector<std::string> target) { talk(target); };
        commands["take"] = [this](std::vector<std::string> target) { take(target); };
        commands["give"] = [this](std::vector<std::string> target) { give(target); };
        commands["go"] = [this](std::vector<std::string> target) { go(target); };
        commands["look"] = [this](std::vector<std::string> target) { look(target); };
        commands["quit"] = [this](std::vector<std::string> target) { quit(target); };

        return commands

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