#include <goose/imdb/imdb.h>
#include <goose/imdb/imdb_constants.h>
#include <goose/common/file_system.h>


using namespace goose;
using namespace std;

namespace imdb {

void dbgen(Goose &db) {
	Connection con(db);
	con.Query("BEGIN TRANSACTION");
	for (int t = 0; t < IMDB_TABLE_COUNT; t++) {
		con.Query(IMDB_TABLE_DDL[t]);
		string table_name = string(IMDB_TABLE_NAMES[t]);
		string data_file_name = "third_party/imdb/data/"+table_name+".csv.gz";
		auto file_system = FileSystem::CreateLocal();
		if (!file_system->FileExists(data_file_name)) {
			throw InvalidInputException("IMDB data file missing, try `make imdb` to download.");
		}
		con.Query("COPY "+table_name+" FROM '"+data_file_name+"' DELIMITER ',' ESCAPE '\\';");
	}
	con.Query("COMMIT");
}

string get_query(int query) {
	if (query <= 0 || query > IMDB_QUERIES_COUNT) {
		throw SyntaxException("Out of range IMDB query number %d", query);
	}
	return IMDB_QUERIES[query - 1];
}
} // namespace imdb
