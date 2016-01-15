module TChanHelper (getListFromTChan) where

import Control.Concurrent.STM.TChan
import Control.Concurrent.STM


getListFromTChan :: TChan a -> STM [a] 
getListFromTChan chan = getListFromTChanActual chan []

getListFromTChanActual :: TChan a -> [a] -> STM [a]
getListFromTChanActual chan ls = do  
  isEmptyTChan chan >>= 
    (\a ->  
      case a of
        True -> return ls
        False -> readTChan chan >>= (\a -> getListFromTChanActual chan (a:ls)))
