import Data.Monoid 

import Options.Applicative as O

import CuddSolver

main = execParser opts >>= run
    where
    opts   = info (helper <*> parser) (fullDesc <> progDesc "Solve the game specified in INPUT" <> O.header "Simple BDD solver")
    parser = Options <$> flag False True (long "quiet"   <> short 'q' <> help "Be quiet")
                     <*> flag False True (long "noreord" <> short 'n' <> help "Disable reordering")
                     <*> flag False True (long "noearly" <> short 'e' <> help "Disable early termination")
                     <*> O.option str (long "reordering_trigger" <> long "rt" <>  help "Reordering tigger: m (manual), sa (semi-autoamtic), a (automatic), else disabled. (m)" <> value "m")
                     <*> O.option str (long "heuristic" <> long "h" <>  help "Reordering heuristic: sifting/ group" <> value "sift")
                     <*> argument O.str (metavar "INPUT")