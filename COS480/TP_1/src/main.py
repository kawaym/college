from disk import start_db, load_db_data
from definitions import TABLE_HEADER

table = TABLE_HEADER(table_name="Digimons", record_size=71)

start_db()
load_db_data("digimons")