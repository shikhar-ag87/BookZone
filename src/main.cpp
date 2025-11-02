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
    } while (!currentUser);

    library.saveToFile();
    userManager.saveToFile();
    return 0;
}