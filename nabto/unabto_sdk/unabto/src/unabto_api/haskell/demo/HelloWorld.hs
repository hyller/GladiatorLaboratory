import UNabto

import Control.Concurrent

import UNabtoHelper

import qualified Data.ByteString.Lazy as DBL

hw = unabto $ defaultConfig { serverId = "a.starterkit.u.nabto.net", 
                              handlers = [(1, lightWrite), (2, lightRead)]}

asyncWrite a = do
  threadDelay 1000
  return (ApplicationRequestOk, (DBL.pack ([DBL.index a 1])))

lightWrite a = do
  let lightId = DBL.index a 0
      lightOn = DBL.index a 1
  putStrLn $ "light write " ++ (show lightId) ++ " " ++ (show lightOn)
  return (ApplicationRequestOk, DBL.pack [lightOn])
  
lightRead a = do
  let lightId = DBL.index a 0
  putStrLn $ "lightId " ++ (show lightId)
  return (ApplicationRequestOk, DBL.pack [0])
