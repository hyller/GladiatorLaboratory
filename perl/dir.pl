#! /usr/bin/perl 

my $dir_to_process = '.';
opendir my $dh, $dir_to_process or die "Cannot open $dir_to_process: $!";
foreach $file (readdir $dh) {
  print "one file in $dir_to_process is $file\n";
}
closedir $dh;


