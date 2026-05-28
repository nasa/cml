import sys, re, pdb
import numpy as np
path = sys.argv[1]
group_match_str = 'Group:'.encode()
condition_headers = ['condition_name','condition_type','condition_count',
                     'condition_first_time', 'condition_last_time']
empty_line = '\n'.encode()
group_match = False
parse_line_check1 = False
parse_line_check2 = False

variables = list()
units = list()
conditions = list()
index_condvar = list()
time_list = []
data_list = []

def decode_float(byte_str_in):
    try:
        out = float(byte_str_in.decode().strip())
    except Exception as e:
        out = None
    return out

cond_info = {}
name = ''
with open(path,'rb') as file_in:
    for line in file_in:
        if line == empty_line:
            parse_line_check1 = False
            parse_line_check2 = False
            continue

        if parse_line_check1:
            header = line.split(b',')
            header = [h.strip() for h in header]
            
            curr_grp_vars = []
            for column in header:
                if not column.startswith(b'condition_'):
                    column = column.decode()
                    var, _, unit = re.search(r'(\S+)(\s+)?{(\S+)}',column).groups()

                    if unit == '--':
                        unit = '1'

                    curr_grp_vars.append(var)
                    if var not in variables:
                        variables.append(var)
                        units.append(unit)
                        
            parse_line_check2 = True
            parse_line_check1 = False
            continue
            
        # [0] Condition_name, [1] Condition_type, [2] Condition_count,
        # [3] condition_first_time, [4] condition_last_time
        if parse_line_check2:
            condition,data = re.split(rb'^(\w+.*? *),( *\d.*)',line,re.MULTILINE)[1:3]
            condition = condition.decode().strip()
            data = data.split(b',')
            # condition = data[0].decode()
            # condition = condition.strip()
            
            cond_type = data[0].decode().strip()
            
            if condition not in conditions:
                conditions.append(condition)

                if len(data) > 2:

                    if cond_type == '1':
                        # condition = f'Discrete: {condition}'
                        cond_time = decode_float(data[2])

                    elif cond_type == '0':
                        cond_time = decode_float(data[3])
                    
                    # Variable info comes with data[5:]
                    for idx, var_data in enumerate(data[4:]):
                        var_name = curr_grp_vars[idx]
                        if condition not in cond_info:
                            cond_info[condition] = []
                        
                        data_out = decode_float(var_data)
                        if data_out is not None:
                            cond_info[condition].append([var_name, cond_time, data_out])
                            
                            index_condvar.append([conditions.index(condition),variables.index(var_name)])
                            time_list.append(cond_time)
                            data_list.append(data_out)

                else:                  
                    for column in header:
                        if not column.startswith(b'condition_'):
                            column = column.decode()
                            var, _, unit = re.search(r'(\S+)(\s+)?{(\S+)}',column).groups()
                            index_condvar.append([conditions.index(condition),variables.index(var)])
                            time_list.append(np.nan)
                            data_list.append(np.nan)
            else:
                print(f"Duplicate condition ({condition}) will not be logged")

        if group_match_str in line:
            parse_line_check1 = True

            # Come up with name of data collect file
            # If single group, use that group name
            # If multi group, use multiGROUP
            if name == 'multiGROUP':
                pass
            elif not name:
                # Group lines always start with 'Group:'
                # remove leading/trailing spaces or \n
                # replace any remaining spaces with _
                name = line.decode()[6:]
                name = name.strip()
                name = re.sub(r'\s+','_',name)
            else:
                name = 'multiGROUP'
        else:
            parse_line_check1 = False

import os, h5py
file_out = os.path.join(os.path.dirname(path),f'enhanced_logging_{name}.h5')
    
if os.path.isfile(file_out):
    os.remove(file_out)
runs = [0]

# conditions = list(set(conditions))

# time_list = []
# data_list = []
# index_condvar = []
# for condition in conditions:
#     for info in cond_info[condition]:
#         index_condvar.append([conditions.index(condition),variables.index(info[0])])
#         time_list.append(info[1])
#         data_list.append(info[2])
# discrete_conditions
# index_condvar = np.array(index_condvar)

# variables,units,index_condvar = zip(*sorted(zip(variables,units,index_condvar)))

index_condvar = np.array(index_condvar, dtype=np.int32).T

times = np.array(time_list)
values = np.array(data_list)
with h5py.File(file_out, "w") as f:
      vlen_bytes_dtype = h5py.special_dtype(vlen=bytes)
      f.create_dataset("info.RUN_number", data=runs)
      f.create_dataset("condition_name", data=np.char.encode(conditions), dtype=vlen_bytes_dtype)
      f.create_dataset("variable_name", data=np.char.encode(variables), dtype=vlen_bytes_dtype)
      f.create_dataset("variable_units", data=np.char.encode(units), dtype=vlen_bytes_dtype)
      f.create_dataset("index_CONDVAR", data=index_condvar + 1) # use 1-based indices in the file for consistency with other tools

      if times.ndim == 2:
          f.create_dataset("condition_time", data=times, chunks=(times.shape[0], 1), compression="gzip")
      elif times.ndim == 1:
          f.create_dataset("condition_time", data=times)
      else:
          raise Exception("bad times shape")

      if values.ndim == 2:
          f.create_dataset("condition_value", data=values, chunks=(values.shape[0], 1), compression="gzip")
      elif values.ndim == 1:
          f.create_dataset("condition_value", data=values)
      else:
          raise Exception("bad values shape")
