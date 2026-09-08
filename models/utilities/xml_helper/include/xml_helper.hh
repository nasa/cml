/*############################################################################
PURPOSE:
  (Parse the fault injection XML file)

LIBRARY DEPENDENCY:
  ((../src/xml_helper.cc))

PROGRAMMERS:
  (((Andrew Spencer)  (OSR) (June 2015) (CR3333) (Initial version))
   ((Daniel Ghan) (OSR) (October 2021) (Antares) (Split from fault_arch)))
############################################################################*/


#ifndef CML_XML_HELPER_HH
#define CML_XML_HELPER_HH

#include <string>
#include <libxml/parser.h>

class XmlHelper {
 public:
  static xmlNodePtr xml_find(        xmlNodePtr node, const std::string& name);
  static xmlNodePtr xml_find_child(  xmlNodePtr node, const std::string& name);
  static xmlNodePtr xml_find_progeny(xmlNodePtr node, const std::string& name);
  static std::string xml_find_value( xmlNodePtr node,
                                     const std::string& name,
                                     bool allow_case = false);
  static bool xml_name_match(        xmlNodePtr node, const std::string& name);

  static const std::string xml_convert_ptr (const void *);
};

#endif
