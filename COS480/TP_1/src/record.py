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
    
def create_variable_record(record):
    processed_record = {"bytes": ""}
    fixed_fields = ""
    variable_fields = ""
    null_bitmap = "".join('0' for _ in record.keys())
    header_size = len("".join('0000' for _ in VARIABLE_FIELDS_WITH_SIZE.keys()))
    
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            continue
        fixed_fields += str(record[key])
        
    offset = header_size + len(fixed_fields + null_bitmap)
        
    for key in VARIABLE_FIELDS_WITH_SIZE.keys():
        processed_record['bytes'] += f"{offset:02}" + "{:02}".format(len(str(record[key])))
        variable_fields += str(record[key])
        offset += len(str(record[key]))
        
        
    data_fields = fixed_fields + null_bitmap + variable_fields
    processed_record['bytes'] += data_fields
    return processed_record

def save_variable_record(record, record_file):
    stream = create_variable_record(record)
    record_file.write(stream["bytes"])
    print("Registro salvo")