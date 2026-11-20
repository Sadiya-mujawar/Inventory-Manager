#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <iomanip>
#include <limits>
#include <stdexcept>
using namespace std;

class Item {
public:
    int id;
    string name;
    int quantity;
    double price;

    Item() {}

    Item(int i, string n, int q, double p) {
        id = i;
        name = n;
        quantity = q;
        price = p;
    }
};

class InventoryManager {
private:
    map<int, Item> inventory;

    void saveToFile() {
        ofstream file("inventory.txt");

        if (!file) {
            throw runtime_error("Unable to save file!");
        }

        for (auto &x : inventory) {
            Item item = x.second;

            file << item.id << "|"
                 << item.name << "|"
                 << item.quantity << "|"
                 << item.price << endl;
        }

        file.close();
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
                string name = line.substr(p1 + 1, p2 - p1 - 1);
                int quantity = stoi(line.substr(p2 + 1, p3 - p2 - 1));
                double price = stod(line.substr(p3 + 1));

                if (quantity >= 0 && price >= 0) {
                    inventory[id] = Item(id, name, quantity, price);
                }
            }
            catch (...) {
                continue;
            }
        }

        file.close();
    }

public:
    InventoryManager() {
        loadFromFile();
    }

    void addItem() {
        int id, quantity;
        string name;
        double price;

        cout << "Enter Item ID: ";
        cin >> id;

        if (inventory.count(id)) {
            cout << "Item ID already exists!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter Item Name: ";
        getline(cin, name);

        cout << "Enter Quantity: ";
        cin >> quantity;

        cout << "Enter Price: ";
        cin >> price;

        if (quantity < 0 || price < 0 || name.empty()) {
            cout << "Invalid item details!\n";
            return;
        }

        inventory[id] = Item(id, name, quantity, price);

        saveToFile();

        cout << "Item added successfully!\n";
    }

    void searchItem() {
        int id;

        cout << "Enter Item ID: ";
        cin >> id;

        if (inventory.count(id)) {
            Item item = inventory[id];

            cout << "\nItem ID: " << item.id;
            cout << "\nName: " << item.name;
            cout << "\nQuantity: " << item.quantity;
            cout << "\nPrice: " << item.price << endl;
        }
        else {
            cout << "Item not found!\n";
        }
    }

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

        for (auto &x : inventory) {
            Item item = x.second;

            cout << left << setw(10) << item.id
                 << setw(20) << item.name
                 << setw(12) << item.quantity
                 << setw(10) << item.price << endl;
        }
    }

    void updateItem() {
        int id;

        cout << "Enter Item ID: ";
        cin >> id;

        if (!inventory.count(id)) {
            cout << "Item not found!\n";
            return;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << "Enter New Name: ";
        getline(cin, inventory[id].name);

        cout << "Enter New Price: ";
        double price;
        cin >> price;

        if (price < 0 || inventory[id].name.empty()) {
            cout << "Invalid details!\n";
            return;
        }

        inventory[id].price = price;

        saveToFile();

        cout << "Item updated successfully!\n";
    }

    void stockIn() {
        int id, quantity;

        cout << "Enter Item ID: ";
        cin >> id;

        if (!inventory.count(id)) {
            cout << "Item not found!\n";
            return;
        }

        cout << "Enter Stock Quantity: ";
        cin >> quantity;

        if (quantity <= 0 ||
            inventory[id].quantity > numeric_limits<int>::max() - quantity) {
            cout << "Invalid quantity!\n";
            return;
        }

        inventory[id].quantity += quantity;

        saveToFile();

        cout << "Stock added successfully!\n";
    }

    void stockOut() {
        int id, quantity;

        cout << "Enter Item ID: ";
        cin >> id;

        if (!inventory.count(id)) {
            cout << "Item not found!\n";
            return;
        }

        cout << "Enter Quantity to Remove: ";
        cin >> quantity;

        if (quantity <= 0) {
            cout << "Invalid quantity!\n";
            return;
        }

        if (quantity > inventory[id].quantity) {
            cout << "Insufficient stock!\n";
            return;
        }

        inventory[id].quantity -= quantity;

        saveToFile();

        cout << "Stock removed successfully!\n";
    }
};

int main() {
    InventoryManager manager;
    int choice;

    try {
        do {
            cout << "\n===== INVENTORY MANAGEMENT SYSTEM =====\n";
            cout << "1. Add Item\n";
            cout << "2. Search Item\n";
            cout << "3. Display Items\n";
            cout << "4. Update Item\n";
            cout << "5. Stock In\n";
            cout << "6. Stock Out\n";
            cout << "7. Exit\n";

            cout << "\nEnter your choice: ";
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input!\n";
                continue;
            }

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
                    cout << "Invalid choice!\n";
            }

        } while (choice != 7);
    }
    catch (const exception &e) {
        cout << "Error: " << e.what() << endl;
    }

    return 0;
}