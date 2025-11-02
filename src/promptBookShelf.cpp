#include <iostream>
#include <limits>
#include <algorithm> // for std::transform
#include "promptBookShelf.h"

using namespace std;

// Helper function to convert a string to lowercase
static std::string toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), ::tolower);
    return out;
}

void promptBookShelfAdmin(Library* library, User** currentUser, int* choice) {
    string shelfName;
    int showTopN;
    cout << "\n\033[36m===== 📖 Library Menu =====\033[0m\n";
    cout << "1. Add Bookshelf\n";
    cout << "2. Delete Bookshelf\n";
    cout << "3. Display All Bookshelves\n";
    cout << "4. Select Bookshelf\n";
    cout << "5. N most borrowed books\n";
    cout << "6. Exit\n";

    cout << "Enter your choice: ";
    cin >> *choice;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clear input buffer

    switch (*choice) {
    case 1:
        while (true) {
            cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
            getline(cin, shelfName);
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
            cout << "Enter Bookshelf Name to delete (or type 'cancel' to abort): ";
            getline(cin, shelfName);
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
            cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
            getline(cin, shelfName);
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
        int n;
        cout << "Enter N: ";
        cin >> n;
        library->displayTopBorrowedBooks(n);
        break;
    case 6:
        (*currentUser) = nullptr;
        break;

    default:
        cout << "\033[31m❗ Invalid choice. Try again.\033[0m\n";
    }
}

void promptBookShelf(Library* library, User** currentUser, int* choice) {
    string shelfName;
    int showTopN;
    int id;
    cout << "\n\033[36m===== 📖 Library Menu =====\033[0m\n";
    cout << "1. Display All Bookshelves\n";
    cout << "2. Select Bookshelf\n";
    cout << "3. N most borrowed books\n";
    cout << "4. Exit\n";

    cout << "Enter your choice: ";
    cin >> *choice;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clear input buffer

    switch (*choice) {
    case 1:
        library->displayBookshelves();
        break;

    case 2: {
        while (true) {
            cout << "Enter Bookshelf Name (or type 'cancel' to abort): ";
            getline(cin, shelfName);
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
        int n;
        cout << "Enter N: ";
        cin >> n;
        library->displayTopBorrowedBooks(n);
        break;

    case 4:
        (*currentUser) = nullptr;
        break;

    default:
        cout << "\033[31m❗ Invalid choice. Try again.\033[0m\n";
    }
}