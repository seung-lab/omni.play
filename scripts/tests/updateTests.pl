#!/usr/bin/perl -w

use strict;
use XML::Simple;
use Cwd 'abs_path';
use File::Basename;
use File::Copy;
use POSIX;
use Time::HiRes;
use Text::Template;
use Safe;
use File::Temp qw/tempfile tempdir/;
use CGI;
use Statistics::Descriptive;

$| = 1;  # autoflush

my $perf_sample_size = 6;
my $perf_limit = 0.10;				# Allow 10% slop on perf errors.

my $developing = 0;		# If set, user is trying to develop a test.
if(grep { $_ eq '-d' } @ARGV) {
    $developing = 1;
}

my $override = 0;		# If set, user is doesn't want to bootstrap.
if(grep { $_ eq '-o' } @ARGV) {
    $override = 1;
}

my $cluster_build = 0;
if(`hostname -d` =~ /mit/){
	$cluster_build = 1;
}

my ($script_name, $script_path, $script_suffix) = fileparse( abs_path( $0 ) );

my $root = "/home/buildbox/tests";
my $main_output = $root."/www/testsOutput";
my @folders = ("omni.src.exp",
	       "omni.src.dev",
	       "omni.src.stable",
	       "omni.src.staging",
	       "omni.src.trunk" );

our($SUCCESS);
my $outFilePath;	# The location the test will output its files.
my(@reasons);

my $bootstrap_options = "5";
if(!$override){
	$bootstrap_options = "6";
}
sub main {
    while(1){
	runFolders();
	print "find $main_output -name '*omni.files' -mtime +1 | xargs rm -r\n";
	print `find $main_output -name '*omni.files' -mtime +1 | xargs rm -r`;
        $bootstrap_options = "5";
    }
}


sub runFolders {
    foreach my $f (@folders) {
	my $start = Time::HiRes::gettimeofday();

	print "******************************************************\n";
	updateFolder( $f, strftime('%d-%b-%Y.%H.%M',localtime).$start );

	my $end = Time::HiRes::gettimeofday();
	printf("(completed in %.2f seconds)\n", $end - $start);
    }
}

sub updateFolder {
	my $folder = $_[0];
	my $test_stamp = $_[1];
	my $src_path = $root."/".$folder;   
	print $src_path."\n";

	return if !-e $src_path;

	print "==> updating src: svn up $src_path\n";
	`svn up $src_path`;
	print "done\n";

	my $xml_log = `svn log -l 1 --with-no-revprops --xml $src_path`;
	if ($xml_log =~ /revision/) {
		my $xs1 = XML::Simple->new();
		my $doc = $xs1->XMLin($xml_log);
		my $svn_ver = $doc->{logentry}->{revision};
		print "\tat revision: ".$svn_ver."\n";

		my $output_folder = $main_output."/".$folder."/".$svn_ver;
		if( -d $output_folder ){
	    		#print "skipping...\n";
	    		#return;
		}

		my $output_folder_base = $main_output."/".$folder;

		print "==> building ...\n\twarnings:\n";
		print "perl $src_path/bootstrap.pl $bootstrap_options";
		chdir($src_path);
		print `perl bootstrap.pl $bootstrap_options`;
		if($cluster_build){
			chdir("$src_path/omni");
			print `./qmakeCluster.sh`;
			chdir($src_path);
		}

		print "==> running test...\n\twarnings:\n";
		runTests( $svn_ver, $output_folder, $main_output."/".$folder."/", $test_stamp, $src_path );
	} else {
		print "warning: $xml_log\n";
	}

	print "\n\ndone!\n";
}

