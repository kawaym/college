from record import save_variable_record, create_variable_record
from constants import HEADER, FILE_PATH, VARIABLE_FIELDS

import pandas as pd
import re

def insert_one(data):
    HEADER.read_header_from_json()
    HEADER.records_number += 1
    inserted_index = 0
    if not HEADER['end_of_free_space']:
        HEADER['end_of_free_space'].append(4095)
        HEADER.number_of_blocks = 1
    record = create_variable_record(data)
    position = HEADER['end_of_free_space'][0] - len(record['bytes'])
    for i in range(0, len(HEADER['end_of_free_space'])):
        free_space_in_block = HEADER['end_of_free_space'][i]
        inserted_index = i
        if (free_space_in_block) < len(record['bytes']):
            if i+1 == len(HEADER['end_of_free_space']):
                HEADER['end_of_free_space'].append(4095)
                inserted_index += 1
                HEADER.number_of_blocks += 1
            position = HEADER['end_of_free_space'][i+1] + HEADER.blocking_factor * i - len(record['bytes'])
    HEADER['end_of_free_space'][inserted_index] -= len(record['bytes'])
                   
    file = open(FILE_PATH, "r+")
    save_variable_record(record, file, position=position)
    file.close()
    HEADER.write_header_to_json()
    
def insert_many(data_array):
    rows = []
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            rows.append(row)
    else:
        for row in data_array:
            rows.append(row)
    for row in rows:
        insert_one(row)
        
"36 07 43 08 51 05 56 05 956 210 708 410 813 398 81 NanimonChampionVirusEarth"
"41 07 48 08 56 05 61 05 095 06 2 1070 084 108 133 098 081 Nanimon Champion Virus Earth"
def get_all_records():
    file = open(FILE_PATH, "r")
    content = file.read()
    pattern = r'r\|([^|]+)\|'
    records = re.findall(pattern, content)
    return records
    

def select_one(query, field):
    records = get_all_records()
    
    for record in records:
        header_end = len(VARIABLE_FIELDS) * 4
        variables_beginning = int(record[:header_end][:2])
        fixed_fields = record[header_end:variables_beginning]
        if field not in VARIABLE_FIELDS:
            offset = 0
            size = 0
            for key, value in HEADER['schema'].items():
                if key in VARIABLE_FIELDS:
                    continue
                if key == field:
                    size = value
                    break
                offset += value
            if query == fixed_fields[offset:offset + value]:
                return record
        else:
            for index in range(0, len(VARIABLE_FIELDS)):
                header = record[:header_end]
                field_index = int(header[index * 4 : index * 4 + 2])
                size = int(header[index * 4 + 2 : index * 4 + 4])
                if query == record[field_index: field_index + size]:
                    return record
    return None

def select_many_by_array(query, field):
    records = get_all_records()
    return_array = []
    
    for record in records:
        header_end = len(VARIABLE_FIELDS) * 4
        variables_beginning = int(record[:header_end][:2])
        fixed_fields = record[header_end:variables_beginning]
        if field not in VARIABLE_FIELDS:
            offset = 0
            size = 0
            for key, value in HEADER['schema'].items():
                if key in VARIABLE_FIELDS:
                    continue
                if key == field:
                    size = value
                    break
                offset += value
            if fixed_fields[offset:offset + value] in query:
                return_array.append(record)
        else:
            for index in range(0, len(VARIABLE_FIELDS)):
                header = record[:header_end]
                field_index = int(header[index * 4 : index * 4 + 2])
                size = int(header[index * 4 + 2 : index * 4 + 4])
                if record[field_index: field_index + size] in query:
                    return_array.append(record)                    
    return return_array

def select_many_by_interval(query, field):
    records = get_all_records()
    return_array = []
    
    for record in records:
        header_end = len(VARIABLE_FIELDS) * 4
        variables_beginning = int(record[:header_end][:2])
        fixed_fields = record[header_end:variables_beginning]
        if field not in VARIABLE_FIELDS:
            offset = 0
            size = 0
            for key, value in HEADER['schema'].items():
                if key in VARIABLE_FIELDS:
                    continue
                if key == field:
                    size = value
                    break
                offset += value
            if int(fixed_fields[offset:offset + value]) in range(int(query[0]), int(query[1]) + 1):
                return_array.append(record)
        else:
            for index in range(0, len(VARIABLE_FIELDS)):
                header = record[:header_end]
                field_index = int(header[index * 4 : index * 4 + 2])
                size = int(header[index * 4 + 2 : index * 4 + 4])
                if int(fixed_fields[offset:offset + value]) in range(int(query[0]), int(query[1]) + 1):
                    return_array.append(record)
    return return_array

def select_many_by_field(query, field):
    records = get_all_records()
    return_array = []
    
    for record in records:
        header_end = len(VARIABLE_FIELDS) * 4
        variables_beginning = int(record[:header_end][:2])
        fixed_fields = record[header_end:variables_beginning]
        if field not in VARIABLE_FIELDS:
            offset = 0
            size = 0
            for key, value in HEADER['schema'].items():
                if key in VARIABLE_FIELDS:
                    continue
                if key == field:
                    size = value
                    break
                offset += value
            if query == fixed_fields[offset:offset + value]:
                return_array.append(record)
        else:
            for index in range(0, len(VARIABLE_FIELDS)):
                header = record[:header_end]
                field_index = int(header[index * 4 : index * 4 + 2])
                size = int(header[index * 4 + 2 : index * 4 + 4])
                if query == record[field_index: field_index + size]:
                    return_array.append(record)
    return return_array

def delete_one(query, field):
    record = select_one(query, field)
    if record:
        HEADER.records_number -= 1
        with open(FILE_PATH, 'r') as file:
            content = file.read()
        true_record = "r|" + record + "|"
        content = content.replace(true_record, ' ' * len(record))
        with open(FILE_PATH, "w") as file:
            file.write(content)
    HEADER.number_of_deletes_since_last_organization += 1
    if HEADER.number_of_deletes_since_last_organization > 10:
        organize()
        HEADER.write_header_to_json()
    return None

def delete_many_by_field(query, field):
    records = select_many_by_field(query, field)
    HEADER.records_number -= len(records)
    with open(FILE_PATH, 'r') as file:
        content = file.read()
    for record in records:
        true_record = "r|" + record + "|"
        content = content.replace(true_record, ' ' * len(record))
    with open(FILE_PATH, "w") as file:
        file.write(content)
    HEADER.number_of_deletes_since_last_organization += 1
    if HEADER.number_of_deletes_since_last_organization > 10:
        organize()
    HEADER.write_header_to_json()
    return None       

def organize():
    HEADER.read_header_from_json()
    def process_block(file):
        initial_index = file.tell() 

        block = file.read(HEADER.blocking_factor)
        
        if not block:
            return None, None
        
        block_without_whitespace = ''.join(block.split())

        final_block = ' ' * (HEADER.blocking_factor - len(block_without_whitespace)) + block_without_whitespace

        first_record = re.search(r'r\|', final_block)
        
        if first_record:
            index_first_record = first_record.start()
        else:
            index_first_record = None
        return final_block, index_first_record

    with open(FILE_PATH, 'r+') as file:
        for i in range(0, HEADER.number_of_blocks):
            modified_block, record_index = process_block(file)
            
            if modified_block is None:
                break
            
            file.seek(HEADER.blocking_factor * (i + 1) - len(modified_block))
            
            file.write(modified_block)
            
            if record_index is not None:
                HEADER['end_of_free_space'][i] = record_index
    HEADER.write_header_to_json()
