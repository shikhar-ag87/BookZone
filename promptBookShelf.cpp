#include <iostream>

#include "promptBookShelf.h"

using namespace std;

// Helper function for autocompletion
void showBookshelfAutocomplete(Library* library, const std::string& partial) {
    bool found = false;
    for (const auto& pair : library->getBookshelves()) {
        if (pair.first.find(partial) == 0) {
            std::cout << "  " << pair.first << std::endl;
            found = true;
        }
    }
    if (!found) std::cout << "  (no matches)\n";
}

void promptBookShelfAdmin(Library* library, User** currentUser, int* choice) {
    string shelfName;
    cout << "\n\033[36m===== 📖 Library Menu =====\033[0m\n";
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
        while (true) {
            cout << "Enter Bookshelf Name (or type 'cancel' to abort, or press TAB for suggestions): ";
            getline(cin, shelfName);
            if (shelfName == "\t") {
                cout << "\033[36mSuggestions:\033[0m\n";
                showBookshelfAutocomplete(library, "");
                continue;
            }
            if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
                cout << "\033[33mOperation cancelled.\033[0m\n";
                break;
            }
            library->addBookshelf(shelfName);
            break;
        }
        break;

    case 2:
        while (true) {
            cout << "Enter Bookshelf Name to delete (or type 'cancel' to abort, or press TAB for suggestions): ";
            getline(cin, shelfName);
            if (shelfName == "\t") {
                cout << "\033[36mSuggestions:\033[0m\n";
                showBookshelfAutocomplete(library, "");
                continue;
            }
            if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
                cout << "\033[33mOperation cancelled.\033[0m\n";
                break;
            }
            library->removeBookshelf(shelfName);
            break;
        }
        break;

    case 3:
        library->displayBookshelves();
        break;

    case 4: {
        while (true) {
            cout << "Enter Bookshelf Name (or type 'cancel' to abort, or press TAB for suggestions): ";
            getline(cin, shelfName);
            if (shelfName == "\t") {
                cout << "\033[36mSuggestions:\033[0m\n";
                showBookshelfAutocomplete(library, "");
                continue;
            }
            if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
                cout << "\033[33mOperation cancelled.\033[0m\n";
                break;
            }
            Bookshelf* shelf = library->getBookshelf(shelfName);
            if (shelf) {
                cout << "\033[32mSelected Bookshelf: " << shelf->getName() << "\033[0m\n";
                int subChoice;
                do {
                    promptBookAdmin(*currentUser, shelf, &subChoice);
                } while (subChoice != 8);
                break;
            }
            else {
                cout << "\033[31m❌ Bookshelf not found.\033[0m\n";
            }
        }
        break;
    }

    case 5:
        (*currentUser) = nullptr;
        break;

    default:
        cout << "\033[31m❗ Invalid choice. Try again.\033[0m\n";
    }
}

void promptBookShelf(Library* library, User** currentUser, int* choice) {
    string shelfName;
    int id;
    cout << "\n\033[36m===== 📖 Library Menu =====\033[0m\n";
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
        while (true) {
            cout << "Enter Bookshelf Name (or type 'cancel' to abort, or press TAB for suggestions): ";
            getline(cin, shelfName);
            if (shelfName == "\t") {
                cout << "\033[36mSuggestions:\033[0m\n";
                showBookshelfAutocomplete(library, "");
                continue;
            }
            if (shelfName == "cancel" || shelfName == "CANCEL" || shelfName == "Cancel") {
                cout << "\033[33mOperation cancelled.\033[0m\n";
                break;
            }
            Bookshelf* shelf = library->getBookshelf(shelfName);
            if (shelf) {
                cout << "\033[32mSelected Bookshelf: " << shelf->getName() << "\033[0m\n";
                int subChoice;
                do {
                    promptBook(*currentUser, shelf, &subChoice);
                } while (subChoice != 3);
                break;
            }
            else {
                cout << "\033[31m❌ Bookshelf not found.\033[0m\n";
            }
        }
        break;
    }

    case 3:
        (*currentUser) = nullptr;
        break;

    default:
        cout << "\033[31m❗ Invalid choice. Try again.\033[0m\n";
    }
}