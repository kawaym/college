from record import save_fixed_record
import pandas as pd

def insert_one(data, file):
    save_fixed_record(data, file)   
    print("Inserção realizada com sucesso")
def insert_many(data_array, file):
    if isinstance(data_array, pd.DataFrame):
        for _, row in data_array.iterrows():
            save_fixed_record(row, file)
    else:
        for row in data_array:
            save_fixed_record(row, file)
    print("Inserções realizadas com sucesso")