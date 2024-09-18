from fetch import dataset
from record import save_fixed_record, save_variable_record
from disk import start_db, close_db, TABLE_HEADER
from constants import MAX_SIZES

print(MAX_SIZES)

table = TABLE_HEADER(table_name="Digimons", record_size=72)

FILE_INFO = {
    "PATH": "../data/",
    "NAME": "records",
}

file, inUse = start_db(name=FILE_INFO['NAME'], path=FILE_INFO['PATH'], table=table).values()

dataset.apply(lambda row: save_fixed_record(row, file), axis=1)
# dataset.apply(lambda row: save_variable_record(row, file), axis=1)

close_db(file)