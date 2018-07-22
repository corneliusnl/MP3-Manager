#ifndef _DB_RAW_H
#define _DB_RAW_H

#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include "../src/mp3scan.h"

bool raw_setup();
bool raw_finish();
bool raw_commit_mp3(const struct mp3data *);
#endif
