from constants import VARIABLE_FIELDS_WITH_SIZE

def create_fixed_record(record):
    processed_record = {"bytes": ""}
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            processed_record["bytes"] += record[key].ljust(VARIABLE_FIELDS_WITH_SIZE[key])
            continue
        processed_record["bytes"] += str(record[key])
    return processed_record

def save_fixed_record(record, record_file):
    stream = create_fixed_record(record)
    record_file.write(stream["bytes"])
    print("Registro salvo")
    