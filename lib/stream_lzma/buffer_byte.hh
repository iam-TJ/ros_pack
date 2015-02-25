/*****************************************************************************
 * $LastChangedDate: 2014-01-12 17:54:50 -0500 (Sun, 12 Jan 2014) $
 * @file
 * @author  Jim E. Brooks
 * @brief   Byte buffer that auto-destroys itself.
 *//*
 * LEGAL:   COPYRIGHT (C) 2014 JIM E. BROOKS  http://www.jimbrooks.org
 *****************************************************************************/

#ifndef BASE_BUFFER_BYTE_HH
#define BASE_BUFFER_BYTE_HH 1

namespace base {

////////////////////////////////////////////////////////////////////////////////
/// @brief Byte buffer that auto-destroys itself.
///
/// ByteBuffer contains bytes/chars.
/// For safety, ByteBuffer allocates a margin.
/// Consider using StringBuffer instead.
///
/// Within a block, ByteBuffer can be a regular non-shptr variable.
/// If a ByteBuffer needs to be returned or passed outside of a block, turn it into a shptr.
///
/// Limiting this class to bytes guarantees Size() returns a byte count equal to element count.
/// Larger types would require sizeof() and might be subject to implementation-specific padding.
///
template<typename BYTE=byte>
class ByteBuffer final : public Shared
{
    private: CLASS_CONSTEXPR uint MARGIN = 0x100;

    /// ctor.  Count must be > 0.
    public: ByteBuffer( const uint byteCount )
    :   mBuf(new BYTE[byteCount + MARGIN]),
        mByteCount(byteCount)
    {
    ASSERT( byteCount > 0 );
  //COMPILE_TIME_ASSERT( sizeof(BYTE) == 1, "BYTE isn't 8-bits" );
    }

    /// LIMITED-USE: Creates an invalid buffer which can be checked by IfInvalid().
    public: ByteBuffer( void )
    :   mBuf(nullptr),
        mByteCount(0)
    {
    }

    /// dtor.
    public: virtual ~ByteBuffer()
    {
        delete[] mBuf; mBuf = nullptr;
    }

    /// @return False if buffer is invalid.
    public: bool IfValid( void ) const
    {
        return mByteCount > 0;
    }

    /// @return Pointer to buffer.
    public: BYTE* Get( void ) const
    {
    ASSERT( IfValid() );

        return mBuf;
    }

    /// @return Size in bytes.
    public: uint Size( void ) const
    {
  //ASSERT( IfValid() );  // allow getting size in any case

        return mByteCount;
    }

    private: BYTE* mBuf;
    private: uint  mByteCount;
};

} // namespace base

#endif // BASE_BUFFER_BYTE_HH
