#!/usr/bin/perl

my $testnum = $ENV{TESTNUM};
   $testnum = 5 unless defined $testnum;
my $testname = $ENV{TESTNAME};

my @filetypes = ('SRCS','HDRS','LEXS','YACCS','NOEXT');

my %cextensions = (SRCS  => '.gcc',
                   HDRS  => '.h',
                   LEXS  => '.l',
                   YACCS => '.y',
                   NOEXT => ''  );
my %cycextensions = (SRCS  => '.cyc',
                     HDRS  => '.h',
                     LEXS  => '.l',
                     YACCS => '.y',
                     NOEXT => ''  );

my $reportfile = 'report.diff';
my $texfile = 'report.tex';
my $bm = `pwd`;
$bm = `basename $bm`;
chomp($bm);

# This overwrites the existing file
open(REPORT,">$reportfile") || die "Opening $reportfile";
open(TEX,">$texfile") || die "Opening $texfile";

format REPORT =
  @<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<: @<<<<< @<<<<< @<<<<<
  $recname, $total, $fmt_add, $fmt_sub
.

## Global "Constants"
$END_STMT = ';';
($BEGIN_BLOCK, $END_BLOCK) = ("{", "}");
($DQUOTE, $SQUOTE) = ('"', "'");

## clc -- count the lines/ nc-loc, and statements in the given file
##        (a filename of '-' means use STDIN).
##
## returns the array (#lines, #nc-loc, #stmts).
## If the file can't be opened, then returns (-1, -1, -1)
##
# This subroutine comes from:
#
# clc -- C/C++ line-counter (actually - it counts statements too)
#
# Created 02/14/95 by Brad Appleton
sub clc {
   local($filename) = @_;
   local($filehandle);
   local($_);
   local($LineCount, $NonCmtLineCount, $StmtCount, $BlockCount);
   local($BlockLevel, $InComment, $InQuote);
   local($Style, $ContinuedQuote);
   local($isNonCmtLine, $isPpDirective, $lineStmts);

   ++$Cmts{"C++"};
   ++$Cmts{"C"};

   ## Initialize
   $LineCount = $NonCmtLineCount = $StmtCount = $BlockCount = 0;
   $BlockLevel = $InComment = $InQuote = 0;

   ## Open input
   if ($filename eq "-") {
      $filehandle = STDIN;
   } else {
      $filehandle = INPUT;
      unless (open(INPUT, $filename)) {
         &error_msg("Can't open $filename for reading: $!\n");
         return (-1, -1, -1);
      }
   }
      
   ## Loop over input lines
   ##
   ##   Note: If I actually look at every character on a line, it takes
   ##   far too much time, so what I instead do is remove all "uninteresting"
   ##   characters from a line (like alphanumerics) before I iterate over
   ##   each of its characters. This proved to be a tremendous speed-up.
   ##
   while (<$filehandle>) {
      ++$LineCount;

      if ($Cmts{"C"}) {
         ## try not to get confused by "uninteresting" stuff between
         ## a '*' and a '/', or a '/' and a '*'
         s|/([\\\w\t ]+)\*|/\~$2\~$3*|go;
         s|\*([\\\w\t ]+)/|*\~$2\~$3/|go;
      }

      ## remove anything "uninteresting"
      s/\\.//go;       ## escaped characters
      s/\s+//go;       ## whitespace
      s/\w+/A/go;      ## alphanumerics (compressed to a single letter)

      ## watchout for some special perl constructs
      s/\$#//go if ($Cmts{"perl"});

      ## skip blank lines
      next if (m/^\s*$/o);

      ## Have to figure out (eventually) whether to include this line
      ## in our count of non-comment source-lines.
      $isNonCmtLine = 0;

      ## Keep track of number of statements on this line
      $lineStmts = 0;

      ## See if this is a preprocessor directive
      $isPpDirective = ( /^\s*#/o ? 1 : 0 );

      @str = split ("");   ## split on empty string to get a character array
      for ($i = 0; $i <= $#str ; $i++) {
         if ($InComment) {
            if (($Style eq "C") && ($str[$i] eq "*") && ($str[$i+1] eq "/")) {
               ++$i;
               $InComment = 0;
               $Style = "";
            }
         } elsif ($InQuote) {
            ++$isNonCmtLine;
            $ContinuedQuote = 0;
            if (($str[$i] eq "\\") && ($str[$i+1] eq "\n")) {
               $ContinuedQuote = 1 if ($Style eq $DQUOTE);            
               last;
            } elsif ($str[$i] eq $Style) {
               $InQuote = 0;
               $Style = "";
            }
         } else {
            if ($Cmts{"C++"} && ($str[$i] eq "/") && ($str[$i+1] eq "/")) {
               last;
            } elsif ($Cmts{"perl"} && ($str[$i] eq "#")) {
               last;
            } elsif ($Cmts{"C"} && ($str[$i] eq "/") && ($str[$i+1] eq "*")) {
               ++$InComment;
               ++$i;
               $Style = "C";
            } else {
               ++$isNonCmtLine unless ($str[$i] =~ /\s/o);
               if ($str[$i] eq $DQUOTE) {
                  ++$InQuote;
                  $Style = $DQUOTE
               } elsif ($str[$i] eq $SQUOTE) {
                  ++$InQuote;
                  $Style = $SQUOTE
               } elsif ($str[$i] eq $BEGIN_BLOCK) {
                  ++$BlockLevel;
               } elsif ($str[$i] eq $END_BLOCK) {
                  --$BlockLevel;
                  ++$BlockCount;
               } elsif ($str[$i] eq $END_STMT) {
                  ++$lineStmts;
               }
            }
         }
      }

      ## Now add up what we found
      ++$NonCmtLineCount if ($isNonCmtLine || $isPpDirective);
      if ($lineStmts) {
         $StmtCount += $lineStmts;
      } else {
         ++$StmtCount if ($isPpDirective);
      }
   }
   close($filehandle) unless ($filename eq '-');

   ## Lets say that each block represents another (compound) statement
   $StmtCount += $BlockCount;

   ($LineCount, $NonCmtLineCount, $StmtCount);
}

sub dodiff( $$$ ) {
  ($src,$tgt,$which) = @_;
  if ($which == 0) {
      %srcexts = %cextensions;
  }
  else {
      %srcexts = %cycextensions;
  }
  my $running_total = 0;
  my $running_add = 0;
  my $running_sub = 0;
  print REPORT "Diff: $testname $src $tgt\n";
  foreach my $type (@filetypes) {
    if(defined($ENV{$type})) {
      foreach my $i (split(/\s+/,$ENV{$type})) {
        my ($srcext,$tgtext) = ($srcexts{$type},$cycextensions{$type});
        my $name = "$i$srcext.$tgt.diff";
        $name =~ tr/\//_/;
        system("diff -w $src/$i$srcext $tgt/$i$tgtext > $name");
	($lc,$nclc,$stc) = clc("$src/$i$srcext");
        ($add,$sub) = (`grep '^>' $name | wc -l`,
		       `grep '^<' $name | wc -l`);
        chomp ($total,$add,$sub);
	$add =~ s/^\s*//;
	$sub =~ s/^\s*//;
	$total =~ s/^\s*//;
	$total = $nclc;
	$running_total += $total;
	$running_add += $add;
	$running_sub += $sub;
        $recname = "$i$srcext";
	$fmt_add = "+$add";
	$fmt_sub = "-$sub";
	write REPORT;
      }
    }
  }
  $fmt_add = "+$running_add";
  $fmt_sub = "-$running_sub";
  $recname = "Total";
  $total = $running_total;
  write REPORT;

  if ($src eq "c" && $tgt eq "cyc") {
      $cloc = $total;
      if ($running_add > $running_sub) {
	  $cdiff = $running_add;
      }
      else {
	  $cdiff = $running_sub;
      }
      $cpercent = int (($cdiff / $cloc) * 100);
      $cpercent = "($cpercent\\\%)";
  } else {
      if ($src eq "cyc" && $tgt eq "cyc-unique") {
	  $cycloc = $total;
	  if ($running_add > $running_sub) {
	      $cycdiff = $running_add;
	  }
	  else {
	      $cycdiff = $running_sub;
	  }
	  $cycpercent = int (($cycdiff / $cycloc) * 100);
	  $cycpercent = "($cycpercent\\\%)";
      }
  }
}

foreach my $j (<cyclone*>) {
    dodiff("gcc",$j,0);
}

print TEX "$bm & $cloc & \$\pm\$ $cdiff $cpercent & \$\pm\$ $cycdiff $cycpercent \\\\ \n";

close(REPORT);
close(TEX);
