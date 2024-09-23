import pandas as pd

from pathlib import Path
from constants import HEADER
from sql import insert_many

def start_db():
    file_path = Path("../data/metadata.json")
    HEADER.read_header_from_json()
    if(Path(file_path).exists()):   
        open(file_path, "a+")
    else:
        open(file_path, "x")

def load_db_data(file_name):
    file_path = Path(f"../data/{file_name}.csv")
    data = pd.read_csv(file_path)
    HEADER.read_header_from_json()
    HEADER.records_number = 0
    HEADER.number_of_blocks = 1
    HEADER.deleted = []
    HEADER['end_of_free_space'] = []
    HEADER.write_header_to_json()
    open("../data/records", "w")
    insert_many(data) 
    