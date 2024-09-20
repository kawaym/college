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
            header.deleted.pop(0)
            rows.pop(0)
        file.close()
    for row in rows:
        file = open(file_path, "a+")
        save_fixed_record(row, file)
        file.close()
    write_header_to_json(header)
    print("Inserções realizadas com sucesso")

def select_one(query, file_path, field="Number", size=3):
    file = open(file_path, "r")
    index = 0
    offset = 0
    record = ""
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + size)]
        if query == read_number:
            record = read_record
            return [record, index]
        index += 1
    return []

def select_many_by_array(query, file_path, field="Number", size=3):
    file = open(file_path, "r")
    index = 0
    offset = 0
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + size)]
        if read_number in query:
            records.append([read_record, index])
        index += 1
    return records

def select_many_by_interval(query, file_path, field="Number", size=3):
    file = open(file_path, "r")
    index = 0
    offset = 0
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = int(read_record[offset: (offset + size)])
        if read_number in range(query[0], query[1] + 1):
            records.append([read_record, index])
        index += 1
    return records

def select_many_by_field(query, file_path, field="Digimon", size=20):
    file = open(file_path, "r")
    index = 0
    offset = 3
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_field = read_record[offset: (offset + size)]
        if read_field.strip() == query:
            records.append([read_record, index])
        index += 1
    return records
        
def delete_one(query, file_path, field="Number", size=3):
    record = select_one(query, file_path, field=field, size=size)
    if record:
        delete_by_index(record[1])
    print("Deleção realizada com sucesso")
    
def delete_many_by_field(query, file_path, field="Digimon", size=20):
    records = select_many_by_field(query, file_path, field=field, size=size)
    for record in records:
        delete_by_index(record[1])
    print("Deleções realizadas com sucesso")


def delete_by_index(index):
    header = read_header_from_json()
    header.records_number -= 1
    header.deleted.append(index)
    write_header_to_json(header)