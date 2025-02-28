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


	void NPC::addMessage(const std::string& message) {
    messages.push_back(message);  // Add the message to the vector
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

    std::vector<NPC> Location::get_npcs() const { return npcs ;}

    void Location::add_item(const Item& item){
        items.push_back(item);
    }

    void Location::remove_item(const Item& item) {
    auto it = std::find_if(items.begin(), items.end(),
        [&item](const Item& i) { return i.getName() == item.getName(); });
    if (it != items.end()) {
        items.erase(it);
    }
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
    commands = setup_commands();
    createWorld();
    currentWeight = 0;
    caloriesNeeded = 500;
    inProgress = true;
    currentLocation = randomLocation();
    drunkness = 0;

    if (currentLocation) {
        currentLocation->set_visited();
    }
}

std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> Game::setup_commands() {
    std::map<std::string, std::function<void(Game*, std::vector<std::string>)>> commands;
    commands.insert(std::make_pair("help", &Game::showHelp));
    commands.insert(std::make_pair("talk", &Game::talk));
    commands.insert(std::make_pair("take", &Game::take));
    commands.insert(std::make_pair("get", &Game::take));
    commands.insert(std::make_pair("give", &Game::give));
    commands.insert(std::make_pair("go", &Game::go));
    commands.insert(std::make_pair("look", &Game::look));
    commands.insert(std::make_pair("quit", &Game::quit));
    commands.insert(std::make_pair("exit", &Game::quit));
    commands.insert(std::make_pair("i", &Game::showInventory));

    return commands;
}

void Game::createWorld() {
    // Create locations and add them to the vector
    locations.push_back(Location("Kirkhoff Upstairs", "The student union. There are restaurants, a store, and places to congregate."));
    locations.push_back(Location("Kirkhoff Downstairs", "The lower level of the student union. It’s quieter here, with study areas and vending machines."));
    locations.push_back(Location("Library", "A quiet place filled with books and study spaces. The air smells like old paper."));
    locations.push_back(Location("Woods", "A dense forest behind campus. The trees are tall, and the air is fresh. This is where the elf resides."));
    locations.push_back(Location("Dormitory", "A cozy dorm room with a bed, desk, and mini-fridge. It feels like home."));
    locations.push_back(Location("Cafeteria", "A bustling area with long tables and a variety of food options. The smell of food fills the air."));
    locations.push_back(Location("Parking Lot", "A large open area filled with cars. It’s noisy and smells like gasoline."));
    locations.push_back(Location("Science Building", "A modern building with labs and classrooms. The hallways are quiet and sterile."));

    // Add items to locations
    locations[0].add_item(Item("Sandwich", "A tasty sandwich with ham and cheese.", 300, 0.5));
    locations[0].add_item(Item("Coffee", "A hot cup of coffee. It smells amazing.", 5, 0.3));

    locations[1].add_item(Item("Chips", "A bag of salty potato chips.", 150, 0.2));
    locations[1].add_item(Item("Soda", "A cold can of soda. It’s fizzy and refreshing.", 140, 0.4));

    locations[2].add_item(Item("Book", "A heavy textbook. It’s not edible.", 0, 3.0));
    locations[2].add_item(Item("Apple", "A fresh, juicy apple.", 95, 0.3));

    locations[3].add_item(Item("Mushroom", "A strange-looking mushroom. It might be poisonous.", 0, 0.1));
    locations[3].add_item(Item("Berries", "A handful of wild berries. They look delicious.", 50, 0.2));

    locations[4].add_item(Item("Pizza", "A slice of pepperoni pizza. It’s still warm.", 285, 0.6));
    locations[4].add_item(Item("Energy Drink", "A can of energy drink. It promises to keep you awake.", 110, 0.5));

    locations[5].add_item(Item("Salad", "A healthy bowl of salad with lettuce and tomatoes.", 100, 0.4));
    locations[5].add_item(Item("Bread", "A loaf of freshly baked bread.", 200, 0.8));

    locations[6].add_item(Item("Granola Bar", "A chewy granola bar. It’s packed with oats and honey.", 120, 0.2));
    locations[6].add_item(Item("Water Bottle", "A bottle of water. It’s essential for hydration.", 0, 0.5));

    locations[7].add_item(Item("Protein Bar", "A high-protein bar. It’s dense and filling.", 200, 0.3));
    locations[7].add_item(Item("Banana", "A ripe banana. It’s easy to eat on the go.", 105, 0.3));

    // Add NPCs to locations
    NPC janitor("Janitor", "A tired-looking janitor cleaning the floors.");
    janitor.addMessage("Keep it quiet down here, I need to focus.");
    janitor.addMessage("I’m just trying to get through the day.");
    locations[1].add_npc(janitor);

    NPC librarian("Librarian", "A stern librarian who shushes anyone who talks too loudly.");
    librarian.addMessage("Shh... Keep the noise down in the library.");
    librarian.addMessage("If you’re looking for a book, let me know.");
    locations[2].add_npc(librarian);

    NPC elf("Elf", "A mysterious elf who needs your help.");
    elf.addMessage("The forest whispers secrets... Are you ready to listen?");
    elf.addMessage("I can help you, but it will cost you.");
    locations[3].add_npc(elf);

    NPC roommate("Roommate", "Your friendly roommate, always ready to chat.");
    roommate.addMessage("Hey! You getting settled in?");
    roommate.addMessage("I’ve got the pizza ordered, we’re gonna have a great night.");
    locations[4].add_npc(roommate);

    NPC chef("Chef", "A cheerful chef cooking up a storm in the cafeteria.");
    chef.addMessage("The food here is great, but the secret is in the seasoning.");
    chef.addMessage("You should try the special of the day!");
    locations[5].add_npc(chef);

    NPC securityGuard("Security Guard", "A vigilant security guard patrolling the parking lot.");
    securityGuard.addMessage("Watch your step around here. I’ve seen some strange things.");
    securityGuard.addMessage("If you see anything suspicious, let me know.");
    locations[6].add_npc(securityGuard);

    NPC professor("Professor", "A busy professor rushing to their next class.");
    professor.addMessage("I’m late for my lecture, but can I help you with something?");
    professor.addMessage("You look like someone who appreciates a good experiment.");
    locations[7].add_npc(professor);

    // Connect locations using pointers
    locations[0].add_location("downstairs", &locations[1]);
    locations[1].add_location("upstairs", &locations[0]);
    locations[1].add_location("north", &locations[2]);
    locations[2].add_location("south", &locations[1]);
    locations[2].add_location("east", &locations[3]);
    locations[3].add_location("west", &locations[2]);
    locations[3].add_location("south", &locations[4]);
    locations[4].add_location("north", &locations[3]);
    locations[4].add_location("east", &locations[5]);
    locations[5].add_location("west", &locations[4]);
    locations[5].add_location("south", &locations[6]);
    locations[6].add_location("north", &locations[5]);
    locations[6].add_location("east", &locations[7]);
    locations[7].add_location("west", &locations[6]);
}



void Game::executeCommand(std::string command, std::vector<std::string> args) {
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if (commands.find(command) != commands.end()) {
        commands[command](this, args);  // Pass 'this' to refer to the current instance of the Game class
    } else {
        std::cout << "Unknown command! Type 'help' for a list of commands." << std::endl;
    }
}


//void Game::drink(std::vector<std::string> args) {
//    if (args.empty()) {
//        std::cout << "Drink what? You need to specify an item.\n";
//        return;
//    }
//
//    std::string drinkName = args[0];
//    auto it = std::find_if(inventory.begin(), inventory.end(), [&](const Item& item) {
//        return item.getName() == drinkName;
//    });
//
//    if (it == inventory.end()) {
//        std::cout << "You don't have a " << drinkName << " to drink.\n";
//        return;
//    }
//
//    // Remove the item from inventory after drinking
//    Item drink = *it;
//    inventory.erase(it);
//
//    // Increase drunkness if it's an alcoholic drink
//    if (drinkName == "Beer1" || drinkName == "Beer2" || drinkName == "Beer3") {
//        drunkness += 1;
//        std::cout << "You drink the " << drinkName << ". You feel a little tipsy. (Drunkness: " << drunkness << ")\n";
//    } else {
//        std::cout << "You drink the " << drinkName << ". Refreshing!\n";
//    }
//}

void Game::look(std::vector<std::string> args) {
    if (currentLocation) {
        std::cout << *currentLocation << std::endl;  // Dereference the pointer
    } else {
        std::cout << "You are in an unknown place..." << std::endl;
    }
}

void Game::quit(std::vector<std::string> args) { std::cout << "Quitting game..." << std::endl; exit(0); }

void Game::showHelp(std::vector<std::string> args) {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

void Game::showInventory(std::vector<std::string> args) {
    if (inventory.empty()) {
        std::cout << "Your inventory is empty.\n";
    } else {
        std::cout << "Your inventory contains:\n";
        for (const auto& item : inventory) {
            std::cout << "- " << item << std::endl;
        }
    }
}

void Game::take(std::vector<std::string> args) {
    // Check if the first argument is an article ("the" or "a")
    std::vector<std::string> articles = {"the", "a"};

    if (args.size() > 0 && std::find(articles.begin(), articles.end(), args[0]) != articles.end()) {
        args.erase(args.begin());  // Remove the article
    }

    // Join the remaining arguments into a single string for the item name
    std::string fullItemName = "";
    for (const auto& arg : args) {
        if (!fullItemName.empty()) fullItemName += " ";
        fullItemName += arg;
    }

    // Search for the item in the current location's items
    bool itemFound = false;
    for (auto& item : currentLocation->get_items()) {
        if (item.getName() == fullItemName) {
            itemFound = true;
            inventory.push_back(item);  // Add item to the inventory
            currentLocation->remove_item(item);  // Remove item from the current location
            std::cout << "You have taken the " << fullItemName << "." << std::endl;
            break;
        }
    }

    if (!itemFound) {
        std::cout << "Item not found in this location." << std::endl;
    }
}

void Game::give(std::vector<std::string> target) {
	if (target.size() < 2) {
        std::cout << "Usage: give <npc> <item>\nExample: give elf apple\n";
        return;
    }

    std::string npcName = target[0];
    std::string itemName = target[1];

    bool elfPresent = false;
    for (const NPC& npc : currentLocation->get_npcs()) {
        if (npc.getName() == "Elf") {
            elfPresent = true;
            break;
        }
    }

    // Verify we're giving to the elf in correct location
    if (!elfPresent) {
        std::cout << "You can only give items to the elf!\n";
        return;
    }

    // Find item in inventory
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&](const Item& i) { return i.getName() == itemName; });

    if (it == inventory.end()) {
        std::cout << "You don't have a " << itemName << " in your inventory.\n";
        return;
    }

    // Check if item is edible
    if (it->getCalories() <= 0) {
        std::cout << "The elf can't eat " << itemName << "!\n";
        return;
    }

    // Update calorie tracking
    int itemCalories = it->getCalories();
    caloriesNeeded = std::max(0, caloriesNeeded - itemCalories);
    inventory.erase(it);

    std::cout << "The elf ate " << itemName << " (" << itemCalories
              << " calories). Remaining needed: " << caloriesNeeded << "\n";
}


