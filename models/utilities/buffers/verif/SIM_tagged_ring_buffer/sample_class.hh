/*******************************TRICK HEADER******************************
PURPOSE: (An example of how the buffer might be implemented.)

PROGRAMMERS:
    (
      ((Gary Turner) (OSR) (Sep 2023) (ANTARES) (initial))
    )
**********************************************************************/

#include "cml/models/utilities/buffers/include/tagged_ring_buffer.hh"

struct TestStruc
{
  double x;
  int i;
  TestStruc() : x(1.1), i(1){};
  TestStruc( const TestStruc & rhs) : x(rhs.x), i(rhs.i){};
};

struct SampleModel
{
  TestStruc  instance;
  TestStruc  buffered_instances[10];
  double     tag;
  double     buffered_tags[10];

  size_t buffer_size;
  CMLTaggedRingBuffer<double, TestStruc> buffer;
  double lookup_tag;
  double lookup_x;
  SampleModel()
    :
    tag(1.0),
    buffer("test_buffer")
  {
    buffer.max_buffer_size = 10;
  }

  void update()
  {
    // Typically do stuff, populating instance.
    // In this case, these are being populated by the unit-test framework.
    buffer.buffer_data(tag, instance);
    buffer_size = buffer.data.size();
    unsigned int ii = 0;
    for (; ii < buffer_size && ii < 10; ii++) {
      buffered_instances[ii] = buffer.data[ii].data;
      buffered_tags[ii] = buffer.data[ii].tag;
    }
    for (; ii < 10; ii++) {
      buffered_instances[ii].x = 0;
      buffered_instances[ii].i = 0;
      buffered_tags[ii]        = 0;
    }

    const TestStruc * lookup = buffer.lookup_tag( lookup_tag);
    if (lookup != nullptr) {
      lookup_x = lookup->x;
    }
  }
};
