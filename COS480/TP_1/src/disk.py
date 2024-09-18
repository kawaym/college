
def start_db():
    try:
        record_file = open("../data/record", "x")
    except:
        record_file = open("../data/record", "a")
    return record_file 

def close_db(file):
    file.close()