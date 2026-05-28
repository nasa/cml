/*******************************TRICK HEADER******************************
 PURPOSE: (To execute pre-configurations for testing the XmlHelper model)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (Jul 2022) (ANTARES) (initial)))
***********************************************************************/
#ifndef CML_XML_HELPER_TESTER
#define CML_XML_HELPER_TESTER

#include <libxml/parser.h> // xmlParseFile, xmlNodePtr
#include <cstring>
#include <string>
#include <iostream>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "cml/models/utilities/xml_helper/include/xml_helper.hh"


struct XmlHelperTester {
  std::string filename;

  void test() {
    // Sanity checks to make sure the specified CML file reads correctly
    // These are not part of the model testing.
    xmlDocPtr xml_file = xmlParseFile(filename.c_str());
    if (!xml_file) {
      std::cout << "File " << filename << " not found.\n";
      error_report();
    }
    xmlNodePtr root_node = xml_file->children;
    if (!root_node) {
      std::cout << "File " << filename << " has no content.\n";
      error_report();
    }

    // Model testing starts here:
    // Start by identifying useful nodes:
    std::cout << "\n******************** name_match **************************\n";

    // Use XmlHelper::xml_name_match to verify root-node's name
    if (XmlHelper::xml_name_match( root_node, "SampleRoot")) {
      std::cout << "Name match with SampleRoot.\n";
    } else {
      std::cout << "ERROR: Name mismatch with expected root name, SampleRoot.\n";
      error_report();
    }

    std::cout << "**********************************************************\n";
    std::cout << "\n**************** xml_find_value **************************\n";
    std::cout << "Purpose of root-node is: "<<
                 XmlHelper::xml_find_value( root_node, "purpose") << "\n***\n";

    size_t node_count = 0;
    for (xmlNodePtr branch_node = root_node->children;
         branch_node != nullptr;
         branch_node = branch_node->next) {
      // Note - the first child might be a dummy text-node with no purpose.
      const char* purpose = XmlHelper::xml_find_value( branch_node, "purpose",true);
      if (purpose) {
        node_count++;
        std::cout << "Purpose of node "<< node_count <<" is: "<<
        purpose << "\n";
      }
    }
    if (node_count != 4) {
      std::cout << "ERROR: error in xml_find_value\n";
      error_report();
    }
    xmlNodePtr node2 = root_node->children->next->next->next;
    const char* purpose = XmlHelper::xml_find_value( node2, "purpose",true);
    std::cout << "Verifying node2: " << purpose << "\n";

    xmlNodePtr node3 = node2->next->next;
    purpose = XmlHelper::xml_find_value( node3, "purpose");
    std::cout << "Verifying node3: " << purpose << "\n";

    xmlNodePtr node4 = node3->next->next;
    purpose = XmlHelper::xml_find_value( node4, "purpose");
    std::cout << "Verifying node4: " << purpose << "\n";

    xmlNodePtr node5 = node2->children->next;
    purpose = XmlHelper::xml_find_value( node5, "purpose");
    std::cout << "Verifying node5: " << purpose << "\n";

    xmlNodePtr node11 = node5->next->next->children->next->next->next->next->next;
    purpose = XmlHelper::xml_find_value( node11, "purpose");
    std::cout << "Verifying node11: " << purpose << "\n";



    // Investigate use of xml_find
    // There are no nodes called SampleAltNode at the level of root-node
    // SampleAltNode exist at the 1st and 2nd generations.
    std::cout << "**********************************************************\n";
    std::cout << "\n******************** xml_find ****************************\n";
    xmlNodePtr start_node = root_node->children->next;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    xmlNodePtr node = XmlHelper::xml_find( start_node, "TargetNode");
    if (node==node2) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose",true) << "\n***\n";
    } else {
      std::cout << "ERROR: did not find correct TargetNode.\n";
      error_report();
    }

    start_node = node3;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    node = XmlHelper::xml_find( start_node, "TargetNode");
    if (node==node4) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose") << "\n***\n";
    } else {
      std::cout << "ERROR: did not find TargetNode.\n";
      error_report();
    }

    start_node = node5;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    node = XmlHelper::xml_find( start_node, "TargetNode");
    if (node) {
      std::cout << "ERROR: found TargetNode.\n" <<
                    XmlHelper::xml_find_value( node, "purpose") << "\n";
      error_report();
    } else {
      std::cout << "Did not find a TargetNode.\n";
    }


    std::cout << "**********************************************************\n";
    std::cout << "\n**************** xml_find_child **************************\n";
    // Investigate use of xml_find_child
    // There is a child-node of root-node called TargetNode (1st child)
    start_node = root_node;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    node = XmlHelper::xml_find_child( start_node, "TargetNode");
    if (node == node2) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose",true) << "\n***\n";
    } else {
      std::cout << "ERROR: did not find correct TargetNode.\n";
      error_report();
    }


    start_node = node2;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose",true) << "\n";
    node = XmlHelper::xml_find_child( start_node, "TargetNode");
    if (node) {
      std::cout << "ERROR: found TargetNode.\n" <<
                    XmlHelper::xml_find_value( node, "purpose") << "\n";
      error_report();
    } else {
      std::cout << "Did not find a TargetNode.\n***\n";
    }


    start_node = node3;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    node = XmlHelper::xml_find_child( start_node, "TargetNode");
    if (node) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose") << "\n***\n";
    } else {
      std::cout << "ERROR: did not find TargetNode.\n";
      error_report();
    }


    // Investigate use of xml_find_progeny
    // SampleAltNode exist at the 1st and 2nd generations (at 1.3 and 1.2.2).
    // 1.2.2 (second level
    std::cout << "**********************************************************\n";
    std::cout << "\n**************** xml_find_progeny ************************\n";

    start_node = root_node;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose") << "\n";
    node = XmlHelper::xml_find_progeny( start_node, "TargetNode");
    if (node==node2) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose",true) << "\n***\n";
    } else {
      std::cout << "ERROR: did not find correct TargetNode.\n";
      error_report();
    }

    start_node = node2;
    std::cout << "Starting with node "<<
                  XmlHelper::xml_find_value( start_node, "purpose",true) << "\n";
    node = XmlHelper::xml_find_progeny( start_node, "TargetNode");
    if (node==node11) {
      std::cout << "found TargetNode at " <<
                    XmlHelper::xml_find_value( node, "purpose") << "\n***\n";
    } else {
      std::cout << "ERROR: did not find correct TargetNode.\n";
      error_report();
    }

    std::cout << "**********************************************************\n";
  }


  void error_report() {
    std::cout << "**********************************************************\n";
    CMLMessage::fail(
      __FILE__,__LINE__,"Error\n",
      "Error encountered. Terminating.\n");
  }


  void code_coverage() {
    double x = 1.0;
    char c[2] = "a";
    const char * ptr = XmlHelper::xml_convert_ptr( &x);
    if (ptr != nullptr) error_report();
    ptr = XmlHelper::xml_convert_ptr( c);
    if (strcmp(ptr,c) !=0)  error_report();
  }

};
#endif