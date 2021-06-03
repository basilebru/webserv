#!/usr/bin/perl -w
my $file = "./html/images/42.png";
my $length = -s $file;
print "Accept-Ranges: bytes\r\n";
print "Content-Length: $length \r\n";
print "Content-Type: image/png\r\n";
print "Connection: keep-alive\r\n\r\n";

binmode STDOUT;
open (FH,'<', $file) || die "Could not open $file: $!";
my $buffer = "";
while (read(FH, $buffer, 1024)) {
    print $buffer;
}