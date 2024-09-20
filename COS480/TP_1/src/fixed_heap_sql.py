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
    write_header_to_json(header)
    file = open(file_path, "a+")
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            save_fixed_record(row, file)
    else:
        for row in data_array:
            save_fixed_record(row, file)
    print("Inserções realizadas com sucesso")

def delete_from_index(index):
    header = read_header_from_json()
    header.records_number -= 1
    header.deleted.append(index)
    write_header_to_json(header)
    print("Deleção realizada com sucesso")