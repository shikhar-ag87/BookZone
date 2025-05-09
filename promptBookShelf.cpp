#include <iostream>

#include "promptBookShelf.h"

using namespace std;


void promptBookShelfAdmin(Library* library, User** currentUser, int* choice) {
    string shelfName;
    cout << "\n===== 📖 Library Menu =====\n";
    cout << "1. Add Bookshelf\n";
    cout << "2. Delete Bookshelf\n";
    cout << "3. Display All Bookshelves\n";
    cout << "4. Select Bookshelf\n";
    cout << "5. Exit\n";

    cout << "Enter your choice: ";
    cin >> *choice;
    cin.ignore();  // clear newline from input buffer

    switch (*choice) {
    case 1:
        cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
        getline(cin, shelfName);
        if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        library->addBookshelf(shelfName);
        break;

    case 2:
        cout << "Enter Bookshelf Name to delete (or type 'cancel' to abort): ";
        getline(cin, shelfName);
        if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        library->removeBookshelf(shelfName);
        break;
    case 3:
        library->displayBookshelves();
        break;

    case 4: {
        cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
        getline(cin, shelfName);
        if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        Bookshelf* shelf = library->getBookshelf(shelfName);
        if (shelf) {
            cout << "Selected Bookshelf: " << shelf->getName() << "\n";
            int subChoice;
            do {
                promptBookAdmin(*currentUser, shelf, &subChoice);
            } while (subChoice != 8);
        }
        else {
            cout << "❌ Bookshelf not found.\n";
        }
        break;
    }

    case 5:
        (*currentUser) = nullptr;
        break;

    default:
        cout << "❗ Invalid choice. Try again.\n";
    }
}


void promptBookShelf(Library* library, User** currentUser, int* choice) {
    string shelfName;
    int id;
    cout << "\n===== 📖 Library Menu =====\n";
    cout << "1. Display All Bookshelves\n";
    cout << "2. Select Bookshelf\n";
    cout << "3. Exit\n";

    cout << "Enter your choice: ";
    cin >> *choice;
    cin.ignore();  // clear newline from input buffer

    switch (*choice) {
    case 1:
        library->displayBookshelves();
        break;

    case 2: {
        cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
        getline(cin, shelfName);
        if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        Bookshelf* shelf = library->getBookshelf(shelfName);
        if (shelf) {
            cout << "Selected Bookshelf: " << shelf->getName() << "\n";
            int subChoice;
            do {
                promptBook(*currentUser, shelf, &subChoice);
            } while (subChoice != 3);
        }
        else {
            cout << "❌ Bookshelf not found.\n";
        }
        break;
    }

    case 3:
        (*currentUser) = nullptr;
        break;
    default:
        cout << "❗ Invalid choice. Try again.\n";
    }
}