#!perl -T

use Test::More tests => 1;

BEGIN {
    use_ok( 'oescript' ) || print "Bail out!
";
}

diag( "Testing oescript $oescript::VERSION, Perl $], $^X" );

