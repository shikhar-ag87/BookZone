#include <iostream>
#include <string>
#include "promptUser.h"

using namespace std;


int promptUser(Library* library, UserManager* userManager, User** currentUser) {
    int authChoice;
    cout << "\n\033[36m=== Welcome ===\033[0m\n";
    cout << "1. Register\n";
    cout << "2. Login\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
    cin >> authChoice;

    switch (authChoice) {
    case 1:
        userManager->registerUser();
        break;
    case 2:
        *currentUser = userManager->loginUser();
        break;
    case 5:
        break;
    case 3:
        // *currentUser = nullptr;
        cout << "\033[35m👋 Exiting program.\033[0m\n";
        return 1;
        break;
    default:
        cout << "\033[31m❗ Invalid choice.\033[0m\n";
    }

    // Prompt for top N most borrowed books after login/registration
    if (*currentUser) {
    }

    if (*currentUser) {
        bool isAdmin = (*currentUser)->isAdmin;
        int choice;
        if (isAdmin) {
            do {
                promptBookShelfAdmin(library, currentUser, &choice);
            } while (*currentUser != nullptr);
        }
        else {
            do {
                promptBookShelf(library, currentUser, &choice);
            } while (*currentUser != nullptr);
        }

    }
    return 0;
}