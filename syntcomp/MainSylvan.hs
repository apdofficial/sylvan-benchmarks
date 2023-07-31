import Data.Monoid 

import Options.Applicative as O

import SylvanSolver

main = execParser opts >>= run
    where
    opts   = info (helper <*> parser) (fullDesc <> progDesc "Solve the game specified in INPUT" <> O.header "Simple BDD solver")
    parser = Options <$> flag False True (long "quiet" <> short 'q' <> help "Be quiet")
                     <*> O.option auto (long "threads" <> short 'n' <> help "Number of threads (1)" <> value 1)
                     <*> O.option auto (long "nodes_threshold" <> long "nt" <> help "Reordering nodes threshold (0)" <> value 0)
                     <*> O.option auto (long "table_ratio" <> long "tr"  <> help "Unique vs computer table es ratio (10)" <> value 10)
                     <*> O.option auto (long "table_size" <> long "ts" <>  help "Max table size (34)" <> value 34)
                     <*> O.option auto (long "max_var" <> long "mv" <>  help "Maximum number of variable swaps during the reordering. (100)" <> value 100)
                     <*> O.option auto (long "max_swap" <> long "ms" <>  help "Maximum number of swaps during the reordering.  (10000)" <> value 10000)
                     <*> O.option auto (long "max_growth" <> long "mg" <>  help "Maximum size growth allowed during the siftign.  (1.2)" <> value 1.2)
                     <*> O.option str (long "reordering_trigger" <> long "rt" <>  help "Reordering tigger: m (manual), sa (semi-autoamtic), a (automatic), else disabled. (m)" <> value "m")
                     <*> argument O.str (metavar "INPUT")