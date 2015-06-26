chdir '/' or die "Can't chdir to root directory: $!";
exec 'svn help' or die "Can't exec ls: $!";