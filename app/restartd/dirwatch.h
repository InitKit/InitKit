#ifndef DIRWATCH_H_
#define DIRWATCH_H_

#include "list.h"

typedef struct DirWatch DirWatch;

ListGenForNameType (dirwatch, DirWatch);

typedef struct DirWatch
{
    int id;
    void * userData;
    void (*cb) (void *, long);
} DirWatch;

/* Find a directory watcher by its ID.
 * Returns 0 if no such directory watcher is found. */
DirWatch * dirwatch_find (int id);

/* Deletes a directory watcher by its file descriptor. */
void dirwatch_del (int id);

/* Installs a directory watcher for a path.
 * Returns the file descriptor, which acts as the identifier for the
 * dirwatch. */
int dirwatch_add (const char * path, void * userData,
                  void (*cb) (void *, long));

#endif
