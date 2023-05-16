import Data.Monoid 

import Options.Applicative as O

import SylvanSolver

main = execParser opts >>= run
    where
    opts   = info (helper <*> parser) (fullDesc <> progDesc "Solve the game specified in INPUT" <> O.header "Simple BDD solver")
    parser = Options <$> flag False True (long "quiet" <> short 'q' <> help "Be quiet")
                     <*> O.option auto (long "threads" <> short 'n' <> metavar "N" <> help "Number of threads" <> value 4)
                     <*> argument O.str (metavar "INPUT")