module UNabtoHelper where

import UNabtoInterface

import Foreign.C.Types
import Foreign.Ptr
import Foreign.Storable
import Foreign.Marshal.Array
import qualified Data.ByteString.Lazy as DBL

data ApplicationRequestStatus = 
  ApplicationRequestOk |
  ApplicationRequestNoQueryId |
  ApplicationRequestFail String

data UNabtoStatus = UNabtoOk | UNabtoFail deriving Enum

makeCallback :: (DBL.ByteString -> IO ()) -> (Ptr C'application_request_t -> Ptr C'buffer_read_t -> IO ())
makeCallback f = 
  (\app_req br -> do 
      bufferRead <- peek br
      inputBufferT <- peek (c'buffer_read_t'buffer bufferRead)
      inp <- peekArray (fromIntegral $ c'buffer_read_t'available bufferRead) (c'buffer_t'data inputBufferT)
      f (DBL.pack inp))
                
exportCallback :: (Ptr C'application_request_t -> Ptr C'buffer_read_t -> IO ()) -> IO ()
exportCallback f = do
  fx <- mk'applicationRequestCallback f
  c'unabto_api_register_query_handler fx
  return ()
