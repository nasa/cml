/*############################################################################
PURPOSE:
  (Handling for groups of triggers)

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Split from fault_arch)))
############################################################################*/

#include "../include/xml_helper.hh"
#include <cstring> // strcmp
#include <cctype> //toupper
#include <string>
#include <libxml/parser.h> // xmlNodePtr, xmlAttrPtr

/*******************************************************************************
xml_find
Purpose:(Finds an XML node with the given name at the same level as the
specified node.)
*******************************************************************************/
xmlNodePtr XmlHelper::xml_find(xmlNodePtr node, const char* name) {
  if (!node || !name) return nullptr;

  for(; node; node=node->next) {
    if(xml_name_match(node, name)) {
      return node;
    }
  }
  return nullptr;
}


/*******************************************************************************
xml_find_child
Purpose:(Finds a child XML node with the given name.)
*******************************************************************************/
xmlNodePtr XmlHelper::xml_find_child(xmlNodePtr node, const char* name) {
  if (!node || !name) return nullptr;

  return xml_find(node->children, name);
}


/*****************************************************************************
xml_find_progeny
Purpose:
  (Finds a node in the subtree of the specified node, with a name
   matching the specified name. xml_find_child is restricted to one
   generation below the specified node; this method will search through the
   entire subtree through all generations.)
*****************************************************************************/
xmlNodePtr XmlHelper::xml_find_progeny(xmlNodePtr node, const char* name) {
  if (!node || !name) return nullptr;

  xmlNodePtr found_node = nullptr;
  xmlNodePtr search_node = node->children;
  while (search_node && !found_node) {
    if (xml_name_match( search_node, name)) {
      found_node = search_node;
    }
    else {
      found_node = xml_find_progeny( search_node, name);
    }
    search_node = search_node->next;
  }
  return found_node;
}

/*******************************************************************************
xml_find_value
Purpose:(Finds the value of a property of an XML node.)
*******************************************************************************/
const char* XmlHelper::xml_find_value(
  xmlNodePtr node,
  const char* name,
  bool allow_case)
{
  if (!node || !name) return nullptr;

  for(xmlAttrPtr val = node->properties; val; val = val->next) {
    // XML uses unsigned chars to represent strings, but it is much more
    // common to use chars for this purpose. For ASCII strings, it is safe
    // to convert from unsigned char* to char* using reinterpret_cast.
    // Before using reinterpret-cast, check the data type, allowing char*
    // directly, and reinterpret cast on unsigned char*, and disallowing all
    // else, just in case.
    if (val->name && val->children) {
      if(strcmp(xml_convert_ptr(val->name), name) == 0) {
        return xml_convert_ptr(val->children->content);
      }
      // If that failed, try changing the case on the first character if
      // allowing a case difference to be considered a match.
      //else:
      if (allow_case) {
        std::string left(xml_convert_ptr(val->name));
        std::string right(name);
        if (left.length() > 0 && right.length() > 0) {
          left[0]  = toupper( left[0]);
          right[0] = toupper(right[0]);
          if (left == right) {
            return xml_convert_ptr(val->children->content);
          }
        }
      }  
    }
  }
  return nullptr;
}


/*******************************************************************************
xml_name_match
Purpose:(Checks if an XML node has the given name.)
*******************************************************************************/
bool XmlHelper::xml_name_match(xmlNodePtr node, const char* name) {
  if (!node || !name) return false;

  // See the comment in xml_find_value
  return strcmp(xml_convert_ptr(node->name), name) == 0;
}


/*****************************************************************************
xml_convert_ptr
Purpose:(
 - Overloaded method providing safety to reinterpret-cast.
   Allows reinterpret-cast to be applied to convert an unsigned char* to char*
   Passes char* straight back,
   Rejects everything else.
*****************************************************************************/
const char * XmlHelper::xml_convert_ptr( const void* )
{
  return nullptr;
}
const char * XmlHelper::xml_convert_ptr( const char* in)
{
  return in;
}
const char * XmlHelper::xml_convert_ptr( const unsigned char* in)
{
  return reinterpret_cast<const char *>(in);
}
