from record import save_fixed_record
from disk import read_header_from_json, write_header_to_json
import pandas as pd

def insert_one(data, file):
    header = read_header_from_json()
    header.records_number += 1
    write_header_to_json(header)
    save_fixed_record(data, file)   
    print("Inserção realizada com sucesso")

def insert_many(data_array, file):
    header = read_header_from_json()
    header.records_number += len(data_array)
    write_header_to_json(header)
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            save_fixed_record(row, file)
    else:
        for row in data_array:
            save_fixed_record(row, file)
    print("Inserções realizadas com sucesso")