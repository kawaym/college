import json

from pathlib import Path
from utils import convert_to_bytes
from constants import DELIMITER

def start_db(path, name, table):
    file_path = path + name
    
    if(Path(file_path).exists()):
        record_file = open(file_path, "a+")
        return {"file": record_file, "inUse": True}
    else:
        record_file = open(file_path, "x+")
        # table.write_to_file(record_file)
        return {"file": record_file, "inUse": False} 

def close_db(file):
    file.close()    
    
class TABLE_HEADER:
    def __init__(self, table_name, blocking_factor=4096, is_fixed=True, organization_type="fixed_heap", *args, **kwargs):
        self.table_name = table_name
        self.records_number = 0
        self.blocking_factor = blocking_factor
        self.is_fixed = is_fixed
        self.organization_type = organization_type
        for key, value in  kwargs.items():
            setattr(self, key, value)
    def write_to_file(self, file):
        file.write(convert_to_bytes(DELIMITER))
        for key, value in vars(self).items():
            file.write(convert_to_bytes(f'{key}: {value}\n'))
        file.write(convert_to_bytes(DELIMITER))
    def display(self):
        for key, value in vars(self).items():
            print(f"{key}: {value}")

def read_header_from_json():
    file = open("../data/metadata.json", 'r')
    data = json.load(file)
    table = TABLE_HEADER(**data)
    table.display()
    