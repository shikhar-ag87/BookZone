#include <iostream>
#include <string>

#include "main.h"

using namespace std;

Library* gLibrary = nullptr;

int main() {
    Library library;
    gLibrary = &library;
    int choice, id;
    string title, author, shelfName;

    UserManager userManager;
    User* currentUser = nullptr;

    do {
        bool isExit = promptUser(&library, &userManager, &currentUser);
        if (isExit) return 0;

        // Add menu for top borrowed books
        int showTop;
        cout << "Show top N most borrowed books? (1 = Yes, 0 = No): ";
        cin >> showTop;
        if (showTop == 1) {
            int N;
            cout << "Enter N: ";
            cin >> N;
            library.displayTopBorrowedBooks(N);
        }
    } while (!currentUser);

    library.saveToFile();
    userManager.saveToFile();
    return 0;
}