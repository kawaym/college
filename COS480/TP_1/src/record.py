from constants import VARIABLE_FIELDS_WITH_SIZE, MAX_SIZES

def create_fixed_record(record):
    processed_record = {"bytes": ""}
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            processed_record["bytes"] += record[key].ljust(VARIABLE_FIELDS_WITH_SIZE[key])
            continue
        processed_record["bytes"] += str(record[key]).rjust(MAX_SIZES[key], "0")
    return processed_record

def save_fixed_record(record, file, position=-1):
    stream = create_fixed_record(record)
    if position != -1:
        file.seek(position)
    file.write(stream['bytes'])
    
def create_variable_record(record):
    processed_record = {"bytes": "r|"}
    fixed_fields = ""
    variable_fields = ""
    header_size = len("".join('0000' for _ in VARIABLE_FIELDS_WITH_SIZE.keys()))
    
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            continue
        fixed_fields += str(record[key]).rjust(MAX_SIZES[key], "0")
        
    offset = header_size + len(fixed_fields) 
        
    for key in VARIABLE_FIELDS_WITH_SIZE.keys():
        processed_record['bytes'] += f"{offset:02}" + "{:02}".format(len(str(record[key])))
        variable_fields += str(record[key])
        offset += len(str(record[key]))
        
        
    data_fields = fixed_fields + variable_fields
    
    processed_record['bytes'] +=  data_fields + '|'
    
    return processed_record

def save_variable_record(record, file, position=-1):
    if position != -1:
        file.seek(position)
    file.write(record['bytes'])
    
def read_fixed_record(size, file_path):
    file = open(file_path, "r")
    data = file.read(size)
    file.close()
    return data    
    