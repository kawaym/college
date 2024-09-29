from disk import start_db, load_db_data
from sql import delete_many_by_field
from fetch import dataset
from constants import HEADER
from fixed_heap_sql import insert_one

start_db()
HEADER.read_header_from_json()
# HEADER.write_header_to_json()
# insert_one(dataset.iloc[0])
# load_db_data("digimons")
# delete_many_by_field("Electric", "Attribute")
