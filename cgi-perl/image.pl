my $file = "./html/images/favicon.icofezfzef";

binmode STDOUT;
open (FH,'<', $file) || die "File not found\n";
print "Content-Type: image/x-icon\n\n";
my $buffer = "";
while (read(FH, $buffer, 1024)) {
    print $buffer;
}