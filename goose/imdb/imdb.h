#pragma once

#include <goose/catalog/catalog.h>
#include <goose/goose.h>

namespace imdb {
//! Adds the IMDB tables to the database
void dbgen(goose::Goose &database);

//! Gets the specified IMDB JOB Query number as a string
std::string get_query(int query);

} // namespace imdb
