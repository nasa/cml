# Assumptions for this script:
# 1. The given struct ends with a CSU bus name (that's how the script works best)
#       Example: fsw_out.fswOut_block.fswOut.fsw1HzOut.GDOout1Hz.ORBGUID
# 2. This script will not be called from the command line
#
# You can use this class in any python file that gets executed at the input level
# (e.g. RUN_*/input.py, Log_data/log_*.py). Example implementation given below:
# exec(open("Modified_data/common/LogStruct.py").read())
# LogStruct("<struct.name",<log_cycle>)
# LogStruct("<struct.name>",log_cycle = X.X, skip_gen = True, parent_class_layer = 3, etc.)
#
# See the class's doc string for description of the optional inputs
#
# To use LogStruct's logfile generating functionality from the command line, 
# see SIM_orion/Modified_data/common/LogStruct_ExternalGen.py.

import sys, os, pdb
import xml.etree.ElementTree as ET

class LogStruct():
    """
        How to use this class in any file at the input level:
        exec(open("Modified_data/common/LogStruct.py").read())
        LogStruct("<struct.name>",optional arguments)

        Optional arguments:
        log_cycle (double)            - logging rate of executed log file
                                        Default is 1.0 seconds

        log_path (string)             - Location of generated log file
                                        Default is SIM_orion/Log_data/Log_struct_generated

        skip_gen (bool)               - Skips generating log file if it already exists.
                                        Default is False

        exec_logfile (bool)           - Flag to execute the generated logfile
                                        Default is True

        parent_class_layer (int)      - Indicates how many layers of class names to
                                        go back to grab for logfile naming.
                                        Default is 2

        output_textfile (string)      - Name of text file to write variables to.
                                        Default is empty, meaning a python log file will
                                        be generated

        desc_include (bool)           - Include the descriptions of variables to output
                                        text file
                                        Default is False

        dr_format (string)            - Data record format, csv, trk, or hdf5
                                        Default is trk

        dr_group (data record object) - Use this dr_group to add the found variables

        sie_path (string)             - Path to S_sie.resource file to parse for variables
                                        Default is 'S_sie.resource', relative to sim

        return_variable_names (bool)  - Returns after finding variable names, skipping
                                        any logging script creation and/or execution
                                        Default is False

        ignore_trick_logging (bool)   - Skip adding new dr_group to trick_logging
                                        Default is False

        spare_include (bool)          - Flag to include Spare variables
                                        Default is False
    """

    def __init__(self,
                 var_struct,
                 log_cycle = 1.0,
                 log_path = 'Log_data/Log_struct_generated',
                 skip_gen = False,
                 exec_logfile = True,
                 parent_class_layer = 2,
                 output_textfile = '',
                 desc_include = False,
                 dr_format = 'trk',
                 ignore_trick_logging = False,
                 dr_group = None,
                 sie_path = 'S_sie.resource',
                 return_variable_names = False,
                 spare_include = False):
        """
        This script uses a given struct and log_cycle to generate, execute, and call a
        python logging file that logs all the variables associated with the struct.
        """

        # Check to see that S_sie.resource exists
        sie_path = os.path.abspath(sie_path)
        if not os.path.isfile(sie_path):
            print("S_sie.resource file doesn't exist, you need to build the sim.\n")
            sys.exit(1)
        else:
            print("LogStruct: Extracting structures from S_sie.resource...."),
        
        self.dsa = 'data_structure_address'
        self.count_lim = sys.getrecursionlimit() #limit the number of recursive calls

        # Extract the tree and root
        tree = ET.parse(sie_path)
        self.root = tree.getroot()
        print("Done\n")
        
        # Determine if Trick version is 13 or 17
        if self.root.findall('top_level_object'):
            self.trick_ver = '17'
        
            # Collect all object names, useful for knowing when a leaf is hit
            self.object_list = [tmp_class.get('name') for tmp_class in self.root.findall('class')]
        else:
            self.trick_ver = '13'

        # Initialize list to be populated
        self.var_list = []

        self.dr_format = dr_format
        self.ignore_trick_logging = ignore_trick_logging

        # Break up the given structure into its constituent parameters
        self.str_prms = var_struct.split('.')

        # Define the last class name as the parent and add a . to the struct
        # for later use
        self.parent = self.str_prms[-1]
        self.parent_base = var_struct + '.'

        # Get the first tree node
        first_node = self.get_first_node()

        # Find variables
        count = 0
        count = self.find_vars(first_node,self.parent,self.parent_base,count,spare_include)

        # Collect variable names into list
        self.var_names = []
        for var in self.var_list:
            var_name = var[0]
            var_dim = var[1]
            if len(var_dim) == 1:
                if var_dim[0] in [0,1]:
                    self.var_names.append(var_name)
                else:
                    [self.var_names.append(f'{var_name}[{i}]') for i in range(var_dim[0])]
            elif len(var_dim) == 2:
                [self.var_names.append(f'{var_name}[{i}][{j}]') for i in range(var_dim[0]) for j in range(var_dim[1])]
            else:
                for i in range(var_dim[0]):
                    for j in range(var_dim[1]):
                        for k in range(var_dim[2]):
                            self.var_names.append(dr_group.add_variable(f'{var_name}[{i}][{j}][{k}]'))

        # If only names desired, return
        if return_variable_names:
            return

        if dr_group is not None:
            self.add_variables_to_dr_group(dr_group)
            print("LogStruct: Adding variables to existing dr_group.")
            return

        # Build the output file name
        if not os.path.isdir(log_path):
            os.makedirs(log_path)

        if output_textfile:
            print(f"LogStruct: Exporting variable list to text file for struct {var_struct} ...")
            output_filename = '.'.join([output_textfile,'csv'])
        else:
            print(f"LogStruct: Building python logging file for struct {var_struct} ...")
            output_filename = self.build_output_filename(log_path,parent_class_layer)

        self.output_file_name = os.path.join(log_path,output_filename)

        if skip_gen:
            # Check to see if the output file already exists
            if not os.path.isfile(self.output_file_name):
                skip_gen = False

        if not skip_gen:

            # Write variables to desired output file
            if self.output_file_name.endswith('py'):
                # Create the log file
                self.make_log_file()
            else:
                # Right now there are only 2 supported output formats, so
                # the only other option besides py is csv
                # A csv file can't be exeucted, so make exec_logfile false
                self.make_text_file(desc_include)
                exec_logfile = False
            print("Done\n")

        # Execute and call the log file
        if exec_logfile:
            print(f"LogStruct: Executing and calling struct {var_struct} ...\n")
            print("In current working diretory {os.getcwd()}\n")
            exec(open(self.output_file_name).read())
            exec(f"log_{self.gp_p}({log_cycle})")
            print("Done\n")

    def build_output_filename(self,log_path,parent_class_layer):
        if len(self.str_prms) > 1:
            try:
                gparent = self.str_prms[-parent_class_layer]
            except:
                print("LogStruct: Struct name not long enough to name as requested. Reverting to default.\n")
                gparent = self.str_prms[-2]
            self.gp_p = f"{gparent.lower()}_{self.parent.lower()}"
        else:
            self.gp_p = self.parent.lower()

        return f'log_{self.gp_p}_struct.py'

    def get_first_node(self):
        """
        get_first_node() finds the type name that applies to the last class
        name in the given full struct (self.parent). This type name will
        serve as the first node to start finding the children.
        """

        str_search = self.str_prms[0]

        if self.trick_ver == '17':
            for tmp in self.root.findall('top_level_object'):
                str_name = tmp.get('name')
                type_name = tmp.get('type')
                if str_search == str_name:
                    str_search = type_name
                    break
            for i in range(1, len(self.str_prms)):
                for tmp_class in self.root.findall('class'):
                    class_name = tmp_class.get('name')
                    if str_search == class_name:
                        for tmp_member in tmp_class.findall('member'):
                            str_name = tmp_member.get('name')
                            type_name = tmp_member.get('type')
                            if str_name == self.str_prms[i]:
                                str_search = type_name
                                break
                        break

        else:
            for i in range(1,len(self.str_prms)):
                for tmp in self.root.findall('template'):
                    str_name = tmp.find('struct_name').text
                    prm = tmp.find('parameter').text
                    if str_name == str_search and prm == self.str_prms[i]:
                        str_search = tmp.find('type_name').text
                        break
        return str_search

    def find_vars(self,node,parent,base,count,spare_include):
        """
        find_vars() takes in a struct name (node) and a parameter (parent).
        Also, it takes in a flag to decide to include Spare variables or not.
        It builds the list of variable names with their dimensions and description.
        """
        def get_dimensions_13(tmp):
            """
            get_dimensions_13() takes in a template block and outputs
            the associated dimension sizes in a vector up to 1x3.
            """

            num_dim = int(tmp.find('num_dimensions').text)
            if num_dim > 0 and num_dim < 4:
                dim = [0]*num_dim
                for i in range(num_dim):
                    str_dim = 'dim' + str(i)
                    dim[i] = int(tmp.find(str_dim).text)
            elif num_dim > 3:
                var = tmp.find('parameter').text
                print(f'The dimensions of the variable {var} are outside the limits of LogStruct \n')
                sys.exit(1)
            else: # if num_dim = 0
                dim = [0]

            return dim

        def get_dimensions_17(tmp):
            """
            get_dimensions_17() takes in a class member block and outputs
            the associated dimension sizes in a vector up to 1x3.
            """

            dim_list = tmp.findall('dimension')
            if dim_list:
                num_dim = len(dim_list)
                if num_dim > 0 and num_dim < 4:
                    dim = [0]*num_dim
                    for i in range(num_dim):
                        dim[i] = int(dim_list[i].text)
                    # If a dimension is present, but is only 0, change it to have a dimension of [1]
                    if dim == [0]:
                        dim == [1]
                elif num_dim > 3:
                    var = tmp.get('name')
                    print(f'The dimensions of the variable {var} are outside the limits of LogStruct \n')
                    sys.exit(1)
                else:
                    dim = [0]
            else:
                dim = [0]
            return dim

        children = []
        children_tn = []
        children_td = []
        children_desc = []
        dim_list = []

        if count > self.count_lim:
            print('Recursive count limit reached, exiting.')
            sys.exit(1)

        if self.trick_ver == '17':
            for tmp_class in self.root.findall('class'):
                class_name = tmp_class.get('name')
                if class_name == node:
                    for tmp_member in tmp_class.findall('member'):
                        prm = tmp_member.get('name')
                        type_name = tmp_member.get('type')
                        units = tmp_member.get('units')

                        # Extract description
                        if 'description' in tmp_member.keys():
                            desc = tmp_member.get('description')
                            if desc.startswith('@n '):
                                desc = desc[len('@n '):]
                        else:
                            desc = ''

                        # Extract dimensions
                        dim = get_dimensions_17(tmp_member)

                        # A few special skip conditions to avoid infinite recursion
                        if prm.endswith('_'):
                            continue

                        if class_name == type_name:
                            continue

                        if class_name == 'GravityInteraction' and prm == 'dyn_parent':
                            continue

                        if class_name == 'Planet' and prm == 'grav_body':
                            continue

                        if class_name == 'TimeDyn' and prm == 'time_manager':
                            continue

                        if class_name == 'JeodBaseTime' and prm == 'time_manager':
                            continue

                        # If the new parameter is a data structure array, add
                        # parameter[i] for each i in the size of the array dimension
                        if type_name in self.object_list and dim[0] > 0:
                            for k in range(dim[0]):
                                children.append(prm + '[' + str(k) + ']')
                                children_tn.append(type_name)
                                children_desc.append(desc)
                                dim_list.append(dim)
                        else:

                            if not spare_include and "Spare" in prm:
                                continue
                            children.append(prm)
                            children_tn.append(type_name)
                            children_desc.append(desc)
                            dim_list.append(dim)

            # If a child has the same name as a class in object_list, then
            # that means that child has children of its own. Call this function recursively
            # until no more children are found for that path and add that child to the
            # leaf list
            if children:
                for i in range(len(children)):
                    if children_tn[i] in self.object_list:
                        node = children_tn[i]
                        parent = children[i]
                        count += 1
                        new_base = base + parent + '.'
                        count = self.find_vars(node,parent,new_base,count,spare_include)
                    else:
                        var = base + children[i]
                        self.var_list.append([var, dim_list[i], children_desc[i]])
        else:
            # Trick 13 option

            # Find the specific template group(s) that has the node as it's struct name.
            # The parameters in the template group(s) are the parent's children
            for tmp in self.root.findall('template'):
                str_name = tmp.findtext('struct_name')
                if str_name == node:
                    prm = tmp.findtext('parameter')
                    type_def = tmp.findtext('type_def')
                    type_name = tmp.findtext('type_name')
                    dim = get_dimensions_13(tmp)

                    if tmp.find('description'):
                        desc = tmp.findtext('description')
                        if desc.startswith('@n '):
                            desc = desc[len('@n '):]
                        else:
                            desc = ''

                    # A few special skip conditions to avoid infinite recursion
                    if prm.endswith('_'):
                        continue

                    if str_name == type_name:
                        continue

                    if str_name == 'GravityInteraction' and prm == 'dyn_parent':
                        continue

                    if str_name == 'Planet' and prm == 'grav_body':
                        continue

                    if str_name == 'TimeDyn' and prm == 'time_manager':
                        continue

                    if str_name == 'JeodBaseTime' and prm == 'time_manager':
                        continue

                    # If the new parameter is a data structure array, add
                    # parameter[i] for each i in the size of the array dimension
                    if type_def == self.dsa and dim[0] > 0:
                        for k in range(dim[0]):
                            children.append(prm + '[' + str(k) + ']')
                            children_tn.append(type_name)
                            children_td.append(type_def)
                            children_desc.append(desc)
                            dim_list.append(dim)
                    else:

                        if not spare_include and "Spare" in prm:
                            continue
                        children.append(prm)
                        children_tn.append(type_name)
                        children_td.append(type_def)
                        children_desc.append(desc)
                        dim_list.append(dim)

            # If a child has "data_structure_address" as its type_def, then
            # that means that child has children of its own. Call this function recursively
            # until no more children are found for that path and add that child to the
            # leaf list
            if children:
                for i in range(len(children)):
                    if children_td[i] == self.dsa:
                        node = children_tn[i]
                        parent = children[i]
                        count += 1
                        new_base = base + parent + '.'
                        count = self.find_vars(node,parent,new_base,count,spare_include)
                    else:
                        var = base + children[i]
                        self.var_list.append([var, dim_list[i], children_desc[i]])

        return count

    def get_log_type(self,dim):
        """
        get_log_type() takes in a dimension vector and determines
        what type of logging is necessary for that dimension vector.
        """

        if len(dim) == 1:
            if dim[0] == 0 or dim[0] == 1:
                log_type = 'add_var'
            elif dim[0] == 3:
                log_type = '3vec'
            else:
                log_type = 'array'
        elif len(dim) == 2:
            if (dim[0] == 3 and dim[1] == 3):
                log_type = '3x3'
            else:
                log_type = 'mxn'
        else:
            log_type = 'mxnxp'

        return log_type

    def log_common_core_LogStruct(self):
        log_function_text = f'def log_{self.gp_p}(log_cycle):\n\n'
        if self.dr_format == 'trk':
            dr_text = 'DRBinary'
        elif self.dr_format == 'csv':
            dr_text = 'DRAscii'
        elif self.dr_format == 'hdf5':
            dr_text = 'DRHDF5'
        else:
            print("Invalid data record format given, defaulting to .trk")
            dr_text =  'DRBinary'
        log_function_text += f'  dr_group = trick.{dr_text}("{self.gp_p}_struct")\n'

        log_function_text += '  dr_group.thisown = 0\n'
        log_function_text += '  dr_group.set_cycle(log_cycle)\n'
        log_function_text += '  dr_group.set_freq(trick.DR_Always)\n'
        log_function_text += '  trick.add_data_record_group(dr_group, trick.DR_Buffer)\n'
        if not self.ignore_trick_logging:
            log_function_text += '  trick_logging.dr_groups.add_to_list(dr_group)\n'
        log_function_text += '\n'

        return log_function_text

    def get_log_function_text(self):
        """
        get_log_function_text() makes a text block that contains a log function
        definition of all the variables within the variable dictionary. It finds the
        variable's log_type according to its dimensions, then adds the variable
        addition to self.log_function_text.
        """
        for var_name in self.var_names:
            self.log_function_text += f'  dr_group.add_variable("{var_name}")\n'

        self.log_function_text += '\n  return\n\n'

    def add_variables_to_dr_group(self, dr_group):
        """
        add_variables_to_dr_group() adds the variables to an existing dr_group
        """
        [dr_group.add_variable(var_name) for var_name in self.var_names]

    def make_log_file(self):
        """
        make_log_file() creates the python log file handle and calls get_log_function_text() for each of the first_children
        until it has the full log file text and writes it to a python logging file.
        """
        # Create the file handle and define string alias
        with open(self.output_file_name,'w') as lf:
            # Set up the function text
            self.log_function_text = self.log_common_core_LogStruct()
            self.get_log_function_text()

            # Write to file and close the handle
            lf.write(self.log_function_text)

    def make_text_file(self, desc_include):
        """
        make_text_file() will export the variable list to a readable text file
        with the column headings Bus name and Dim
        """
        with open(self.output_file_name,'w') as tf:
            tf_text = 'Variable, Dim1, Dim2, Description\n'
            for var in self.var_list:
                var_name = var[0]
                var_dim = var[1]
                if desc_include:
                    temp_desc = var[2]
                else:
                    temp_desc = ''
                if var_dim == [0]:
                    var_dim = [1, 1]
                if len(var_dim) == 1:
                    var_dim = [var_dim[0], 1]
                tf_text += ', '.join([var_name,str(var_dim[0]),str(var_dim[1]),temp_desc]) + '\n'

            tf.write(tf_text)
