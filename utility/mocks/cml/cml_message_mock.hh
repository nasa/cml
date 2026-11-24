#ifndef CML_CML_MESSAGE_MOCK_HH
#define CML_CML_MESSAGE_MOCK_HH
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>

#include "cml/models/utilities/cml_message/include/cml_message.hh"

namespace cml::message {

// Mock interface to CML's message class
class Mock {
public:
    Mock();
    ~Mock();
    Mock(const Mock&) = delete;
    Mock& operator=(const Mock&) = delete;

    MOCK_METHOD(void, publish, (
        PublishLevel msg_level,
        const std::string& file,
        int line,
        const std::string& text));
};

} // namespace message
#endif