void Game::go(std::vector<std::string> args) {

  	currentLocation->set_visited(); // Mark as visited

    if (args.empty()) {
        std::cout << "Go where? Please specify a direction.\n";
        return;
    }

    std::string direction = args[0]; // Assuming the direction is the first argument

    // Check if the current location has a neighbor in that direction
    auto it = currentLocation->neighbors.find(direction);
    if (it == currentLocation->neighbors.end()) {
        std::cout << "You can't go that way.\n";
        return;
    }

    // Move to the new location
    currentLocation = it->second;


    // Print the new location details
    std::cout << *currentLocation << std::endl;
}

Location* Game::randomLocation() {
    if (locations.empty()) {
        return nullptr;  // Handle the case where the vector is empty
    }

    std::srand(std::time(nullptr));  // Seed the random number generator
    int randomIndex = std::rand() % locations.size();  // Get a random index

    return &locations[randomIndex];
}

void Game::talk(std::vector<std::string> args) {

    if (!currentLocation) {
        std::cout << "No locations available to talk to." << std::endl;
        return;
    }

    // Check if there are any NPCs in the current location
    std::vector<NPC> npcs = currentLocation->get_npcs();
    if (npcs.empty()) {
        std::cout << "There are no NPCs to talk to in this location." << std::endl;
        return;
    }

    // Handle talking to the NPCs
    if (args.empty()) {
        std::cout << "You need to specify which NPC to talk to." << std::endl;
        return;
    }

    // Find the NPC to talk to
    std::string npcName = args[0];
    for (auto& npc : npcs) {
        if (npc.getName() == npcName) {
            std::cout << "You start a conversation with " << npc.getName() << "..." << std::endl;
            std::cout << npc.getMessage() << std::endl;
            return;
        }
    }

    // If no NPC is found with the specified name
    std::cout << "No NPC named " << npcName << " in this location." << std::endl;
}

void Game::play() {

    std::cout << "Starting the game..." << std::endl;

    std::string input;
    while (true) {
      	 if (caloriesNeeded <= 0) {
        	std::cout << "\n*** The elf is satisfied! You've won the game! ***\n";
        	inProgress = false;
    	 }
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