from disk import start_db, load_db_data
from sql import select_many_by_field

start_db()
load_db_data("digimons")

data = select_many_by_field("Kuramon", field="Digimon")
print(data)