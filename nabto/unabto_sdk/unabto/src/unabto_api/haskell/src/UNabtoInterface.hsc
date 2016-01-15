{-# LANGUAGE ForeignFunctionInterface, CPP #-}

#include <bindings.dsl.h>
#include <unabto_api/unabto_api.h>
#include <unabto_api/unabto_api_platform.h>
#include <unabto/unabto_stream.h>

module UNabtoInterface where
#strict_import

import Foreign
import Foreign.Ptr
import Foreign.C.Types
import Foreign.C.String

#starttype application_request_t
#field query_id , Word32
#field client_id , CString
#field is_local , Word8
#stoptype

#starttype buffer_t
#field size, CSize
#field data, Ptr Word8
#stoptype

#starttype buffer_read_t
#field buffer, Ptr <buffer_t>
#field available, CSize
#field offset, CSize
#stoptype

#starttype buffer_write_t
#field buffer, Ptr <buffer_t>
#field used, CSize
#stoptype

#num UNABTO_API_OK
#num UNABTO_API_FAIL

#opaque_t nabto_stream_handle_t

-- #starttype nabto_stream_handle_t
-- #field ptr, Ptr <nabto_stream> 
-- #field id, Word16
-- #stoptype 

-- platform functions
#ccall unabto_api_platform_init , IO (CInt)
#ccall unabto_api_platform_start , IO (CInt)
#ccall unabto_api_platform_stop , IO (CInt)
#ccall unabto_api_platform_set_name , CString -> IO (CInt)

-- application level functions
#callback applicationRequestCallback , Ptr <application_request_t> -> Ptr <buffer_read_t> -> IO ()
#ccall unabto_api_register_query_handler , <applicationRequestCallback> -> IO (CInt)
#ccall unabto_api_query_response , Ptr <application_request_t> -> CInt -> Ptr <buffer_write_t> -> IO (CInt) 

#ccall unabto_api_stream_accept , Ptr (Ptr <nabto_stream_handle_t>) -> IO (CInt)
#ccall unabto_api_stream_close , Ptr <nabto_stream_handle_t> -> IO (CInt)
#ccall unabto_api_stream_release , Ptr <nabto_stream_handle_t> -> IO (CInt)
#ccall unabto_api_stream_read , Ptr <nabto_stream_handle_t> -> Ptr Word8 -> CSize -> Ptr CSize -> IO (CInt)
#ccall unabto_api_stream_write , Ptr <nabto_stream_handle_t> -> Ptr Word8 -> CSize -> Ptr CSize -> IO (CInt)

