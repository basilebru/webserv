my $file = "./html/images/favicon.ico";


binmode STDOUT;
open (FH,'<', $file) || die "Error opening file\n";
print "Content-Type: image/x-icon\n\n";
my $buffer = "";
while (read(FH, $buffer, 1024)) {
    print $buffer;
}