/*############################################################################
PURPOSE:
  (Parse the fault injection XML file)

LIBRARY DEPENDENCY:
  ((../src/xml_helper.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Split from fault_arch)))
############################################################################*/


#ifndef CML_XML_HELPER_CLASS_HH
#define CML_XML_HELPER_CLASS_HH

#include <libxml/parser.h> // xmlNodePtr

class XmlHelper {
 public:
  static xmlNodePtr xml_find(        xmlNodePtr node, const char* name);
  static xmlNodePtr xml_find_child(  xmlNodePtr node, const char* name);
  static xmlNodePtr xml_find_progeny(xmlNodePtr node, const char* name);
  static const char* xml_find_value( xmlNodePtr node,
                                     const char* name,
                                     bool allow_case = false);
  static bool xml_name_match(        xmlNodePtr node, const char* name);

  static const char * xml_convert_ptr (const void *);
  static const char * xml_convert_ptr (const char *);
  static const char * xml_convert_ptr (const unsigned char *);
 private:
  XmlHelper(const XmlHelper&);
  XmlHelper& operator = (const XmlHelper&);
};

#endif
