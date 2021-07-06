my $query = "";
read( STDIN, $query, $ENV{CONTENT_LENGTH} ) == $ENV{CONTENT_LENGTH}
  or return undef;

print "Content-Type: text/html\n\n";

print "<html>\n";
print "<head><title>POST Result</title><meta charset=\"UTF-8\"></head>\n";
print "<body>\n";
print "<h1>Vous avez posté :</h1>\n";
print "<p>";
print "Content-Type: $ENV{CONTENT_TYPE}";
print "</p>\n";
print "<p>";
print $query;
print "</p>\n";
print "</body>\n";
print "</html>\n";