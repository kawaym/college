from record import save_fixed_record, read_fixed_record
from disk import read_header_from_json, write_header_to_json
import pandas as pd

def insert_one(data, file_path):
    header = read_header_from_json()
    header.records_number += 1
    if header.deleted:
        file = open(file_path, "r+")
        position = header.record_size * header.deleted[0]
        save_fixed_record(data, file, position=position)
        header.deleted.pop(0)
        file.close()
    else:
        file = open(file_path, "a")
        save_fixed_record(data, file)
        file.close()   
    write_header_to_json(header)
    print("Inserção realizada com sucesso")

def insert_many(data_array, file_path):
    header = read_header_from_json()
    header.records_number += len(data_array)
    file = open(file_path, "r+")
    rows = []
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            rows.append(row)
    else:
        for row in data_array:
            rows.append(row)
    for position in header.deleted:
        file = open(file_path, "r+")
        if rows:
            save_fixed_record(rows[0], file, position=position * header.record_size)
            header.deleted[0]
            rows.pop(0)
        file.close()
    for row in rows:
        file = open(file_path, "a+")
        save_fixed_record(row, file)
        file.close()
    write_header_to_json(header)
    print("Inserções realizadas com sucesso")

def delete_by_index(index):
    header = read_header_from_json()
    header.records_number -= 1
    header.deleted.append(index)
    write_header_to_json(header)
    print("Deleção realizada com sucesso")