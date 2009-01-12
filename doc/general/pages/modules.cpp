/** @page generalmodules Module Dependencies

    Libraries are shown in black boxes, applications in blue boxes.

    @dot
    digraph modules
    {
        node [shape=box]

        gtp [label="GtpEngine"]

        sg [label="SmartGame"]
        gtp -> sg

        go [label="Go"]
        sg -> go

        sp [label="SimplePlayers"]
        go -> sp

        gouct [label="GoUct"]
        go -> gouct

        fuegotest [label="FuegoTest", style=bold]
        gouct -> fuegotest
        sp -> fuegotest

        fuegomain [label="FuegoMain", style=bold]
        gouct -> fuegomain
    }
    @enddot
*/
