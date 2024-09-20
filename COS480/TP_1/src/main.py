from fetch import dataset
from record import save_fixed_record, save_variable_record, read_fixed_record
from disk import start_db, TABLE_HEADER, read_header_from_json, write_header_to_json
from constants import MAX_SIZES, TEST_SINGLE_INSERT, TEST_MANY_INSERT
from fixed_heap_sql import insert_one, insert_many, delete_by_index

table = TABLE_HEADER(table_name="Digimons", record_size=71)

FILE_INFO = {
    "PATH": "../data/",
    "NAME": "records",
}

file_path = FILE_INFO["PATH"] + FILE_INFO["NAME"]

table = read_header_from_json()
write_header_to_json(table)

start_db(name=FILE_INFO['NAME'], path=FILE_INFO['PATH'], table=table)
insert_one(TEST_SINGLE_INSERT, file_path)
insert_many(dataset.iloc[1:5], file_path)
insert_many(TEST_MANY_INSERT, file_path)
delete_by_index(0)
insert_one({"Number": 2, "Digimon": 'Raichu', 
            "Stage": "Baby", "Type": "Free", 
            "Attribute": "Neutral", "Memory": 0, 
            "Equip Slots": 1, "Lv 50 HP": 100, 
            "Lv50 SP": 20, "Lv50 Atk": 20, 
            "Lv50 Def": 20, "Lv50 Int": 20, 
            "Lv50 Spd": 20}, file_path)
delete_by_index(20)
insert_one({"Number": 2, "Digimon": 'Zikachu', 
            "Stage": "Baby", "Type": "Free", 
            "Attribute": "Neutral", "Memory": 0, 
            "Equip Slots": 1, "Lv 50 HP": 100, 
            "Lv50 SP": 20, "Lv50 Atk": 20, 
            "Lv50 Def": 20, "Lv50 Int": 20, 
            "Lv50 Spd": 20}, file_path)
delete_by_index(1)
delete_by_index(0)
delete_by_index(50)
insert_many(dataset.iloc[40:41], file_path)
# dataset.apply(lambda row: save_fixed_record(row, file), axis=1)
# dataset.apply(lambda row: save_variable_record(row, file), axis=1)