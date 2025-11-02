#ifndef PROMPT_BOOKSHELF_H
#define PROMPT_BOOKSHELF_H

#include "Library.h"
#include "promptBook.h"
#include "User.h"

void promptBookShelfAdmin(Library* library, User** currentUser, int* choice);
void promptBookShelf(Library* library, User** currentUser, int* choice);

#endif
