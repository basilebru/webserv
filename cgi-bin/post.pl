#!/usr/bin/env perl

=head1 DESCRIPTION

printenv — a CGI program that just prints its environment

=cut

my $query = "";
read( STDIN, $query, $ENV{CONTENT_LENGTH} ) == $ENV{CONTENT_LENGTH}
  or return undef;

my $length = 142 + $ENV{CONTENT_LENGTH};

print "Content-Type: text/html\r\n";
print "Content-Length: $length \r\n";
print "Connection: keep-alive\r\n\r\n";

print "<html>\n";
print "<head><title>Variables d'environnement</title><meta charset=\"UTF-8\"></head>\n";
print "<body>\n";
print "<h1>Vous avez posté :</h1>\n";
print "<p>";

print $query;

print "</p>\n";
print "</body>\n";
print "</html>\n";