from record import save_fixed_record
from constants import HEADER, FILE_PATH
import pandas as pd
from pathlib import Path
from os import stat

EXTENSION_FILE_PATH = Path("../data/extension")

def insert_one(data):
    file = open(EXTENSION_FILE_PATH, "a")
    saved_record = select_one(data['Number'], field="Number")
    if saved_record:
        raise Exception("Primary key already exists")
    save_fixed_record(data, file)
    file.close()
    # Se o tamanho de extension for maior do que um bloco, reorganizar o arquivo records
    if stat(EXTENSION_FILE_PATH).st_size >= HEADER.blocking_factor:
        organize()

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

def select_one(query, field):
    file = open(EXTENSION_FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    record = ""
    while True:
        if index in HEADER.deleted:
            continue
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + HEADER['schema'][field])]
        if query == read_number.strip():
            record = read_record
            return [record, index]
        index += 1
    file.close()
    if EXTENSION_FILE_PATH.exists():
        file = open(EXTENSION_FILE_PATH, "r")
        while True:
            if index in HEADER.deleted:
                continue
            read_record = file.read(71)
            if read_record == '':
                break
            read_number = read_record[offset:(offset + HEADER['schema'][field])]
            if query == read_number.strip():
                record = read_record
                return [record, index]
            index += 1
    return []

def select_many_by_array(query, field):
    file = open(EXTENSION_FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        if index in HEADER.deleted:
            continue
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = read_record[offset:(offset + HEADER['schema'][field])]
        if read_number in query:
            records.append([read_record, index])
        index += 1
    file.close()
    if EXTENSION_FILE_PATH.exists():
        file = open(EXTENSION_FILE_PATH, "r")
        while True:
            if index in HEADER.deleted:
                continue
            read_record = file.read(71)
            if read_record == '':
                break
            read_number = read_record[offset:(offset + HEADER['schema'][field])]
            if read_number in query:
                records.append([read_record, index])
            index += 1
    return records

def select_many_by_interval(query, field):
    file = open(EXTENSION_FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        if index in HEADER.deleted:
            continue
        read_record = file.read(71)
        if read_record == '':
            break
        read_number = int(read_record[offset: (offset + HEADER['schema'][field])])
        if read_number in range(query[0], query[1] + 1):
            records.append([read_record, index])
        index += 1
    if EXTENSION_FILE_PATH.exists():
        file = open(EXTENSION_FILE_PATH, "r")
        while True:
            if index in HEADER.deleted:
                continue
            read_record = file.read(71)
            if read_record == '':
                break
            read_number = int(read_record[offset: (offset + HEADER['schema'][field])])
            if read_number in range(query[0], query[1] + 1):
                records.append([read_record, index])
            index += 1
    return records

def select_many_by_field(query, field):
    file = open(EXTENSION_FILE_PATH, "r")
    index = 0
    offset = HEADER.get_offset(field)
    records = []
    while True:
        if index in HEADER.deleted:
            continue
        read_record = file.read(71)
        if read_record == '':
            break
        read_field = read_record[offset: (offset + HEADER['schema'][field])]
        if read_field.strip() == query:
            records.append([read_record, index])
        index += 1
    if EXTENSION_FILE_PATH.exists():
        file = open(EXTENSION_FILE_PATH, "r")
        while True:
            if index in HEADER.deleted:
                continue
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
    HEADER.deleted.append(index)

def organize():
    # Colocar tudo do extension numa fila
    records_extension = []
    with open(EXTENSION_FILE_PATH, "r") as file:
        read_record = file.read(71)
        while read_record != "":
            records_extension.append(read_record)
            read_record = file.read(71)

    records_extension.sort()

    # Passear pelo records:
    with open(FILE_PATH, "r+") as file:
        index = 0
        next_extension_record_number = int(records_extension[0][:-1])
        read_record = file.read(71)
        while read_record != "":
            # Se achar um deletado: 
            if index in HEADER.deleted:
                position = HEADER.record_size * index
                if index == next_extension_record_number:
                    # Colocar o próximo elemento aqui, se for seu lugar
                    save_fixed_record(records_extension[0], file, position=position)
                    records_extension.pop(0)
                    next_extension_record_number = int(records_extension[0][:-1])
                else:
                    # Pegar o próximo elemento em records e colocar aqui
                    next_record = file.read(71)
                    file.seek(file.tell() - 71)
                    save_fixed_record(next_record, file, position=position)
                    HEADER.deleted.append(index + 1)
                HEADER.deleted.remove(index)

            # Se o record em extension for pra ser colocado depois do record atual
            if next_extension_record_number >= int(read_record[0][:-1]):
                # Salvar o próximo record no array de records a serem adicionados
                next_record = file.read(71)
                records_extension.append(next_record)
                file.seek(file.tell() - 71)
                # Substituir o próximo record pelo record em extension
                position = HEADER.record_size * index
                save_fixed_record(records_extension[0], file, position=position)
                records_extension.pop(0)
                # Reorganizar o array, para podermos colocar o record deletado em outro lugar na hora devida
                records_extension.sort()
                next_extension_record_number = int(records_extension[0][:-1])
            index += 1
        # Se ainda houverem record para serem adicionados, adicionar no final do arquivo
        for record in records_extension:
            save_fixed_record(record, file, position=position)
            records_extension.pop(0)