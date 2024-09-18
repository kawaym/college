from constants import VARIABLE_FIELDS_WITH_SIZE
from utils import convert_to_bytes

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
    byte_stream = convert_to_bytes(stream["bytes"])
    record_file.write(byte_stream)
    print("Registro salvo")
    
def create_variable_record(record):
    processed_record = {"bytes": ""}
    fixed_fields = ""
    variable_fields = ""
    # null_bitmap = len(record.keys())
    # null_bitmap_bytes = convert_to_bytes(hex(null_bitmap)[2:])
    header_size = len("".join('0000' for _ in VARIABLE_FIELDS_WITH_SIZE.keys()))
    
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            continue
        fixed_fields += str(record[key])
        
    offset = header_size + len(fixed_fields) 
    # + null_bitmap * 2
        
    for key in VARIABLE_FIELDS_WITH_SIZE.keys():
        processed_record['bytes'] += f"{offset:02}" + "{:02}".format(len(str(record[key])))
        variable_fields += str(record[key])
        offset += len(str(record[key]))
        
        
    data_fields = fixed_fields + variable_fields
    # + null_bitmap_bytes.decode("ascii") 
    
    processed_record['bytes'] += data_fields
    return processed_record

def save_variable_record(record, record_file):
    stream = create_variable_record(record)
    byte_stream = convert_to_bytes(stream['bytes'])
    record_file.write(byte_stream)
    print("Registro salvo")