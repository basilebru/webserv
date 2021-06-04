#!/usr/bin/env perl

=head1 DESCRIPTION

printenv — a CGI program that just prints its environment

=cut

my $query = "";
read( STDIN, $query, $ENV{CONTENT_LENGTH} ) == $ENV{CONTENT_LENGTH}
  or return undef;

my $length = 517 + $ENV{CONTENT_LENGTH};

print "Content-Type: text/html\r\n";
print "Content-Length: $length \r\n";
print "Connection: keep-alive\r\n\r\n";

# for my $var ( sort keys %ENV ) {
#     printf "%s=\"%s\"\n", $var, $ENV{$var};
# }


# print "content-type: text/html\n\n";
print "<HTML>\n";
print "<HEAD><TITLE>Variables d'environnement</TITLE></HEAD>\n";
print "<BODY>\n";

foreach $key (sort(keys %ENV)) {
  print "$key = $ENV{$key}<BR>\n";
}
print $query;
print "</BODY>\n";
print "</HTML>\n";