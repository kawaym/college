from fetch import dataset

def get_size(key):
    return dataset[key].astype(str).map(len).max().item()

VARIABLE_FIELDS = ["Digimon", "Stage", "Type", "Attribute"]
VARIABLE_FIELDS_WITH_SIZE = {key: get_size(key) for key in VARIABLE_FIELDS}


MAX_SIZES = {key: get_size(key) for key in dataset}

DELIMITER = "----------------------------------------------------------------------------------------------------------------------------------\n"

TEST_SINGLE_INSERT = {"Number": 1, "Digimon": 'Antedeguemon', 
            "Stage": "Baby", "Type": "Free", 
            "Attribute": "Neutral", "Memory": 0, 
            "Equip Slots": 1, "Lv 50 HP": 100, 
            "Lv50 SP": 20, "Lv50 Atk": 20, 
            "Lv50 Def": 20, "Lv50 Int": 20, 
            "Lv50 Spd": 20}
TEST_MANY_INSERT = [
    {"Number": 1, "Digimon": 'Antedeguemon', 
            "Stage": "Baby", "Type": "Free", 
            "Attribute": "Neutral", "Memory": 0, 
            "Equip Slots": 1, "Lv 50 HP": 100, 
            "Lv50 SP": 20, "Lv50 Atk": 20, 
            "Lv50 Def": 20, "Lv50 Int": 20, 
            "Lv50 Spd": 20},
    {"Number": 2, "Digimon": 'Antedeguemon', 
            "Stage": "Baby", "Type": "Free", 
            "Attribute": "Neutral", "Memory": 0, 
            "Equip Slots": 1, "Lv 50 HP": 100, 
            "Lv50 SP": 20, "Lv50 Atk": 20, 
            "Lv50 Def": 20, "Lv50 Int": 20, 
            "Lv50 Spd": 20}
]