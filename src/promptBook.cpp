#include <iostream>
#include <sstream>
#include <vector>

#include "promptBook.h"

using namespace std;

void promptBookAdmin(User* currentUser, Bookshelf* shelf, int* subChoice) {
    cout << "\n===== Bookshelf Menu =====\n";
    cout << "1. Add Book\n";
    cout << "2. Display All Books\n";
    cout << "3. Search Book by ID\n";
    cout << "4. Delete Book by ID\n";
    cout << "5. Borrow Book by ID\n";
    cout << "6. Return Book by ID\n";
    cout << "7. Get Book Recommendations\n";
    cout << "8. Exit Bookshelf Menu\n";

    cout << "Enter your choice: ";
    cin >> *subChoice;
    cin.ignore();

    switch (*subChoice) {
    case 1: {
        string title, author;
        vector<string> genres;
        cout << "Enter Title (or type 'cancel' to abort): ";
        getline(cin, title);
        if (title == "cancel" || title == "CANCEL" || title == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        cout << "Enter Author (or type 'cancel' to abort): ";
        getline(cin, author);
        if (author == "cancel" || author == "CANCEL" || author == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        cout << "Enter genres (separate by comma, press enter when done, or type 'cancel' to abort): ";
        string genreInput;
        getline(cin, genreInput);
        if (genreInput == "cancel" || genreInput == "CANCEL" || genreInput == "Cancel") {
            cout << "Operation cancelled.\n";
            break;
        }
        stringstream ss(genreInput);
        string genre;
        while (getline(ss, genre, ',')) {
            genres.push_back(genre);
        }
        shelf->addBookWithGenres(title, author, genres);
        break;
    }

    case 2:
        shelf->displayBooks();
        break;

    case 3: {
        string input;
        cout << "Enter Book ID to search (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        shelf->searchBook(bookId);
        cin.ignore();
        break;
    }

    case 4: {
        string input;
        cout << "Enter Book ID to delete (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        shelf->deleteBook(bookId);
        cin.ignore();
        break;
    }

    case 5: {
        string input;
        cout << "Enter book ID to borrow (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        currentUser->borrowBook(shelf, bookId);
        cin.ignore();
        break;
    }

    case 6: {
        string input;
        cout << "Enter book ID to return (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        currentUser->returnBook(shelf, bookId);
        cin.ignore();
        break;
    }

    case 7: {
        string input;
        cout << "Enter Book ID to get recommendations (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        vector<Book*> recommendations = shelf->getRecommendations(bookId);
        if (!recommendations.empty()) {
            cout << "\n📚 Recommended Books:\n";
            for (Book* book : recommendations) {
                cout << "🔸 ID: " << book->getId()
                    << ", Title: " << book->getTitle()
                    << ", Author: " << book->getAuthor() << endl;
            }
        }
        else {
            cout << "⚠️  No recommendations found.\n";
        }
        cin.ignore();
        break;
    }

    case 8:
        cout << "Returning to main menu.\n";
        break;

    default:
        cout << "❗ Invalid choice. Try again.\n";
    }
}

void promptBook(User* currentUser, Bookshelf* shelf, int* subChoice) {
    cout << "\n===== Bookshelf Menu =====\n";
    cout << "1. Display All Books\n";
    cout << "2. Search Book by ID\n";
    cout << "3. Borrow Book by ID\n";
    cout << "4. Return Book by ID\n";
    cout << "5. Exit Bookshelf Menu\n";

    cout << "Enter your choice: ";
    cin >> *subChoice;
    cin.ignore();

    switch (*subChoice) {
    case 1:
        shelf->displayBooks();
        break;

    case 2: {
        string input;
        cout << "Enter Book ID to search (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int id = stoi(input);
        shelf->searchBook(id);
        cin.ignore();
        break;
    }

    case 3: {
        string input;
        cout << "Enter book ID to borrow (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        currentUser->borrowBook(shelf, bookId);
        cin.ignore();
        break;
    }

    case 4: {
        string input;
        cout << "Enter book ID to return (or type 'cancel' to abort): ";
        cin >> input;
        if (input == "cancel" || input == "CANCEL" || input == "Cancel") {
            cout << "Operation cancelled.\n";
            cin.ignore();
            break;
        }
        int bookId = stoi(input);
        currentUser->returnBook(shelf, bookId);
        cin.ignore();
        break;
    }

    case 5:
        cout << "Returning to main menu.\n";
        break;

    default:
        cout << "❗ Invalid choice. Try again.\n";
    }
}