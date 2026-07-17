#ifndef CML_EXEC_PROTO_MOCK_HH
#define CML_EXEC_PROTO_MOCK_HH
#include "trick/exec_proto.h"

#include <gmock/gmock.h>

// Mock interface to free functions in Trick's exec_proto.h header
class ExecProtoMock final {
public:
    ExecProtoMock();
    ~ExecProtoMock();
    ExecProtoMock(const ExecProtoMock&) = delete;
    ExecProtoMock& operator=(const ExecProtoMock&) = delete;

    MOCK_METHOD(int, exec_terminate_with_return, (int, const char*, int, const char*));
};
#endif
