module UNabto where

import UNabtoSync

import Data.Word
import Data.Binary.Get
import qualified Data.ByteString as DB
import qualified Data.ByteString.Lazy as DBL

import UNabtoHelper

import System.Exit
import qualified System.Posix.Signals as Signals
import Control.Concurrent

type Handler = (Word32, (DBL.ByteString -> IO()))

data Config = Config {
  serverId :: String,
  handlers :: [Handler]
}

defaultConfig = Config "foo.u.nabto.net" []

-- unabto :: Config -> IO (UNabtoSyncState)
-- unabto (Config serverId hs) = do
--   unabtoSetApplicationEventHandler (listToApplicationEventHandler hs)
--   startUnabto serverId 
  
-- listToApplicationEventHandler list = do
--   (\a -> do
--       let queryId = runGet getWord32be a
--       let hs = findHandler queryId list
--       case hs of
--         [] -> return (DBL.empty, ApplicationRequestNoQueryId)
--         ((_,hs):_) -> do
--           (res,bs) <- hs (DBL.drop 4 a)
--           return (bs,res))

findHandler qId list = filter (\(a,b) -> a == qId) list