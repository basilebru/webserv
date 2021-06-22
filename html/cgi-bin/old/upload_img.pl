#!/usr/bin/perl
use strict;
use warnings;
use CGI;

my $output   = '';
my $query    = CGI->new();
my $filename = $query->param("file");
my $upload   = $query->upload("file");
$output     .= "Temp file: " . $query->tmpFileName($filename) . "\n";

if (!$upload) {
    $output .= "<p>CGI.pm failed to upload the file.</p>";
}
else {
    $output .= "----- Contents: ------\n";
    while (<$upload>) {
        $output .= $_;
    }
}
print "Content-type: text/plain\n\n";
print $output;
