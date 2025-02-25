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

    void Location::add_location(const std::string& direction, Location* location){
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

    void Location::add_item(const Item& item){
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
    // currentLocation = randomLocation();
    drunkness = 0;
    }

    std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> Game::setup_commands(){
        std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> commands;
        commands.insert(std::make_pair("help", &Game::showHelp));
        commands.insert(std::make_pair("talk", &Game::talk));
        commands.insert(std::make_pair("meet", &Game::meet));
        commands.insert(std::make_pair("take", &Game::take));
        commands.insert(std::make_pair("give", &Game::give));
        commands.insert(std::make_pair("go", &Game::go));
        commands.insert(std::make_pair("look", &Game::look));

        return commands;

    }


    void Game::createWorld() {
    // Create locations
    Location* kirkhoffUpstairs = new Location("Kirkhoff Upstairs", "The student union. There are restaurants, a store, and places to congregate.");
    Location* kirkhoffDownstairs = new Location("Kirkhoff Downstairs", "The lower level of the student union. It’s quieter here, with study areas and vending machines.");
    Location* library = new Location("Library", "A quiet place filled with books and study spaces. The air smells like old paper.");
    Location* woods = new Location("Woods", "A dense forest behind campus. The trees are tall, and the air is fresh. This is where the elf resides.");
    Location* dormitory = new Location("Dormitory", "A cozy dorm room with a bed, desk, and mini-fridge. It feels like home.");
    Location* cafeteria = new Location("Cafeteria", "A bustling area with long tables and a variety of food options. The smell of food fills the air.");
    Location* parkingLot = new Location("Parking Lot", "A large open area filled with cars. It’s noisy and smells like gasoline.");
    Location* scienceBuilding = new Location("Science Building", "A modern building with labs and classrooms. The hallways are quiet and sterile.");

    // Add items to locations
    kirkhoffUpstairs->add_item(Item("Sandwich", "A tasty sandwich with ham and cheese.", 300, 0.5));
    kirkhoffUpstairs->add_item(Item("Coffee", "A hot cup of coffee. It smells amazing.", 5, 0.3));

    kirkhoffDownstairs->add_item(Item("Chips", "A bag of salty potato chips.", 150, 0.2));
    kirkhoffDownstairs->add_item(Item("Soda", "A cold can of soda. It’s fizzy and refreshing.", 140, 0.4));

    library->add_item(Item("Book", "A heavy textbook. It’s not edible.", 0, 3.0));
    library->add_item(Item("Apple", "A fresh, juicy apple.", 95, 0.3));

    woods->add_item(Item("Mushroom", "A strange-looking mushroom. It might be poisonous.", 0, 0.1));
    woods->add_item(Item("Berries", "A handful of wild berries. They look delicious.", 50, 0.2));

    dormitory->add_item(Item("Pizza", "A slice of pepperoni pizza. It’s still warm.", 285, 0.6));
    dormitory->add_item(Item("Energy Drink", "A can of energy drink. It promises to keep you awake.", 110, 0.5));

    cafeteria->add_item(Item("Salad", "A healthy bowl of salad with lettuce and tomatoes.", 100, 0.4));
    cafeteria->add_item(Item("Bread", "A loaf of freshly baked bread.", 200, 0.8));

    parkingLot->add_item(Item("Granola Bar", "A chewy granola bar. It’s packed with oats and honey.", 120, 0.2));
    parkingLot->add_item(Item("Water Bottle", "A bottle of water. It’s essential for hydration.", 0, 0.5));

    scienceBuilding->add_item(Item("Protein Bar", "A high-protein bar. It’s dense and filling.", 200, 0.3));
    scienceBuilding->add_item(Item("Banana", "A ripe banana. It’s easy to eat on the go.", 105, 0.3));

    // Add NPCs to locations
    kirkhoffDownstairs->add_npc(NPC("Janitor", "A tired-looking janitor cleaning the floors."));
    library->add_npc(NPC("Librarian", "A stern librarian who shushes anyone who talks too loudly."));
    woods->add_npc(NPC("Elf", "A mysterious elf who needs your help."));
    dormitory->add_npc(NPC("Roommate", "Your friendly roommate, always ready to chat."));
    cafeteria->add_npc(NPC("Chef", "A cheerful chef cooking up a storm in the cafeteria."));
    parkingLot->add_npc(NPC("Security Guard", "A vigilant security guard patrolling the parking lot."));
    scienceBuilding->add_npc(NPC("Professor", "A busy professor rushing to their next class."));

    // Connect locations
    kirkhoffUpstairs->add_location("downstairs", kirkhoffDownstairs);
    kirkhoffDownstairs->add_location("upstairs", kirkhoffUpstairs);
    kirkhoffDownstairs->add_location("north", library);
    library->add_location("south", kirkhoffDownstairs);
    library->add_location("east", woods);
    woods->add_location("west", library);
    woods->add_location("south", dormitory);
    dormitory->add_location("north", woods);
    dormitory->add_location("east", cafeteria);
    cafeteria->add_location("west", dormitory);
    cafeteria->add_location("south", parkingLot);
    parkingLot->add_location("north", cafeteria);
    parkingLot->add_location("east", scienceBuilding);
    scienceBuilding->add_location("west", parkingLot);

    // Add locations to the world
    locations.push_back(*kirkhoffUpstairs);
    locations.push_back(*kirkhoffDownstairs);
    locations.push_back(*library);
    locations.push_back(*woods);
    locations.push_back(*dormitory);
    locations.push_back(*cafeteria);
    locations.push_back(*parkingLot);
    locations.push_back(*scienceBuilding);
}



void Game::executeCommand(std::string command, std::vector<std::string> args) {
     std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (commands.find(command) != commands.end()) {
        commands[command](args);
    } else {
        std::cout << "Unknown command! Type 'help' for a list of commands." << std::endl;
    }
}

// Dummy implementation lambdas for now
void Game::drink(std::vector<std::string> args) { std::cout << "You take a sip... or maybe chug it all!" << std::endl; }
void Game::look(std::vector<std::string> args) { std::cout << "Looking around..." << std::endl; }
// void Game::move(std::vector<std::string> args) { std::cout << "You move to a new location." << std::endl; }
// void Game::createWorld() { std::cout << "Creating the world..." << std::endl; }
void Game::quit(std::vector<std::string> args) { std::cout << "Quitting game..." << std::endl; exit(0); }
void Game::showHelp(std::vector<std::string> args) {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}
void Game::talk(std::vector<std::string> args) { std::cout << "You start a conversation..." << std::endl; }
void Game::meet(std::vector<std::string> target) { std::cout << "You start a conversation..." << std::endl; }
void Game::take(std::vector<std::string> target) { std::cout << "You start a conversation..." << std::endl; }
void Game::give(std::vector<std::string> target) { std::cout << "You start a conversation..." << std::endl; }
void Game::go(std::vector<std::string> target) { std::cout << "You start a conversation..." << std::endl; }
void Game::showItems(std::vector<std::string> target) { std::cout << "You start a conversation..." << std::endl; }
// Location* Game::randomLocation() { std::cout << "You start a conversation..." << std::endl; }

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