/*******************************TRICK HEADER******************************
PURPOSE: (
  Provide a tagged ring-buffer. Each element of the buffer vector found
  in the Simple Ring Buffer now contains 2 elements:
   -- a tag
   -- the data that would typically be found as the data-type of a
      normal implementation of a ring buffer.
  )

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2023) (ANTARES) (initial))
  )

ASSUMPTIONS:
 ((The data type -- template-parameter, Data -- must have an operator=))

 NOTES:
 ((Boost provides a ring buffer implementation.))
**********************************************************************/
#ifndef CML_TAGGED_RING_BUFFER
#define CML_TAGGED_RING_BUFFER

#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "simple_ring_buffer.hh"
#include <cmath>

/*****************************************************************************
CMLTaggedRingBufferMember
Purpose:
  The data structure put into the ring-buffer; comprises a tag and an
  accompanying data structure.
*****************************************************************************/
template <typename T_Tag, typename T_Data>
struct CMLTaggedRingBufferMember
{
  T_Tag tag; /* (--)
    The tag to be attached to a data-set to facilitate future retrieval of
    a data-set. */
  T_Data data; /* (--)
    The data-set associated with "tag". */

  CMLTaggedRingBufferMember()
    :
    tag(),
    data()
  {};
};

/*****************************************************************************
CMLTaggedRingBuffer
Purpose:
  An extension of the SimpleRingBuffer, allowing for the tagging of each
  element in the buffer.
*****************************************************************************/
template <typename T_Tag, typename T_Data>
class CMLTaggedRingBuffer : public CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >
{
 public:
  bool require_exact_tag; /* (--)
    When looking for a data-set with the specified tag, this flag determines
    whether the model looks for the data-set with a tag closest in value to
    the specified tag (false, default), or for a data-set with a tag identical
    to the specified tag (true).*/


