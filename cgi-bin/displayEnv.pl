#!/usr/bin/env perl

=head1 DESCRIPTION

printenv — a CGI program that just prints its environment

=cut

my $length = 514 + $ENV{CONTENT_LENGTH};

print "Content-Type: text/html\n";
# print "Content-Length: $length \n";
print "Connection: keep-alive\n\n";

print "<HTML>\n";
print "<HEAD><TITLE>Variables d'environnement</TITLE></HEAD>\n";
print "<BODY>\n";

foreach $key (sort(keys %ENV)) {
  print "$key = $ENV{$key}<BR>\n";
}
print "</BODY>\n";
print "</HTML>\n";