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

        go_uct [label="GoUct", color=black]
        go -> go_uct

        gtp_uct [label="GtpUct", color=blue]
        go_uct -> gtp_uct
    }
    @enddot
*/