  CMLTaggedRingBuffer( std::string name)
    :
    CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >(name),
    require_exact_tag(false)
  {};
  virtual ~CMLTaggedRingBuffer(){};


/*****************************************************************************
buffer_data
Purpose: Adds the data and its tag to the buffer
Note:
  This could also be achieved by inheritance -- construct an instance of
  the CMLTaggedRingBufferMember and call the CMLSimpleRingBuffer::buffer_data
  method, passing that constructed member.
  But this process would require an extra copy operation to populate the
  intermediate instance.
  The code is ugly to read, but simplified with the use of aliases.
*****************************************************************************/
  virtual void buffer_data( T_Tag t,
                            const T_Data & d)
  {
    // Create aliases to the tagged data-type, and to the buffer base-type:
    // Note -- aliases are only to make the code easier to read.
    using DataMember = CMLTaggedRingBufferMember< T_Tag, T_Data>;
    using SimpleBuf = CMLSimpleRingBuffer< DataMember >;

    // advance the index, using the method defined in the simple-ring-buffer.
    SimpleBuf::advance_index();
    // get a reference to the element of the vector-of-buffered-content
    // that is about to be overwritten:
    DataMember & data_ = SimpleBuf::data[SimpleBuf::buffer_ix];
    // overwrite that member's tag and data elements:
    data_.tag  = t;
    data_.data = d;
  }

/*****************************************************************************
set_buffer_size
Purpose:  Access to the set_buffer_size method in CMLSimpleRingBuffer
*****************************************************************************/
  void set_buffer_size (size_t new_buffer_size)
  {
    CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >::
                                         set_buffer_size(new_buffer_size);
  }

/*****************************************************************************
identify_previous_index
Purpose:  Access to the identify_previous_index method in CMLSimpleRingBuffer
*****************************************************************************/
  size_t identify_previous_index (size_t n) const
  {
    return
      CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >::
                                                   identify_previous_index(n);
  }

/*****************************************************************************
lookup_tag
Purpose:
  Obtain the data from the buffer with a tag matching, or most
  closely matching, the specified tag.
  Default behavior is to identify the data set with tag closest to the
  specified tag. If using a continuous (e.g. floating-point) data-type for the
  tag, this should be the configuration.
  If  using a discrete data-type for the tag, there is the option to set the
  require_exact_tag flag and only return a data-set from the buffer when the
  specified tag matches with a buffered tag.
Note:
  There is no internal checking that tags are not reused. If the same tag is
  attached to multiple buffered data-sets, this method will return the first
  data-set encountered.
*****************************************************************************/
  const T_Data * lookup_tag( T_Tag t) const
  {
    // set references as aliases to make coding easier to read:
    const std::vector<CMLTaggedRingBufferMember< T_Tag, T_Data> > & data_ =
      CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >::data;
    const std::string & name_ =
      CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >::name;
    const size_t & buffer_ix_ =
      CMLSimpleRingBuffer< CMLTaggedRingBufferMember< T_Tag, T_Data> >::buffer_ix;

    if ( data_.empty()) {
      CMLMessage::error( __FILE__,__LINE__,
        "In buffer ",name_,", there are no entries.\n"
        "Returning NULL.\n");
      return nullptr;
    }

    /* start searching backward from the current buffer_ix for the desired
     * tag match.*/
    size_t num_buffers = data_.size();

    /* If we require an exact tag match, stop searching and return on the
     * first match encountered --
     * i.e. the most recent buffered data set with matching tag.
     *  Try the current index and keep looping until we find the tag, or get
     *  back to the current index.
     *  Note:
     *   It would be unusual to have floating-point tags AND require exact
     *   tag matches, but not illegal to do so.
     *   If the value t is computed, it is unlikely that a perfect match
     *   will be found from the available tags and the code will almost
     *   certainly drop down to the error message. This is probably a
     *   user-misconfiguration.
     *   If the value t is assigned from a set of predefined vlaues, it is
     *   feasible that this is a legitimate use of requiring equality between
     *   floating-point values so this is permitted for floating-point
     *   specializations.
     *   The use of is_equal is thus appropriate in this context.*/
    if (require_exact_tag) {
      for (unsigned int ii = 0; ii < num_buffers; ii++) {
        size_t search_ix = identify_previous_index(ii);
        if (MathUtils::is_equal( data_[search_ix].tag,
                                 t)) {
          return &data_[search_ix].data;
        }
      }

      // If we get back to buffer_ix_ without returning, no match was found.
      CMLMessage::error( __FILE__,__LINE__,
        "In buffer ", name_,", could not find an exact match to tag ",t,"\n"
        "Cannot identify the desired data set.\n"
        "Returning NULL.\n");
      return nullptr;
    }

    /* else: we are looking for the buffered set with the closest tag-value.
     * Unless we hit a perfect match we need to process through
     * all of the buffered values.
     * Process the most recently buffered set to get a baseline delta, then
     * process the others in reverse chronological order.*/
    size_t prox_ix = buffer_ix_;
    double tag_delta = std::abs( data_[buffer_ix_].tag - t);
    double tag_delta_min = tag_delta;

    for (unsigned int ii = 1; ii < num_buffers; ii++) {
      size_t search_ix = identify_previous_index(ii);
      // If the tag on this index is a perfect match with the desired value
      // stop searching immediately, cannot get any closer than a match.
      // Note if T_tag is a floating point value, this will almost always
      // produce false. We could specialize the template to avoid this step.
      if ( MathUtils::is_equal(data_[search_ix].tag, t)) {
        return &data_[search_ix].data;
      }
      // Otherwise, calculate the offset and evaluate whether the tag for
      // this index is closer to the target value than the tag for any other
      // index has been.
      tag_delta = std::abs( data_[search_ix].tag - t);
      if (tag_delta < tag_delta_min) {
        prox_ix = search_ix;
        tag_delta_min = tag_delta;
      }
    }
    // return the data at the index identified with prox_ix.
    return &data_[prox_ix].data;
  }
};
#endif
