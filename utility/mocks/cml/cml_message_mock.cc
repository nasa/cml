#include "cml_message_mock.hh"

#include <exception>
#include <gmock/gmock.h>

namespace {

CMLMessage::Mock* global_mock_ptr = nullptr;

} // namespace

namespace CMLMessage {

Mock::Mock() {
    if (global_mock_ptr != nullptr) {
        throw std::runtime_error("Multiple CMLMessage::Mock instances created for a single test!");
    }
    global_mock_ptr = this;
}

Mock::~Mock() {
    testing::Mock::VerifyAndClear(this);
    global_mock_ptr = nullptr;
}

void publish(
    PublishLevel msg_level,
    const std::string& file,
    int line,
    const std::string& text) {
    if (global_mock_ptr != nullptr) {
        global_mock_ptr->publish(msg_level, file, line, text);
    }
}

} // namespace CMLMessage
