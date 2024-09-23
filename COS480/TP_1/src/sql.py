from constants import HEADER
import fixed_heap_sql as fh
import ordenated_archive_sql as oa
import variable_heap_sql as vh

def insert_one(data):
    if HEADER.organization_type == "fixed_heap":
        return fh.insert_one(data)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.insert_one(data)
    elif HEADER.organization_type == "variable_heap":
        return vh.insert_one(data)

def insert_many(data_array):
    if HEADER.organization_type == "fixed_heap":
        return fh.insert_many(data_array)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.insert_many(data_array)
    elif HEADER.organization_type == "variable_heap":
        return vh.insert_many(data_array)

def select_one(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_one(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.select_one(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.select_one(query, field=field)

def select_many_by_array(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_array(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.select_many_by_array(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.select_many_by_array(query, field=field)

def select_many_by_interval(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_interval(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.select_many_by_interval(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.select_many_by_interval(query, field=field)

def select_many_by_field(query, field="Digimon"):
    if HEADER.organization_type == "fixed_heap":
        return fh.select_many_by_field(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.select_many_by_field(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.select_many_by_field(query, field=field)
        
def delete_one(query, field="Number"):
    if HEADER.organization_type == "fixed_heap":
        return fh.delete_one(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.delete_one(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.delete_one(query, field=field)
        
def delete_many_by_field(query, field="Digimon"):
    if HEADER.organization_type == "fixed_heap":
        return fh.delete_many_by_field(query, field=field)
    elif HEADER.organization_type == "ordenated_archive":
        return oa.delete_many_by_field(query, field=field)
    elif HEADER.organization_type == "variable_heap":
        return vh.delete_many_by_field(query, field=field)