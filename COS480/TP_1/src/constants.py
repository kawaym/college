from fetch import dataset

def get_size(key):
    return dataset[key].astype(str).map(len).max().item()

VARIABLE_FIELDS = ["Digimon", "Stage", "Type", "Attribute"]
VARIABLE_FIELDS_WITH_SIZE = {key: get_size(key) for key in VARIABLE_FIELDS}
