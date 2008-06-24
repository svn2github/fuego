/** @page generalmodules Module Dependencies

    Libraries are shown in black boxes, applications in blue boxes.

    @dot
    digraph modules
    {
        node [shape=box]

        gtp [label="GtpEngine", color=black]

        sg [label="SmartGame", color=black]
        gtp -> sg

        go [label="Go", color=black]
        sg -> go

        sp [label="SimplePlayers", color=black]
        go -> sp

        gouct [label="GoUct", color=black]
        go -> gouct

        fuegotest [label="FuegoTest", color=blue]
        gouct -> fuegotest

        fuegomain [label="FuegoMain", color=blue]
        gouct -> fuegomain
    }
    @enddot
*/
