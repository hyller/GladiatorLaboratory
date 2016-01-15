{-# LANGUAGE DeriveDataTypeable #-}
module UNabtoSync where

import UNabtoInterface
import UNabtoHelper
import TChanHelper

import Foreign.C.String
import Foreign.C.Types
import Foreign.Ptr
import Foreign.Storable
import Foreign.Marshal.Alloc
import Foreign.Marshal.Array

import Control.Concurrent
import Control.Monad
import Control.Exception
import Control.Concurrent.STM.TChan
import Control.Concurrent.STM.TVar
import Control.Concurrent.STM
import Control.Concurrent

import Data.Typeable

import qualified Data.ByteString.Lazy as DBL



  {- 
defines a synchronized unabto interface 
which implies we can call the unblocking unabto functions without considering synchronization.
-}

data UNabtoSyncState = UNabtoSyncState {
  threadIds :: TChan ThreadId,
  streamHandler :: TVar (Maybe (UNabtoStreamHandle -> IO ()))
  } deriving Show
                                            
instance Show (MVar a) where
  show a = ""
                       
instance Show (TChan a) where
  show a = ""

instance Show (TVar a) where
  show a = ""


type UNabtoStreamHandle = Ptr C'nabto_stream_handle_t

data UNabtoException =
  UNabtoStreamException |
  UNabtoStreamReadException |
  UNabtoStreamAckException
  deriving (Typeable, Show) 
  
instance Exception UNabtoException

unabtoInit :: String -> IO (UNabtoSyncState)
unabtoInit id = do 
  id' <- newCString id 
  _ <- c'unabto_api_platform_init
  _ <- c'unabto_api_platform_set_name id'
  _ <- c'unabto_api_platform_start  
  tids <- newTChanIO
  sh <- newTVarIO Nothing
  return (UNabtoSyncState tids sh)
  
-- defaultUNabtoState ss = do
--   return $ UNabtoSyncState {threadIds = tids,
--                         streamHandler = sh,
--                         syncState = ss                                        
--                         }
startUnabto id = do
  syncState <- unabtoInit id
  (forkIO $ streamHandlerLoop syncState) >>= (\a -> atomically $ writeTChan (threadIds syncState) a)
  return syncState
  
stopUnabto state = do
  tids <- atomically $ getListFromTChan (threadIds state)
  mapM killThread tids
  return ()

streamHandlerLoop state = do
  streamHandlerSpawner state
  threadDelay 1000
  streamHandlerLoop state
                         
streamHandlerSpawner syncState = do
  ret <- unabtoStreamAccept syncState
  case ret of
    Nothing -> return ()
    Just a -> spawnStream a syncState
    
spawnStream handle syncState = do
  maybeSh <- atomically $ readTVar (streamHandler syncState)
  case maybeSh of 
    Nothing -> do
      unabtoStreamClose syncState handle
      return ()
    Just sh -> do
      (forkIO $ sh handle) >>= (\a -> atomically $ writeTChan (threadIds syncState) a)
  

unabtoSetApplicationEventHandler :: (DBL.ByteString -> IO ()) -> IO()
unabtoSetApplicationEventHandler fn = do
  exportCallback $ makeCallback fn
  
unabtoStreamAccept :: UNabtoSyncState -> IO (Maybe UNabtoStreamHandle)
unabtoStreamAccept s = do
  handleptr <- malloc
  ret <- c'unabto_api_stream_accept handleptr
  handle <- peek handleptr
  case toEnum (fromIntegral ret) of 
    UNabtoOk -> return (Just handle)
    UNabtoFail -> do
      free handleptr
      return Nothing
      
--type MaxBytes = Maybe Int
unabtoStreamReadSome :: UNabtoSyncState -> UNabtoStreamHandle -> IO DBL.ByteString
unabtoStreamReadSome s handle = do
  unabtoStreamRead s handle 1024

unabtoStreamRead :: UNabtoSyncState -> UNabtoStreamHandle -> Int -> IO DBL.ByteString
unabtoStreamRead s sh maxSize = do
  buf <- mallocArray maxSize
  readSize <- malloc
  status <- c'unabto_api_stream_read sh buf (fromIntegral maxSize) readSize
  readSize' <- peek readSize
  free readSize
  case status of
    0 -> do
      res <- peekArray (fromIntegral readSize') buf
      free buf
      return $ DBL.pack res
    a -> throw UNabtoStreamReadException

unabtoStreamReadExactly :: (Integral a, Num a) => UNabtoSyncState -> UNabtoStreamHandle -> a -> IO (DBL.ByteString)
unabtoStreamReadExactly s sh len = do
  loop (fromIntegral len) DBL.empty  
  where 
    loop 0 res = return res
    loop len res = do
      threadDelay 1000
      bs <- unabtoStreamRead s sh len
      loop (len - (fromIntegral $ DBL.length bs)) (DBL.append res bs)
        
  
{-
Write some of the input bytestring and return what was not sent.
-}
unabtoStreamWrite :: UNabtoSyncState -> UNabtoStreamHandle -> DBL.ByteString -> IO (Maybe DBL.ByteString)
unabtoStreamWrite s handle d = do
  hint <- malloc
  buf <- newArray (DBL.unpack d)
  wl <- liftM fromIntegral $ c'unabto_api_stream_write handle buf (fromIntegral $ DBL.length d) hint
  free buf
  hint' <- peek hint
  free hint
  case hint' of
    0 -> return $ if (DBL.length d > wl) 
                           then (Just (DBL.take wl d))
                           else Nothing
    a -> throw UNabtoStreamException
      

unabtoStreamWriteBlocking :: UNabtoSyncState -> UNabtoStreamHandle -> DBL.ByteString -> IO ()
unabtoStreamWriteBlocking s handle bs = do
  ret <- unabtoStreamWrite s handle bs :: IO (Maybe DBL.ByteString)
  case ret of
    Nothing -> return ()
    Just a -> do
      threadDelay 10000
      unabtoStreamWriteBlocking s handle a
  
unabtoStreamClose :: UNabtoSyncState -> UNabtoStreamHandle -> IO (CInt)
unabtoStreamClose s handle = c'unabto_api_stream_close handle
  
unabtoStreamRelease :: UNabtoSyncState -> UNabtoStreamHandle -> IO (CInt)
unabtoStreamRelease s handle = c'unabto_api_stream_release handle
      
      