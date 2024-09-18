from constants import VARIABLE_FIELDS_WITH_SIZE

def create_fixed_record(record):
    byte_stream = ""
    for key in record.keys():
        if (key in VARIABLE_FIELDS_WITH_SIZE):
            byte_stream += record[key].ljust(VARIABLE_FIELDS_WITH_SIZE[key])
            continue
        byte_stream += str(record[key])
    return byte_stream

def save_fixed_record(record, record_file):
    stream = create_fixed_record(record)
    record_file.write(stream)
    print("Registro salvo")
    