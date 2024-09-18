from fetch import dataset
from record import save_fixed_record
from disk import start_db, close_db

file = start_db()

dataset.apply(lambda row: save_fixed_record(row, file), axis=1)

close_db(file)