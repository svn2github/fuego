#!/usr/bin/perl -w

use Getopt::Long;
use File::Temp qw/ tempfile /;

$verbose = 0;

$program = "fuego";
$size = 9;
$games = 50000;
$threads = 1;
$playouts = 1;
$memory = -1;
$count = 25;


sub printUsage {
    print STDERR "Usage: fuego-speed-test [options]\n";
    print STDERR "  Options\n";
    print STDERR "    --size <n>         Board size. (default $size)\n";
    print STDERR "    --games <n>        Number of games in search. (default $games)\n";
    print STDERR "    --playouts <n>     Number of playouts per game. (default $playouts)\n";
    print STDERR "    --threads <n>      Number of threads. (default $threads)\n";
    print STDERR "    --memory <n>       Set Fuego's maximum memory paramater.\n";
    print STDERR "    --count <n>        Number of tests to average. (default $count)\n";
    print STDERR "    --program <path>   Path to the Fuego executable.\n";
    print STDERR "    --verbose          Display Fuego's output.\n";
    print STDERR "    --help-config      Display the Fuego config file and exit.\n";
    print STDERR "    --help             Displays this help message.\n";
    exit 0;
}
	

GetOptions('verbose' => \$verbose,
	   'program=s' => \$program, 
           'size=i' => \$size,
           'games=i' => \$games,
           'threads=i' => \$threads,
	   'memory=i' => \$memory,
           'playouts=i' => \$playouts,
           'count=i' => \$count,
	   'help-config' => \$help_config,
           'help' => \$help);

if ($help) {
    printUsage();
}


($CONFIG, $configFilename) = tempfile( UNLINK=> 1 );

print $CONFIG "boardsize $size\n";
print $CONFIG "go_rules cgos\n";
print $CONFIG "komi 7.5\n";

print $CONFIG "book_clear\n";

if ($memory > 0) {
    print $CONFIG "uct_max_memory $memory\n"; 
}

print $CONFIG "uct_param_player ignore_clock 1\n";
print $CONFIG "uct_param_player max_games $games\n";
print $CONFIG "uct_param_player reuse_subtree 0\n";
print $CONFIG "uct_param_player forced_opening_moves 0\n";

print $CONFIG "uct_param_search lock_free 1\n";
print $CONFIG "uct_param_search number_threads $threads\n";
print $CONFIG "uct_param_search number_playouts $playouts\n";
print $CONFIG "uct_param_search move_select estimate\n";


print $CONFIG "reg_genmove b\n";
close($CONFIG);

if ($help_config) {
    open(CONFIG2, "< $configFilename");
    while (<CONFIG2>) {
	print STDOUT $_;
    }
    close(CONFIG2);
    exit 0;
}

$speedTally = 0;
for($i = 1; $i <= $count; $i++) {
    print STDERR "Test $i of $count...\n";
    $speed = 0;
    open(FUEGO, "$program <$configFilename 2>&1 |");
    while (<FUEGO>) {
	if ($verbose) {
	    print $_;
	}
	chomp($_);
	if ($_ =~ /^Games\/s/ ) {
	    @fields=split(/ +/,$_);
	    $speed = $fields[1];
	    $speedTally += $speed;
	}
    }
    close(FUEGO);
    if ($speed > 0) {
	if (!$verbose) {
	    print STDERR "  Games/s    $speed\n";
	}
    } else {
	print STDERR "error: could not find speed in output\n";
	exit 0;
    }

}

$speed = $speedTally / $count;

print STDERR "\n";
printf STDOUT "%.1f\n", $speed;
