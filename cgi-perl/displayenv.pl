print "Content-Type: text/html\n\n";

print "<!DOCTYPE html>\n";
print "<html>\n";
print "<head>\n";
print "<meta charset='UTF-8'>\n";
print "<title>Variables d'environnement</title>\n";
print "</head>\n";
print "<body>\n";
print "<h1>Environment variable by perl scrip</h1>\n";

foreach $key (sort(keys %ENV)) {
  print "<strong>$key</strong> = $ENV{$key}<br>\n";
}
print "<p><strong>© BB & JN :-)</strong></p>\n";
print "</body>\n";
print "</html>\n";