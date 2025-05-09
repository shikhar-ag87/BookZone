#ifndef PROMPT_BOOK_H
#define PROMPT_BOOK_H

#include "User.h"
#include "BookShelf.h"

void promptBookAdmin(User* currentUser, Bookshelf* shelf, int* subChoice);
void promptBook(User* currentUser, Bookshelf* shelf, int* subChoice);

#endif
