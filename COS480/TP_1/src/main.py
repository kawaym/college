from fetch import dataset
from record import save_fixed_record, save_variable_record, read_fixed_record
from disk import start_db, TABLE_HEADER, read_header_from_json, write_header_to_json
from constants import MAX_SIZES, TEST_SINGLE_INSERT, TEST_MANY_INSERT
from fixed_heap_sql import insert_one, insert_many, delete_by_index, select_one, select_many_by_array, select_many_by_interval, select_many_by_field

table = TABLE_HEADER(table_name="Digimons", record_size=71)

FILE_INFO = {
    "PATH": "../data/",
    "NAME": "records",
}

file_path = FILE_INFO["PATH"] + FILE_INFO["NAME"]

table = read_header_from_json()
write_header_to_json(table)

start_db(name=FILE_INFO['NAME'], path=FILE_INFO['PATH'], table=table)
# insert_many(dataset, file_path)
print(select_many_by_field("Punimon", file_path))
# dataset.apply(lambda row: save_fixed_record(row, file), axis=1)
# dataset.apply(lambda row: save_variable_record(row, file), axis=1)