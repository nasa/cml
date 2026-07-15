#pragma once
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include <gmock/gmock.h>
#include <string>

namespace CMLMessage {

// Mock interface to CML's CMLMessage class
class Mock final {
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

} // namespace CMLMessage