sub runTests {
    my $svn_ver = $_[0];
    my $output_folder = $_[1];
    my $output_folder_root = $_[2];
    my $build_stamp  = $_[3];
    my $src_path = $_[4];

    #print "$src_path/scripts/tests/t/\n";
    my @test_files = ();
    if($developing) {
   	@test_files = <$script_path/t/*>;
    } else {
        @test_files = <$src_path/scripts/tests/t/*>;
    }
    foreach my $inFileName (@test_files) {
	@reasons = ();
	my $inFileNameBase = basename($inFileName);
    	#print $inFileName . "\n";

    	$outFilePath = "$output_folder/$inFileNameBase/$build_stamp";
	#print "mkdir -p $outFilePath\n";
	`mkdir -p $outFilePath`;
    	my $outFileName = "$outFilePath/test.cmd";
    	my $markupFileName = "$outFilePath/index.html";

	open TESTLOG, ">>", "$outFilePath/test.log" or die "can't write to log file";

        #open IN_FILE, "<", $inFileName or print "could not read $inFileName";
	my $template = Text::Template->new(TYPE => 'FILE',  SOURCE => $inFileName);

	my $replacements = { DATADIR => $root."/data/",
	    		     OUTDIR => $outFilePath,
			     TESTNAME => $inFileNameBase,
			     TESTLOG => "$outFilePath/test.log"
	};

	our $err;
	#print $template->fill_in(HASH => $replacements);
	our $test = $template->fill_in(HASH => $replacements);
	our(%OMNITEST);
	{
  		#my $comp = Safe->new("OMNITEST");
		#$comp->permit('backtick', 'print');
  		#my $rc = $comp->reval($test);
		my $rc = eval($test);

		$err = main::checkEvalError($rc, $@);
        
		#####################
		## STAGE ONE ########
		#####################
        	if(defined($OMNITEST{'prep'})){
                	#print "$OMNITEST::OMNITEST{'prep'}\n";
                	#my $prep = Safe->new("OMNITESTPREP");
                	#$prep->mask(Opcode::empty_opset());
                	#my $rcprep = $prep->reval($OMNITEST::OMNITEST{'prep'});
			my $rc = eval($OMNITEST{'prep'});
			main::checkEvalError($rc, $@);
                	if(defined($SUCCESS)){
                        	#print "success: $SUCCESS\n";
                        	print TESTLOG "success: $SUCCESS\n";
                	} else {
				print TESTLOG "failure?\n";
                	}
		}
	}
	if(!$err){
		#####################
		## STAGE TWO ########
		#####################
		if(defined($OMNITEST{'cmd'})){
  			open OUT_FILE, ">", $outFileName or die "could not write $outFileName";
			print OUT_FILE $OMNITEST{'cmd'};
			print "==> running test $inFileNameBase...\n\twarnings:\n";
                       	print TESTLOG "==> running test $inFileNameBase...\n\twarnings:\n";
    			close OUT_FILE;
			`ps -elf >> $outFilePath/test.log 2>&1`;
			`$src_path/omni/bin/omni --headless --cmdfile=$outFileName >> $outFilePath/test.log 2>&1`;
		} 
	} else {
		print "failure: $OMNITEST{'cmd'}: $err\n";
	}

	#####################
	## STAGE THREE ######
	#####################
        if(defined($OMNITEST{'finish'})){
                #print "$OMNITEST::OMNITEST{'finish'}\n";
                #my $fin = Safe->new("OMNITESTFINISH");
		#$fin->mask(Opcode::empty_opset());
                #my $rcfin = $fin->reval($OMNITEST::OMNITEST{'finish'});
		$SUCCESS = undef;
	 	my $rc = eval($OMNITEST{'finish'});
		main::checkEvalError($rc, $@);
		if(defined($SUCCESS) && $SUCCESS){
			#print "success: $SUCCESS\n";
			print TESTLOG "success: $SUCCESS\n";
		} else {
			if(defined($SUCCESS)){
				print TESTLOG "failure?: $SUCCESS\n";
				recordFailure("automation-failure",
						$output_folder_root,
						$svn_ver, 
						$inFileNameBase, 
						$outFilePath,
						$SUCCESS);
			} else {
				print TESTLOG "failure?:\n";
				recordFailure("automation-failure",
						$output_folder_root,
						$svn_ver, 
						$inFileNameBase,
						$outFilePath);
				close TESTLOG;		# Forcing a reinspection of the log.
				open TESTLOG, ">>", "$outFilePath/test.log" or die "can't write to log file";
				sane();
			}
		}
        }

	#####################
	## STAGE FOUR #######
	#####################
        if(defined($OMNITEST{'markup'})){
        	#print "$OMNITEST{'markup'}\n";
		our($MARKUP);
		{
			my $rcmu = eval($OMNITEST{'markup'});
                	main::checkEvalError($rcmu, $@);
                	if(defined($MARKUP)){
                        	#print "success: $MARKUP\n";
                        	print main::TESTLOG "success: $MARKUP\n";
				open MARKUP, ">", $markupFileName or die "can't open index file: $1";
                        	print MARKUP "$MARKUP\n";
				close MARKUP;
                	} else {
                        	print main::TESTLOG "failure?\n";
                	}
		}
        }
	#####################
	## STAGE FIVE #######
	#####################
       	if(open MYPERF, "<", "$outFilePath/test.perf") {
		my $perf = <MYPERF>;
		my @ucls;
		close MYPERF;
		my @vals = split(/ /, $perf);
		my $vc = scalar(@vals);
		print scalar(@vals)."\n";

		if(open UCL, "<", "$main_output/$inFileNameBase-$vc.ucl") {
			my $ucl = <UCL>;
			@ucls = split(/ /, $ucl);
			close UCL;
		} else {
   			my @perf_files = <$main_output/*/*/$inFileNameBase/*/test.perf>;
			my @stats;
			for(my $i = 0; $i < scalar(@vals); $i++){
				$stats[$i] = Statistics::Descriptive::Full->new();
			}
			my $inccount = 0;
        		foreach my $perf_file (@perf_files) {
                		if(open PERF, "<", $perf_file) {
                       			my $perf = <PERF>;
					my @old = split(/ /, $perf);
					if(scalar(@old) == scalar(@vals)) {
						for(my $i = 0; $i < scalar(@vals); $i++){
							$stats[$i]->add_data($old[$i]);
						}
						$inccount++;
					}
                       			close PERF;
                		}
			}
			if($inccount > $perf_sample_size) {
				for(my $i = 0; $i < scalar(@vals); $i++){
					my $var = $stats[$i]->variance();
					my $mean = $stats[$i]->mean();
					$ucls[$i] = $mean + sqrt($var) * 3;
					print " > $main_output/$inFileNameBase-$vc.ucl\n";
                			if(open UCL, ">", "$main_output/$inFileNameBase-$vc.ucl") {
						print UCL join(" ", @ucls);
					}
				}
			}
		}
		if(open PERFHTML, ">>", "$outFilePath/perf.html"){
			if(defined($ucls[0])){
                        	for(my $i = 0; $i < scalar(@vals); $i++){
                                	if($vals[$i] > $ucls[$i] * (1.0+$perf_limit)) {
						if(open UCLWARN, "<", "$main_output/$inFileNameBase-$vc.ucl.$i.WARNING") {
							my $old = <UCLWARN>;
							close UCLWARN;
							print PERFHTML "<br>FAILURE: $i: over ucl ($ucls[$i]) $vals[$i] and $old!<br>\n";
                                			recordFailure("performance-failure",
                                                			$output_folder_root,
                                                			$svn_ver,
                                                			$inFileNameBase,
                                                			$outFilePath);

						} elsif(open UCLWARN, ">", "$main_output/$inFileNameBase-$vc.ucl.$i.WARNING") {
							print UCLWARN $vals[$i];
							close UCLWARN;
							print PERFHTML "<br>Warning: $i: over ucl ($ucls[$i]) $vals[$i]<br>\n";
                                                        recordFailure("performance-warning",
                                                                        $output_folder_root,
                                                                        $svn_ver,
                                                                        $inFileNameBase,
                                                                        $outFilePath);

						} else {
							print "WARNING: couldn't create ucl warning file.\n"
						}
					} else {
						unlink("$main_output/$inFileNameBase-$vc.ucl.$i.WARNING");
						print PERFHTML "<br>Success: variable $i with $vals[$i] (< $ucls[$i]).<br>\n";
					}
                        	}
			} else {
                                for(my $i = 0; $i < scalar(@vals); $i++){
					print PERFHTML "<br>Variable $i: $vals[$i]<br>\n";
				}
			}
			close PERFHTML;
		}
	}
    }
}

sub checkEvalError {
	my $rc = $_[0];
	my $code = $_[1];
	my $err = 0;
        if ($code) {
                $err = "ERROR: Failure compiling '' - $code";
		print "$err\n";
        } elsif (! defined($rc)) {
                $err = "ERROR: Failure reading '' - $!";
		print "$err\n";
        } elsif (! $rc) {
                $err = "ERROR: Failure processing ''";
		print "$err\n";
        }
	return $err;
}

sub recordFailure {
	my $type = $_[0];
	my $top = $_[1];
	my $svn_rev = $_[2];
	my $testname = $_[3];
	my $outdir = $_[4];

	foreach my $faildir ($main_output, $top, $top."/".$svn_rev, $top."/".$svn_rev."/".$testname) {
		print "echo $outdir >>$faildir/$type.txt\n";
		print `echo $outdir >>$faildir/$type.txt`;
	}
}

main();

sub start_html
{
	return CGI::start_html(@_);
}
sub a
{
	return CGI::a(@_);
}
sub h1
{
	return CGI::h1(@_) if $SUCCESS;
	return CGI::h1(@_) . why() ;
}
sub end_html
{
	return nav().CGI::end_html(@_);
}
sub why
{
	my $reasons = "";
	foreach my $reason (@reasons){
		$reasons .= " $reason<br>\n";
	}
	return $reasons;
}
sub result
{
	if(defined($SUCCESS) && $SUCCESS){
		return "SUCCESS!";
	} else {
		return "FAILURE!";
	}
}
sub files_exist_omni
{
	return (-e $_[0] && -e "$_[0].files");
}
sub files_copy_omni
{
	#print "cp $_[0] $_[1]\n";
	print `cp $_[0] $_[1]`;
	#print "mkdir $_[1].files\n";
	print `mkdir $_[1].files`;
	#print "cp -r $_[0].files/* $_[1].files/\n";
	print `cp -r $_[0].files/* $_[1].files/`;
}

sub sane
{
	open INLOG, "<", "$outFilePath/test.log" or return 0;

	while (<INLOG>) {
        	if(/([0-9]*\.[0-9]+|[0-9]+) secs(.?)/) {
			open PERFLOG, ">>", "$outFilePath/test.perf" or return 0;
        		print PERFLOG "$1 " if(/([0-9]*\.[0-9]+|[0-9]+) secs(.?)/);
			close PERFLOG;
		}

		if(/Aborted/){
			push @reasons, "$outFilePath/test.log:$. 'Aborted'";
			close INLOG;
			return 0;
		}
		if(/are different/){
			push @reasons, "$outFilePath/test.log:$. '$_'";
			close INLOG;
			return 0;
		}
	}

	close INLOG;
	return 1;
}

sub nav
{
	return "<br><a href=\"test.log\">Test Log File</><br><a href=\"test.cmd\">Test Command File</a><br><a href=\"perf.html\">Test Perf Results</a><br>";
}


