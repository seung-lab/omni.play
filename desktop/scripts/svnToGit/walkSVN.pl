#!/usr/bin/perl -w

use strict;

use XML::Simple;
use Data::Dumper;

use URI::Escape;

die "please specify path" unless 1 == $#ARGV + 1;

my $DIR = $ARGV[0];
my $repos = "$DIR/netgpu";
my $xmlfile = "$DIR/svnlog.xml";
my $SVNB = "/home/mjp/bin/subversion-1.7.3/bin";
my $svnCheckout = "$DIR/netgpu.checkout";

#checkout a small folder 
`$SVNB/svn co file://$repos/omni/trunk/watershed/ $svnCheckout`;

chdir $svnCheckout;

# sample
# <logentry
#	revision="5398">
#	<author></author>
#	<date>2011-10-13T20:03:05.661455Z</date>
#	<msg>fix warning</msg>
# </logentry>

sub dumpSVNlogAsXML {
   `$SVNB/svn log --xml file://$repos/omni > $xmlfile`;
}

sub updateRevisions 
{
    dumpSVNlogAsXML();

    my $xs1 = XML::Simple->new(SuppressEmpty  => 1);
    my $doc = $xs1->XMLin($xmlfile);

    foreach my $entry (@{$doc->{logentry}}) 
    {
	my $rev = $entry->{revision};
	my $msg = $entry->{msg};

	if(!$msg){
	    $msg = "";
	}

	# when using SVN over NFS on my box, my name was never 
	#  in the authorship tag; I should be the over for almost
	#  all commits that don't have an author
	if(!exists($entry->{"author"}))
	{
	    `$SVNB/svn propset --revprop -r $rev svn:author purcaro`;
	}

	if(3574 == $rev)
	{
	    $msg = "Merged revisions 2277,2279-2282,2284-2286,2288-2290,2292-2297,2300-2303,2305-2311,2313-2325,2328-2331,2333-2339,2346-2348,2423,2501-2507,2538,2540-2541,2545-2603,2605-2664,2673-2693,2695-2738,2740-2747,2749,2751-2788,2791-2814,2820-2821,2823-2844,2855-2856,2863,2877,2880,2895,2929-2942,2947-2971,2979-2983,2987,3022,3058,3060,3067,3069,3078,3081,3085,3115-3116,3122-3134,3136,3146-3147,3160,3162-3165,3187-3189,3197,3207-3212,3215-3217,3219-3220,3223-3225,3228-3249,3252-3264,3267-3284,3286-3292,3294,3296,3298-3353,3355,3357-3367,3377-3378,3382,3384,3386,3412,3421,3428,3431-3450,3452-3465,3467-3474,3476-3477,3479-3480,3482-3497,3500-3528,3530-3555,3557-3572 via svnmerge from file:///gaba/svn/netgpu/omni/dev";
	}

	if(2332 == $rev){
	    $msg = "new data";
	}

	if($msg =~ m/^\(original rev/){    
	} else {
	    print $rev.",";

	    $msg =~ s/\"//g;
	    $msg =~ s/\`//g;

	    my $newmsg = "(original rev $rev) ".$msg;
	    `$SVNB/svn propset --revprop -r $rev svn:log "$newmsg"`;
	}
    }
}

sub checkRevisions 
{
    dumpSVNlogAsXML();
    
    my $xs1 = XML::Simple->new(SuppressEmpty  => 1);
    my $doc = $xs1->XMLin($xmlfile);

    foreach my $entry (@{$doc->{logentry}}) 
    {
	my $rev = $entry->{revision};
	my $msg = $entry->{msg};

	if(!$msg){
	    $msg = "";
	}

	if(!exists($entry->{"author"}))
	{
	    print "missing author on rev $rev\n";
	}
	
	if($msg =~ m/^\(original rev/){    
	} else {
	    print "comment in rev $rev doesnt include rev number\n";
	}
    }
}

updateRevisions();
checkRevisions();
