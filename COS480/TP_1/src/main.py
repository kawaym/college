from disk import start_db, load_db_data
from sql import delete_many_by_field
from fetch import dataset

start_db()
load_db_data("digimons")

data = delete_many_by_field("Free", "Type")
print(data)