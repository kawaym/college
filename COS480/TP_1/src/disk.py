
def start_db():
    try:
        record_file = open("../data/records", "xb")
    except:
        record_file = open("../data/records", "wb")
    return record_file 

def close_db(file):
    file.close()