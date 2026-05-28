/*******************************TRICK HEADER******************************
PURPOSE: (An example of how the buffer might be implemented.)

PROGRAMMERS:
    (
      ((Gary Turner) (OSR) (Sep 2023) (ANTARES) (initial))
    )
**********************************************************************/

#include "cml/models/utilities/buffers/include/simple_ring_buffer.hh"

struct TestStruc
{
  double x;
  int i;
  TestStruc() : x(1.1), i(1){};
};

struct SampleModel
{
  TestStruc  instance;
  TestStruc  output[10];
  size_t buffer_size;
  CMLSimpleRingBuffer<TestStruc> buffer;
  SampleModel()
    :
    buffer("buffer")
  {
    buffer.max_buffer_size = 10;
  }

  void update()
  {
    // Typically do stuff, populating instance.
    // In this case, these are being populated by the unit-test framework.
    buffer.buffer_data(instance);
    buffer_size = buffer.data.size();
    unsigned int ii = 0;
    for (; ii < buffer_size && ii < 10; ii++) {
      output[ii] = buffer.data[ii];
    }
    for (; ii < 10; ii++) {
      output[ii].x = 0;
      output[ii].i = 0;
    }
  }
};
