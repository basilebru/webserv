#!/usr/bin/perl -w
my $file = "./html/images/42.png";
my $length = -s $file;
print "Accept-Ranges: bytes\n";
print "Content-Length: $length \n";
print "Content-Type: image/png\n";
print "Connection: keep-alive\n\n";

binmode STDOUT;
open (FH,'<', $file) || die "Could not open $file: $!";
my $buffer = "";
while (read(FH, $buffer, 1024)) {
    print $buffer;
}