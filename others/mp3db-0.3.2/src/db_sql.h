#ifndef _DB_SQL_H
#define _DB_SQL_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../src/mp3scan.h"

bool sql_setup();
bool sql_finish();
bool sql_commit_mp3(const struct mp3data *);
#endif
