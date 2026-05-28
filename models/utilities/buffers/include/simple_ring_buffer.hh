/*******************************TRICK HEADER******************************
PURPOSE: (
  Provide a simple ring-buffer. A new data member overwrites the oldest
  data member and the class keeps track of where the newest data member is
  located.
  Buffer structure is tempalted to provide a vector of any struct or class.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (initial))
  )

ASSUMPTIONS:
 ((The data type -- template-parameter, T -- must have an operator=))

 NOTES:
  STL provides a ring buffer with C++14.
**********************************************************************/
#ifndef CML_SIMPLE_RING_BUFFER
#define CML_SIMPLE_RING_BUFFER

#include <vector>
#include <string>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

template <typename T>
class CMLSimpleRingBuffer {
 public:
  std::string name; /* (--)
    Name of buffer, used for error reporting.*/
  std::vector<T> data;  /* (--)
    A cycling buffer of recorded values. Size is adjustable. */
  size_t buffer_ix;  /* (--)
    The index of the most recently populated set in the buffer.*/
  size_t max_buffer_size; /* (--)
    A settable limit to the buffer size. Prevents accidentially resizing the
    buffer to be something ridiculously large.*/


/*****************************************************************************
Constructor/ destructor
*****************************************************************************/
  CMLSimpleRingBuffer(std::string name_)
    :
    name(name_),
    data(),
    buffer_ix(0),
    max_buffer_size( data.max_size())
  {
    data.resize(1);
  }
  virtual ~CMLSimpleRingBuffer(){};

/*****************************************************************************
advance_index
Purpose:
  Allows for moving the index along externally.
  If populating the next buffer slot with an instance of T, using buffer_data,
  this is not needed (called automatically).
  If populating the next buffer slot with an external assignment, the index
  should be advanced first by calling this method,
*****************************************************************************/
  void advance_index() {
    buffer_ix = (buffer_ix+1)%(data.size());
  }

/*****************************************************************************
buffer_data
Purpose:
  overwrites the oldest data set in the buffer with new data.
*****************************************************************************/
  virtual void buffer_data( const T& data_in) {
    advance_index();
    data[buffer_ix] = data_in;
  }

/*****************************************************************************
set_buffer_size
Purpose:
  Supports modifying the buffer size.
*****************************************************************************/
  void set_buffer_size (size_t new_buffer_size) {
    size_t old_buffer_size = data.size();
    // Trivial case of not changing the size requires no action.
    if (new_buffer_size == old_buffer_size) {
      return;
    }
    if (new_buffer_size == 0) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot set the buffer size to zero.\n"
        "Instruction ignored for simple ring buffer ",name,".\n");
      return;
    }
    if (new_buffer_size > max_buffer_size) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot set the buffer size to > ",max_buffer_size,".\n"
        "Buffer size being reset to ",max_buffer_size,
        " for simple ring buffer ",name,".\n");
      set_buffer_size(max_buffer_size);
      return;
    }

    /* Buffer data values are entered chronologically.
       Starting at buffer_ix and working backwards, we get a progression of
       buffer values working backwards in time.
      - If adding new slots, need to move those entries with indices > buffer_ix
        towards the new end of the buffer, opening up blank slots between
        buffer_ix and the oldest recorded buffer values (which are currently
        at index buffer_ix+1.
      - If removing slots, need to make sure that the oldest values are
        getting removed.*/

    /* Enlarging buffer:
       New slots to be opened immediately to the right of buffer_ix.
       If buffer_ix addresses the last slot in the old buffer,
       there is nothing to move; simply extend the buffer.
       Otherwise, move the values currently to the right of buffer_ix (the oldest
       members of buffer) to the end of the new buffer and fill in with default
       instances of template-type T.*/
    if (new_buffer_size > old_buffer_size) {
      data.resize(new_buffer_size);
      if (buffer_ix != (old_buffer_size -1)) {
        size_t offset = new_buffer_size - old_buffer_size;
        for (size_t ii = old_buffer_size-1; ii > buffer_ix; --ii) {
          data[ii + offset] = data[ii];
        }
        T default_instance;
        size_t buffer_ix_0 = buffer_ix;
        for (unsigned int ii = 0; ii < offset; ++ii) {
          buffer_data(default_instance);
        }
        // adding new data will advance buffer_ix; reset it back to where it was
        buffer_ix = buffer_ix_0;
      }
    }

    /* Otherwise, the buffer is being reduced.
       If the current index (buffer_ix) will remain in the newly sized buffer,
       then we just need to remove the oldest values to its right.
       To do this, we copy over the oldest members with members further to the
       right such that the content of the last slot of the old buffer shifts to
       the last slot of the new buffer.
       Everything to the left of buffer_ix is unaffected.
       Note -- special case, new_buffer_size = buffer_ix + 1, the for-loop
               never enters and the buffer is chopped imediately after buffer_ix.
    */
    else if (new_buffer_size > buffer_ix) {
      size_t offset = old_buffer_size - new_buffer_size;
      for (size_t ii = buffer_ix + 1; ii < new_buffer_size; ++ii) {
        data[ii] = data[ii + offset];
      }
      data.resize(new_buffer_size);
    }

    /* Finally, if the buffer is being reduced and the current index (buffer_ix)
       would not remain in the new buffer, we need to slide everything to the
       left such that buffer_ix occupies the last slot in the new buffer.*/
    else {
      size_t offset = buffer_ix - new_buffer_size + 1;
      for (size_t ii = 0; ii < new_buffer_size; ++ii) {
        data[ii] = data[ii + offset];
      }
      data.resize(new_buffer_size);
      buffer_ix = new_buffer_size -1;
    }
  }


/*****************************************************************************
identify_previous_index
Purpose:
  Identifies the index of the buffer laid down n steps previous.
*****************************************************************************/
  size_t identify_previous_index( size_t n) const
  {
    if (n > data.size()) {
      CMLMessage::error( __FILE__,__LINE__,
        "Cannot step backward ",n," steps because the buffer only contains ",
        data.size(),"entries.\n Returning current buffer index.\n");
      return buffer_ix;
    }
    return (n <= buffer_ix) ? buffer_ix - n :
                              data.size() - (n - buffer_ix);
  }

};
#endif
