#!perl -T

use Test::More tests => 1;

BEGIN {
    use_ok( 'oescriptimpl' ) || print "Bail out!
";
}

diag( "Testing oescriptimpl $oescriptimpl::VERSION, Perl $], $^X" );
