#!/usr/bin/env python3

import argparse
import regex, re
import sys
import textwrap

'''
assumptions:
    The file's trick header starts with /* or // and ends with */ or //
    If those characters are found in the trick header contents,
      i.e. Library dependencies: (another/lib/path.cc) /* used for specific reason */
    you will get undefined behavior with this script

exit codes:
    -1 - too many characters in line
    -2 - invalid indentation
    -3 - invalid python module
    -4 - no python module found
    -5 - inline parenthesis found with trick comment header i.e. PYTHON_MODULE: (some.module)

This script takes a CML source file and checks if the Trick header
meets the desired parameters. With the --check_format argument, the file
is checked and a zero return code is returned if the file meets the
standards. Without the --check_format argument, the file gets rewritten
with the common Trick header format
'''


# trick comment header : [re for inline open parenthesis check, regex string for header contents]
regex_dict = {"DOC TITLE"                   : [r"(?is)doc title:[^\S\r\n]*\(", r"(?is)doc title\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "PURPOSE"                     : [r"(?is)purpose:[^\S\r\n]*\(", r"(?is)purpose\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "REFERENCE"                   : [r"(?is)reference:[^\S\r\n]*\(", r"(?is)reference\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "REQUIREMENTS"                : [r"(?is)requirements:[^\S\r\n]*\(", r"(?is)requirements\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "ASSUMPTIONS AND LIMITATIONS" : [r"(?is)assumptions and limitations:[^\S\r\n]*\(", r"(?is)assumptions and limitations\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "CLASS"                       : [r"(?is)class:[^\S\r\n]*\(", r"(?is)class\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "LIBRARY DEPENDENCY"          : [r"(?is)library[ _]?dependenc(?:y|ies):[^\S\r\n]*\(", r"(?is)library[ _]?dependenc(?:y|ies)\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "IO DEPENDENCY"               : [r"(?is)io dependenc(?:y|ies):[^\S\r\n]*\(", r"(?is)io dependenc(?:y|ies)\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "ICG IGNORE TYPES"            : [r"(?is)icg[ _]?ignore[ _]?types?:[^\S\r\n]*\(", r"(?is)icg[ _]?ignore[ _]?types?\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "SWIG"                        : [r"(?is)swig:[^\S\r\n]*\(", r"(?is)swig\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "ICG"                         : [r"(?is)icg:[^\S\r\n]*\(", r"(?is)icg\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "DEFAULT DATA"                : [r"(?is)default[ _]?data:[^\S\r\n]*\(", r"(?is)default[ _]?data\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "PYTHON_MODULE"               : [r"(?is)python[ _]?modules?:[^\S\r\n]*\(", r"(?is)python[ _]?modules?\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "PROGRAMMERS"                 : [r"(?is)programmers:[^\S\r\n]*\(", r"(?is)programmers\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "LANGUAGE"                    : [r"(?is)language:[^\S\r\n]*\(", r"(?is)language\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"]}

exit_code_dict = {-1 : "error: too many characters in line",
                  -2 : "error: invalid indentation",
                  -3 : "warning: invalid python module",
                  -4 : "warning: no python module found",
                  -5 : "error: inline parenthesis"}

def check_inline_open_parens(text):
    for key in regex_dict.keys():
        res = bool(re.search(regex_dict[key][0], text))
        # inline colon found for a trick header comment
        if res == True:
            return(True)
    return(False)


def read_file(in_file):
    with open(in_file, 'r') as orig:
        return(orig.read())


def check_trick_header_format(original_header_str):
    for line in original_header_str.splitlines():
        if len(line) > args.character_limit:
            exit_code_list.append(in_file + ": " + exit_code_dict[-1])
            return(-1)
        if line.lstrip().startswith("/*") or\
           line.endswith("*/")            or\
           line.lstrip().startswith("\\") or\
           ":" in line or len(line) < 1:
            continue
        else:
            line = line.replace("\t","    ")
            leading_spaces = len(line) - len(line.lstrip())
            if leading_spaces != (args.indent_width+1) and\
               leading_spaces != args.indent_width     and\
               leading_spaces != (args.indent_width+3):
                exit_code_list.append(in_file + ": " + exit_code_dict[-2])
                return(-2)
    if in_file.endswith((".h",".hh")) and check_python_module_str(original_header_str) == False:
        exit_code_list.append(in_file + ": " + exit_code_dict[-3])
        # uncomment the line below when we want to fail this script if anything other than (cml) or (cml.*) is present for a PYTHON_MODULE
        #return(-3)
    if check_inline_open_parens(original_header_str) == True:
        exit_code_list.append(in_file + ": " + exit_code_dict[-4])
        return(-4)
    return(0)


def get_original_comment(in_file):
    original_comment = []
    read_line_flag = False
    with open(in_file, 'r') as orig:
        for line in orig:
            if line.lstrip().startswith("//") or line.lstrip().startswith("/*"):
                read_line_flag = True
            elif line.endswith("*/\n") or line.endswith("//\n"):
                original_comment.append(line)
                read_line_flag = False
                return("".join(original_comment))

            if read_line_flag == True:
                original_comment.append(line)


def remove_asterisks_comment_style(text):
    res_line = []
    for line in text.splitlines(keepends=True):
        if line.lstrip().startswith("/*")  == True or line.endswith("*/") == True:
            pass
        elif line.lstrip().startswith("*"):
            line = line.replace("*", " ", 1)
        res_line.append(line)
    return("".join(res_line))


def strip_outer_parens(text):
    # remove first "(\n" or "("
    text = re.sub(r'\(\n|\(', '', text, count=1)

    # reverse to remove last ")\n" or ")"
    res = text[::-1]
    res = re.sub(r'\n\)|\)', '', res, count=1)

    return res[::-1]

def generate_common_trick_header(original_header_str):
    formatted_comment_str = ""
    header_block_list = []
    header_block = ""
    original_header_str = remove_asterisks_comment_style(original_header_str)

    for key in regex_dict:
        if args.verbose:
            print("generate_common_trick_header: key is:", key)

        trick_pattern = regex.compile(regex_dict[key][1])
        trick_match = trick_pattern.search(original_header_str)
        if trick_match:
            header_block = trick_match.group("block")

            parens_pattern = re.compile(r'^\(\s*\(\s*.*\s*\)\s*\)$', re.DOTALL)
            # two sequential parentheses found, lets remove them for the common format
            if parens_pattern.match(header_block):
                header_block = strip_outer_parens(header_block)

            # remove leading white space from each line
            header_block_list = [txt.lstrip() for txt in header_block.splitlines()]
            # remove trailing empty strings from list
            while header_block_list and header_block_list[-1] == "":
                header_block_list.pop()

            # This block of code fixes an issue where a file has been proccessed with this script
            # with a character limit of 100, then again with a lesser character limit, the PURPOSE and
            # ICG blocks will have a weird indentation since we are removing the parethesis.
            if key == "PURPOSE":
                if header_block_list[0] == "(" and header_block_list[-1] == ")":
                    header_block_list = header_block_list[1:-1]
                header_block_list[0] = header_block_list[0].replace("(", "", 1)
                header_block_list[-1] = "".join(header_block_list[-1].rsplit(")",1))

            if key == "ICG":
                if len(header_block_list) > 1:
                    header_block_list = header_block_list[1:-1]
                header_block_list[0] = header_block_list[0].replace("(", "", 1)
                header_block_list[0] = header_block_list[0].replace(")", "", 1)

            if args.verbose:
                print("generate_common_trick_header: header_block_list:", header_block_list)

            # create common header
            formatted_comment_str += key + ":\n"
            formatted_comment_str += " "*args.indent_width + "(\n"
            for header_block_line in header_block_list:
                # if the only character in a line is a parentheses, don't keep it
                if "(\n" == header_block_line.replace(" ","") or\
                   "("   == header_block_line.replace(" ","") or\
                   ")\n" == header_block_line.replace(" ","") or\
                   "\n"  == header_block_line.replace(" ","") or\
                   ")"   == header_block_line.replace(" ",""):
                    continue
                # The line here is stripped of the leading white space characters that count towards
                #   the defined character limit. The character index is less than the defined character
                #   limit to take account white spaces when the line is indented with the specified
                #   indent width
                character_limit_ndx = args.character_limit - args.indent_width - 1
                if len(header_block_line) > character_limit_ndx:
                    # break line into multiple lines if the character limit is exceeded
                    header_block_line = textwrap.fill(header_block_line, width=character_limit_ndx, subsequent_indent=" "*(args.indent_width+3))
                if not header_block_line.endswith('\n') and len(header_block_line) > 0:
                    header_block_line += "\n"
                formatted_comment_str += " "*(args.indent_width+1) + header_block_line
            formatted_comment_str += " "*args.indent_width + ")\n\n"
        elif key == "PYTHON_MODULE" and in_file.endswith((".h",".hh")):
            exit_code_list.append(in_file + ": " + exit_code_dict[-3])

    return(formatted_comment_str)


def check_python_module_str(text):
    trick_pattern = regex.compile(regex_dict["PYTHON_MODULE"][1])
    trick_match = trick_pattern.search(text)
    if trick_match:
        header_block = trick_match.group("block")

    # this function turns contents within nested parenthesis into a list
    # i.e (abc(def)ghi(jkl)) -> ['(def)', '(jkl)', '(abc(def)ghi(jkl))']
    # then checks each item if it starts with "cml"
    pattern = regex.compile(r"\((?:[^()]+|(?R))*\)")
    res = pattern.findall(text)
    res_list = [m[1:-1] for m in res]
    for line in res_list:
        if line.startswith("cml") == False:
            return(False)
    return(True)


def get_args():
    parser = argparse.ArgumentParser()

    parser.add_argument('-w', '--indent_width', type=int, help='number of spaces to indent text', action="store")
    parser.add_argument('-l', '--character_limit', type=int, help='maximum number of characters per line', action="store")
    parser.add_argument('-c', '--check_format', help='check if file meets format standard and return', default=False, action="store_true")
    parser.add_argument('-v', '--verbose', help='print command line arguments and information during the file execution', default=False, action="store_true")
    parser.add_argument('files', nargs='+', help='list of file names to process')

    args = parser.parse_args()
    return args

def print_args(args):

    print("indent_width    = ",args.indent_width)
    print("character_limit = ",args.character_limit)
    print("check_format    = ",args.check_format)
    print("file(s)         = ",args.files)
    print("verbose         = ",args.verbose)
    return


if __name__ == '__main__':

    args = get_args()
    exit_code_list = []
    printed = False

    if args.verbose:
        print_args(args)

    in_file_list = args.files
    for in_file in in_file_list:
        original_file = read_file(in_file)
        original_trick_header = get_original_comment(in_file)

        return_code = check_trick_header_format(original_trick_header)

        if return_code == 0:
            continue

        if args.check_format == False and printed == False:
            print("Converting the following files:")
            printed = True

        # removing last new line character from the formatted Trick header then surrounding the header with a
        # common comment style /* */ based on the specified character_limit
        common_formatted_header = generate_common_trick_header(original_trick_header)[:-1]
        common_formatted_header = "/" + "*"*(args.character_limit-1) + "\n" + common_formatted_header + "*"*(args.character_limit-1) + "/\n"

        if args.verbose:
            print("resulting common trick header:")
            print(common_formatted_header)

        formatted_file_str = original_file.replace(original_trick_header, common_formatted_header)

        if args.check_format == False:
            with open(in_file, "w") as file:
                file.write(formatted_file_str)

    for file in exit_code_list:
        print(file)
    if any("error:" in line for line in exit_code_list) == False:
        print("File(s) meet the common format standard")
        sys.exit(0)
    else:
        sys.exit(-1)

