from fetch import dataset
from record import save_fixed_record, save_variable_record
from disk import start_db, close_db

file = start_db()

# dataset.apply(lambda row: save_fixed_record(row, file), axis=1)
# dataset.apply(lambda row: save_variable_record(row, file), axis=1)
print(dataset.iloc[0])
save_variable_record(dataset.iloc[0], file)

close_db(file)