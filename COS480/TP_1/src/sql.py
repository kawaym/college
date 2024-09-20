from constants import HEADER
import fixed_heap_sql as fh

def insert_one(data):
    if HEADER.organization_type == "fixed_heap":
        fh.insert_one(data)

def insert_many(data_array):
    if HEADER.organization_type == "fixed_heap":
        fh.insert_many(data_array)

def select_one(query, field="Number", size=3):
    if HEADER.organization_type == "fixed_heap":
        fh.select_one(query, field=field, size=size)

def select_many_by_array(query, field="Number", size=3):
    if HEADER.organization_type == "fixed_heap":
        fh.select_many_by_array(query, field=field, size=size)

def select_many_by_interval(query, field="Number", size=3):
    if HEADER.organization_type == "fixed_heap":
        fh.select_many_by_interval(query, field=field, size=size)

def select_many_by_field(query, field="Digimon", size=20):
    if HEADER.organization_type == "fixed_heap":
        fh.select_many_by_field(query, field=field, size=size)
        
def delete_one(query, field="Number", size=3):
    if HEADER.organization_type == "fixed_heap":
        fh.delete_one(query, field=field, size=size)
        
def delete_many_by_field(query, field="Digimon", size=20):
    if HEADER.organization_type == "fixed_heap":
        fh.delete_many_by_field(query, field=field, size=size)