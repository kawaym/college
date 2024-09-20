import json

class TABLE_HEADER:
    def __init__(self, table_name, blocking_factor=4096, is_fixed=True, organization_type="fixed_heap", *args, **kwargs):
        self.table_name = table_name
        self.records_number = 0
        self.blocking_factor = blocking_factor
        self.is_fixed = is_fixed
        self.organization_type = organization_type
        self.record_size = 0
        self.number_of_blocks = 1
        for key, value in  kwargs.items():
            setattr(self, key, value)
    def __getitem__(self, key):
        return getattr(self, key, None)
    def __setitem__(self, key, value):
        return setattr(self, key, value)
    def get_offset(self, field):
        offset = 0
        for key, value in self['schema'].items():
            if key == field:
                break
            offset += value
        if offset == self.record_size:
            raise Exception("Field does not exist")
        return offset
    def display(self):
        for key, value in vars(self).items():
            print(f"{key}: {value}")
    def toJSON(self):
        return json.dumps(
            self,
            default=lambda o: o.__dict__, 
            sort_keys=False,
            indent=2)
    def read_header_from_json(self):
        file = open("../data/metadata.json", 'r')
        data = json.load(file)
        for key, value in data.items():
            setattr(self, key, value)
        file.close()
    def write_header_to_json(self):
        file = open("../data/metadata.json", 'w')
        data = self.toJSON()
        file.write(data)
        file.close()