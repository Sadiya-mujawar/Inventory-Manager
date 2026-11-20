
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <cmath>

using namespace std;

class Item {
public:
    int id;
    string name;
    int quantity;
    double price;

    Item() : id(0), quantity(0), price(0.0) {}

    Item(int i, string n, int q, double p) {
        id = i;
        name = n;
        quantity = q;
        price = p;
    }
};

// Safe integer input
int readInt(string message) {
    string input;
    int value;
    
    while (true) {
        cout << message;
        if (!getline(cin, input))
            throw runtime_error("Input ended!");

        stringstream ss(input);

        if (ss >> value && (ss >> ws).eof())
            return value;

        cout << "Invalid input! Enter a valid integer.\n";
    }
}

// Safe decimal input
double readDouble(string message) {
    string input;
    double value;

    while (true) {
        cout << message;

        if (!getline(cin, input))
            throw runtime_error("Input ended!");

        stringstream ss(input);

        if (ss >> value && (ss >> ws).eof() && isfinite(value))
            return value;

        cout << "Invalid input! Enter a valid number.\n";
    }
}

class InventoryManager {
private:
    map<int, Item> inventory;

    void saveToFile() {
        ofstream file("inventory.txt");

        if (!file) {
            throw runtime_error("Unable to save file!");
        }

        file << setprecision(17);

        for (const auto &x : inventory) {
            const Item &item = x.second;

            file << item.id << "|"
                 << item.name << "|"
                 << item.quantity << "|"
                 << item.price << "\n";
        }

        if (!file) {
            throw runtime_error("Error writing inventory file!");
        }
    }

    void loadFromFile() {
        ifstream file("inventory.txt");

        if (!file) {
            return;
        }

        string line;

        while (getline(file, line)) {
            try {
                size_t p1 = line.find('|');
                size_t p2 = line.find('|', p1 + 1);
                size_t p3 = line.find('|', p2 + 1);

                if (p1 == string::npos ||
                    p2 == string::npos ||
                    p3 == string::npos) {
                    continue;
                }

                int id = stoi(line.substr(0, p1));

                string name =
                    line.substr(p1 + 1, p2 - p1 - 1);

                int quantity =
                    stoi(line.substr(p2 + 1, p3 - p2 - 1));

                string priceText = line.substr(p3 + 1);
                size_t used = 0;
                double price = stod(priceText, &used);

                if (used != priceText.size())
                    continue;

                if (id > 0 &&
                    !name.empty() &&
                    quantity >= 0 &&
                    price >= 0 &&
                    isfinite(price)) {

                    inventory[id] = Item(id, name, quantity, price);
                }
            }
            catch (...) {
                continue;
            }
        }
    }

public:
    InventoryManager() {
        loadFromFile();
    }

    // Add new item
    void addItem() {
        int id = readInt("Enter Item ID: ");

        if (id <= 0) {
            cout << "Item ID must be positive!\n";
            return;
        }

        if (inventory.count(id)) {
            cout << "Item ID already exists!\n";
            return;
        }

        string name;

        cout << "Enter Item Name: ";

        if (!getline(cin, name))
            throw runtime_error("Input ended!");

        if (name.empty() || name.find('|') != string::npos) {
            cout << "Invalid item name!\n";
            return;
        }

        int quantity = readInt("Enter Quantity: ");
        double price = readDouble("Enter Price: ");

        if (quantity < 0 || price < 0) {
            cout << "Invalid item details!\n";
            return;
        }

        inventory[id] = Item(id, name, quantity, price);

        saveToFile();

        cout << "Item added successfully!\n";
    }

    // Search item
    void searchItem() {
        int id = readInt("Enter Item ID: ");

        auto it = inventory.find(id);

        if (it != inventory.end()) {
            const Item &item = it->second;

            cout << "\nItem ID: " << item.id;
            cout << "\nName: " << item.name;
            cout << "\nQuantity: " << item.quantity;
            cout << "\nPrice: " << item.price << endl;
        }
        else {
            cout << "Item not found!\n";
        }
    }

    // Display all items
    void displayItems() {
        if (inventory.empty()) {
            cout << "Inventory is empty!\n";
            return;
        }

        cout << "\n";

        cout << left << setw(10) << "ID"
             << setw(20) << "Name"
             << setw(12) << "Quantity"
             << setw(10) << "Price" << endl;

        cout << "------------------------------------------------\n";

        for (const auto &x : inventory) {
            const Item &item = x.second;

            cout << left << setw(10) << item.id
                 << setw(20) << item.name
                 << setw(12) << item.quantity
                 << setw(10) << item.price << endl;
        }
    }

    // Update item
    void updateItem() {
        int id = readInt("Enter Item ID: ");

        auto it = inventory.find(id);

        if (it == inventory.end()) {
            cout << "Item not found!\n";
            return;
        }

        string name;

        cout << "Enter New Name: ";

        if (!getline(cin, name))
            throw runtime_error("Input ended!");

        double price = readDouble("Enter New Price: ");

        if (name.empty() ||
            name.find('|') != string::npos ||
            price < 0) {
            cout << "Invalid details!\n";
            return;
        }

        // Update only after validation
        it->second.name = name;
        it->second.price = price;

        saveToFile();

        cout << "Item updated successfully!\n";
    }

    // Add stock
    void stockIn() {
        int id = readInt("Enter Item ID: ");

        auto it = inventory.find(id);

        if (it == inventory.end()) {
            cout << "Item not found!\n";
            return;
        }

        int quantity = readInt("Enter Stock Quantity: ");

        if (quantity <= 0 ||
            it->second.quantity >
            numeric_limits<int>::max() - quantity) {

            cout << "Invalid quantity!\n";
            return;
        }

        it->second.quantity += quantity;

        saveToFile();

        cout << "Stock added successfully!\n";
    }

    // Remove stock
    void stockOut() {
        int id = readInt("Enter Item ID: ");

        auto it = inventory.find(id);

        if (it == inventory.end()) {
            cout << "Item not found!\n";
            return;
        }

        int quantity = readInt("Enter Quantity to Remove: ");

        if (quantity <= 0) {
            cout << "Invalid quantity!\n";
            return;
        }

        if (quantity > it->second.quantity) {
            cout << "Insufficient stock!\n";
            return;
        }

        it->second.quantity -= quantity;

        saveToFile();

        cout << "Stock removed successfully!\n";
    }
};

int main() {
    try {
        InventoryManager manager;
        int choice;

        do {
            cout << "\n===== INVENTORY MANAGEMENT SYSTEM =====\n";
            cout << "1. Add Item\n";
            cout << "2. Search Item\n";
            cout << "3. Display Items\n";
            cout << "4. Update Item\n";
            cout << "5. Stock In\n";
            cout << "6. Stock Out\n";
            cout << "7. Exit\n";

            choice = readInt("\nEnter your choice: ");

            switch (choice) {
                case 1:
                    manager.addItem();
                    break;

                case 2:
                    manager.searchItem();
                    break;

                case 3:
                    manager.displayItems();
                    break;

                case 4:
                    manager.updateItem();
                    break;

                case 5:
                    manager.stockIn();
                    break;

                case 6:
                    manager.stockOut();
                    break;

                case 7:
                    cout << "Thank you for using Inventory Manager!\n";
                    break;

                default:
                    cout << "Invalid choice! Try again.\n";
            }

        } while (choice != 7);
    }
    catch (const exception &e) {
        cout << "\nError: " << e.what() << endl;
    }

    return 0;
}
