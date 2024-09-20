from fetch import dataset
from definitions import TABLE_HEADER
from pathlib import Path

def get_size(key):
    return dataset[key].astype(str).map(len).max().item()

VARIABLE_FIELDS = ["Digimon", "Stage", "Type", "Attribute"]
VARIABLE_FIELDS_WITH_SIZE = {key: get_size(key) for key in VARIABLE_FIELDS}

HEADER = TABLE_HEADER({"example"})
FILE_PATH = Path('../data/records')


MAX_SIZES = {key: get_size(key) for key in dataset}