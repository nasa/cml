#include "exec_proto_mock.hh"

#include <exception>
#include <gmock/gmock.h>

namespace {

ExecProtoMock* global_mock_ptr = nullptr;

} // namespace

ExecProtoMock::ExecProtoMock() {
    if (global_mock_ptr != nullptr) {
        throw std::runtime_error("Multiple ExecProtoMock instances created for a single test!");
    }
    global_mock_ptr = this;
}

ExecProtoMock::~ExecProtoMock() {
    testing::Mock::VerifyAndClear(this);
    global_mock_ptr = nullptr;
}

extern "C" int exec_terminate_with_return(int ret_code, const char* file_name, int line, const char* error) {
    if (global_mock_ptr != nullptr) {
        return global_mock_ptr->exec_terminate_with_return(ret_code, file_name, line, error);
    }
    return 1;
}
