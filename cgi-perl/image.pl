my $file = "./html/images/favicon.ico";

binmode STDOUT;
open (FH,'<', $file) || die "Status: 500\n\n";
print "Content-Type: image/x-icon\n\n";
my $buffer = "";
while (read(FH, $buffer, 1024)) {
    print $buffer;
}