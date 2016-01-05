#** @function private _svnCommand ($cmd, @param)
# @brief	Execute a svn command
# @param	cmd Svn command to execute
# @param	param Parameters of the svn command
# @return	An array with the list of line returned by the svn command
#*
sub _svnCommand {
    my ($cmd, @params) = @_;
    my $cmd_line = "svn ${cmd} ";
    $cmd_line .= join(' ', map { $_ } @params) if @params;
    my @res = `$cmd_line 2>&1`;
    return @res;
}

_svnCommand("up");
my $branch = shift;
my @result = _svnCommand ("merge", "$branch");

#foreach my $line (reverse @result) {
#    printf "$_\n";
    #we can check the result here, TB
#}



