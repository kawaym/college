from fetch import dataset
from record import save_fixed_record, save_variable_record, read_fixed_record
from disk import start_db, close_db, TABLE_HEADER, read_header_from_json, write_header_to_json
from constants import MAX_SIZES, TEST_SINGLE_INSERT, TEST_MANY_INSERT
from fixed_heap_sql import insert_one, insert_many

table = TABLE_HEADER(table_name="Digimons", record_size=71)

FILE_INFO = {
    "PATH": "../data/",
    "NAME": "records",
}

table = read_header_from_json()
write_header_to_json(table)

file, inUse = start_db(name=FILE_INFO['NAME'], path=FILE_INFO['PATH'], table=table).values()
insert_one(TEST_SINGLE_INSERT, file)
insert_many(dataset.iloc[1:5], file)
insert_many(TEST_MANY_INSERT, file)
# dataset.apply(lambda row: save_fixed_record(row, file), axis=1)
# dataset.apply(lambda row: save_variable_record(row, file), axis=1)


close_db(file)