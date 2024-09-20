from constants import HEADER
import fixed_heap_sql as fh

def insert_one(data):
    if HEADER.organization_type == "fixed_heap":
        return fh.insert_one(data)

def insert_many(data_array):
    if HEADER.organization_type == "fixed_heap":
        return fh.insert_many(data_array)

def select_one(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_one(query, field=field)

def select_many_by_array(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_array(query, field=field)

def select_many_by_interval(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_interval(query, field=field)

def select_many_by_field(query, field="Digimon"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_field(query, field=field)
        
def delete_one(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.delete_one(query, field=field)
        
def delete_many_by_field(query, field="Digimon"):
    if HEADER.organization_type == "fixed_heap":
        return fh.delete_many_by_field(query, field=field)