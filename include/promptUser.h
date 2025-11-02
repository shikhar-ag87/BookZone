#ifndef PROMPT_USER_H
#define PROMPT_USER_H

#include "Library.h"
#include "UserManager.h"
#include "User.h"
#include "promptBookShelf.h"

int promptUser(Library* library, UserManager* userManager, User** currentUser);

#endif
