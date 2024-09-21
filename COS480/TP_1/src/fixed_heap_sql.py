from record import save_fixed_record
from constants import HEADER, FILE_PATH
import pandas as pd

def insert_one(data):
    HEADER.read_header_from_json()
    HEADER.records_number += 1
    file = open(FILE_PATH, "r+")
    if HEADER.deleted:
        position = HEADER.record_size * HEADER.deleted[0]
        save_fixed_record(data, file, position=position)
        HEADER.deleted.pop(0)
    else:
        if file.tell() > ((HEADER.blocking_factor * (HEADER.number_of_blocks)) - HEADER.record_size):
            file.seek((HEADER.blocking_factor * (HEADER.number_of_blocks)))
            HEADER.number_of_blocks += 1
        file = open(FILE_PATH, "a")
        save_fixed_record(data, file)
    file.close()   
    HEADER.write_header_to_json()

def insert_many(data_array):
    HEADER.read_header_from_json()
    HEADER.records_number += len(data_array)
    file = open(FILE_PATH, "r+")
    rows = []
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            rows.append(row)
    else:
        for row in data_array:
            rows.append(row)
    file = open(FILE_PATH, "r+")
    for position in HEADER.deleted:
        if rows:
            save_fixed_record(rows[0], file, position=position * HEADER.record_size)
            HEADER.deleted.pop(0)
            rows.pop(0)
    for row in rows:
        if file.tell() > ((HEADER.blocking_factor * (HEADER.number_of_blocks)) - HEADER.record_size):
            file.seek((HEADER.blocking_factor * (HEADER.number_of_blocks)))
            HEADER.number_of_blocks += 1
        save_fixed_record(row, file)
    file.close()
    HEADER.write_header_to_json()

def select_one(query, field):
    file = open(FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    record = ""
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + HEADER['schema'][field])]
        if query == read_number:
            record = read_record
            return [record, index]
        index += 1
    return []

def select_many_by_array(query, field):
    file = open(FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + HEADER['schema'][field])]
        if read_number in query:
            records.append([read_record, index])
        index += 1
    return records

def select_many_by_interval(query, field):
    file = open(FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = int(read_record[offset: (offset + HEADER['schema'][field])])
        if read_number in range(query[0], query[1] + 1):
            records.append([read_record, index])
        index += 1
    return records

def select_many_by_field(query, field):
    file = open(FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        read_record = file.read(71)
        if read_record == '':
            break
        read_field = read_record[offset: (offset + HEADER['schema'][field])]
        if read_field.strip() == query:
            records.append([read_record, index])
        index += 1
    return records
        
def delete_one(query, field):
    record = select_one(query, field=field)
    if record:
        delete_by_index(record[1])
    
def delete_many_by_field(query, field):
    records = select_many_by_field(query, field=field)
    for record in records:
        delete_by_index(record[1])


def delete_by_index(index):
    HEADER.read_header_from_json()
    HEADER.records_number -= 1
    HEADER.deleted.append(index)
    HEADER.write_header_to_json()