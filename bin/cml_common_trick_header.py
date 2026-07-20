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
    -3 - invalid python module or no python module found (future use)
    -4 - inline parenthesis found with trick comment header i.e. PYTHON_MODULE: (some.module)

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
              "DEFAULT DATA"                : [r"(?is)default[ _]?data:[^\S\r\n]*\(", r"(?is)default[ _]?data\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "PYTHON_MODULE"               : [r"(?is)python[ _]?modules?:[^\S\r\n]*\(", r"(?is)python[ _]?modules?\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "PROGRAMMERS"                 : [r"(?is)programmers:[^\S\r\n]*\(", r"(?is)programmers\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"],
              "LANGUAGE"                    : [r"(?is)language:[^\S\r\n]*\(", r"(?is)language\s*:\s*(?P<block>\((?:[^()]+|(?P>block))*\))"]}


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
            print("error: Too many characters in line\n" + line)
            return(-1)
        if line.startswith("*") or line.startswith("/*") or\
           line.startswith("\\") or ":" in line or len(line) < 1:
            continue
        else:
            line = line.replace("\t","    ")
            leading_spaces = len(line) - len(line.lstrip())
            if leading_spaces != (args.indent_width+1) and leading_spaces != args.indent_width:
                print("error: Improper indentation in line\n" + line)
                return(-2)
    if check_inline_open_parens(original_header_str) == True:
        print("error: Inline open parenthesis with trick header comment\n" + line)
        return(-4)
    return(0)


def get_original_comment(in_file):
    original_comment = []
    read_line_flag = False
    with open(in_file, 'r') as orig:
        for line in orig:
            if line.startswith("//") or line.startswith("/*"):
                read_line_flag = True
            elif line.endswith("*/\n") or line.endswith("//\n"):
                original_comment.append(line)
                read_line_flag = False
                return("".join(original_comment))

            if read_line_flag == True:
                original_comment.append(line)


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

    for key in regex_dict:
        if args.verbose:
            print("generate_common_trick_header: key is:", key)

        trick_pattern = regex.compile(regex_dict[key][1])
        trick_match = trick_pattern.search(original_header_str)
        if trick_match:
            header_block = trick_match.group("block")

            if key == "PYTHON_MODULE" and check_python_module_str(header_block) == False:
                print("warning: PYTHON_MODULE in trick header, but invalid module present")
                print("warning: only (cml) or (cml.*) PYTHON_MODULE modules are accepted")
                # uncomment the lines below when we want to fail this script if anything other than (cml) or (cml.*) is present for a PYTHON_MODULE
                #print("error: PYTHON_MODULE in trick header, but invalid module present")
                #print("error: only (cml) or (cml.*) PYTHON_MODULE modules are accepted")
                #sys.exit(-3)

            parens_pattern = re.compile(r'^\(\s*\(\s*.*\s*\)\s*\)$', re.DOTALL)
            # two sequential parentheses found, lets remove them for the common format
            if parens_pattern.match(header_block):
                header_block = strip_outer_parens(header_block)

            # remove leading white space from each line
            header_block_list = [txt.lstrip() for txt in header_block.split("\n")]
            # remove trailing empty strings from list
            while header_block_list and header_block_list[-1] == "":
                header_block_list.pop()

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
                    header_block_line = textwrap.fill(header_block_line, width=character_limit_ndx, subsequent_indent=" "*(args.indent_width+1))
                if not header_block_line.endswith('\n'):
                    header_block_line += "\n"
                formatted_comment_str += " "*(args.indent_width+1) + header_block_line
            formatted_comment_str += " "*args.indent_width + ")\n\n"
        elif key == "PYTHON_MODULE":
            print("warning: no PYTHON_MODULE in trick header")

    return(formatted_comment_str)


def generate_formatted_file(formatted_str):
    with open(args.file, "w") as file:
        file.write(formatted_str)


def check_python_module_str(text):
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
    parser.add_argument('-f', '--file', help='file to check', action="store")
    parser.add_argument('-v', '--verbose', help='print command line arguments and information during the file execution', default=False, action="store_true")

    args = parser.parse_args()
    return args

def print_args(args):

    print("indent_width    = ",args.indent_width)
    print("character_limit = ",args.character_limit)
    print("check_format    = ",args.check_format)
    print("file            = ",args.file)
    print("verbose         = ",args.verbose)
    return


if __name__ == '__main__':

    args = get_args()

    if args.verbose:
        print_args(args)

    in_file = args.file
    original_file = read_file(in_file)
    original_trick_header = get_original_comment(in_file)

    return_code = check_trick_header_format(original_trick_header)

    if args.check_format == True:
        print("return code:", return_code)
        sys.exit(return_code)

    if return_code == 0:
        print("file:",args.file)
        print("Trick header file meets common file standard")
        sys.exit(0)

    # removing last new line character from the formatted Trick header then surrounding the header with a
    # common comment style /* */ based on the specified character_limit
    common_formatted_header = generate_common_trick_header(original_trick_header)[:-1]
    common_formatted_header = "/" + "*"*(args.character_limit-1) + "\n" + common_formatted_header + "*"*(args.character_limit-1) + "/\n"

    if args.verbose:
        print("resulting common trick header:")
        print(common_formatted_header)

    formatted_file_str = original_file.replace(original_trick_header, common_formatted_header)

    with open(args.file, "w") as file:
        file.write(formatted_file_str)

