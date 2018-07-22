#ifndef _DB_PGSQL_H
#define _DB_PGSQL_H

#include "/usr/include/pgsql/libpq-fe.h"
#include <unistd.h>
#include <iostream>
#include <string>
#include "../src/mp3scan.h"

bool pg_setup();
bool pg_finish();
bool pg_commit_mp3(const struct mp3data *);

#endif
