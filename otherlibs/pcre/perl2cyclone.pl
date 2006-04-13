#!/usr/bin/perl

#
# This is an incomplete tool that helps to port Perl programs to
# Cyclone.
#
# It relies on PPI, a Perl parser written in Perl.  Doing
#     sudo cpan PPIx::XPath
# should be enough to get all the required dependencies.
#
# For now the tool only helps automate the porting of regular
# expressions, in particular, the s///, m//, and qr// features of
# Perl.
#
# Porting these by hand is error-prone because Perl and Cyclone string
# quoting conventions are a bit different, and it is easy to get the
# options (like s///egx) wrong.
#
# Given a Perl program as input, the tool examines each Perl
# subroutine and spits out a corresponding Cyclone function.  The body
# of the Cyclone function will have a statement for each s/// and m//
# in the Perl subroutine, and a #define for each qr//.  Other parts of
# the Perl subroutine will be ignored---the translated parts will not
# be in context.  Furthermore, all statements corresponding to s///
# will be together, in order, and similarly for m// and qr//.  If
# there are s/// and m// statements interleaved in the Perl input,
# they will not be interleaved in the Cyclone output.
#

use PPIx::XPath;

my $subpat1 = qr/^s\/([^\/]*)\/([^\/]*)\/([a-z]*)/;
my $subpat2 = qr/^s!([^\/]*)!([^\/]*)!([a-z]*)/;
my $subpat3 = qr/^s[\n\t\r ]*\{([^\}]*)\}[\n\t\r ]*\{([^\}]*)\}[\n\t\r ]*([a-z]*)/;

my $matchpat1 = qr/^m\/([^\/]*)\/([a-z]*)/;
my $matchpat2 = qr/^m!([^!]*)!([a-z]*)/;

my $regexppat1 = qr/^qr\/([^\/]*)\/([a-z]*)/;
my $regexppat2 = qr/^qr!([^!]*)!([a-z]*)/;

sub optionE {
    return m/e/;
}

sub optionG {
    return m/g/;
}

sub pcre_options {
    local $_ = shift;
    my $ret = "0";
    if (/i/) { $ret .= "|PCRE_CASELESS"; }
    if (/m/) { $ret .= "|PCRE_MULTILINE"; }
    if (/s/) { $ret .= "|PCRE_DOTALL"; }
    if (/x/) { $ret .= "|PCRE_EXTENDED"; }
    $ret =~ s/0\|//;
    return $ret;
}

sub pcre_function {
    local $_ = shift;
    if (optionE) {
        if (optionG) {
            return "PCRE_S_EG";
        }
        else {
            return "PCRE_S_E";
        }
    }
    elsif (optionG) {
        return "pcre_s_g";
    }
    else {
        return "pcre_s";
    }
}

sub pat2c {
    my $s = shift;
    $s =~ s/\\/\\\\/g;
    $s =~ s/"/\\"/g;
    $s =~ s/\n/\\n/g;
    return "\"". $s . "\""
}

sub subst2c {
    my $s = shift;

    if (($pat,$repl,$options) = ($s =~ /$subpat1/)) {
        print "t = ", pcre_function($options), "(t,\n";
        print "\t", pat2c($pat), ",\n";
        if (optionE($options)) {
            print "\t({ ", $repl, " }),\n";
        }
        else {
            $repl =~ s/"/\\"/g;
            print "\t\"", $repl, "\",\n";
        }
        print "\t", pcre_options($options), ");\n";
    }
    elsif (($pat,$repl,$options) = ($s =~ /$subpat2/)) {
        print "t = ", pcre_function($options), "(t,\n";
        print "\t", pat2c($pat), ",\n";
        if (optionE($options)) {
            print "\t({ ", $repl, " }),\n";
        }
        else {
            $repl =~ s/"/\\"/g;
            print "\t\"", $repl, "\",\n";
        }
        print "\t", pcre_options($options), ");\n";
    }
    elsif (($pat,$repl,$options) = ($s =~ /$subpat3/)) {
        print "t = ", pcre_function($options), "(t,\n";
        print "\t", pat2c($pat), ",\n";
        if (optionE($options)) {
            print "\t({ ", $repl, " }),\n";
        }
        else {
            $repl =~ s/"/\\"/g;
            print "\t\"", $repl, "\",\n";
        }
        print "\t", pcre_options($options), ");\n";
    }
    else {
        print $s, "\n";
    }
}

sub match2c {
    my $s = shift;

    if (($pat,$options) = ($s =~ /$matchpat1/)) {
        print "pcre_m(t,\n";
        print "\t", pat2c($pat), ",\n";
        print "\t", pcre_options($options), ");\n";
    }
    elsif (($pat,$options) = ($s =~ /$matchpat2/)) {
        print "pcre_m(t,\n";
        print "\t", pat2c($pat), ",\n";
        print "\t", pcre_options($options), ");\n";
    }
    else {
        $s =~ s!^!// !gm;
        print $s, "\n";
    }
}

sub regexp2c {
    my $s = shift;

    if (($pat,$options) = ($s =~ /$regexppat1/)) {
        print "#define RRR ";
        print pat2c($pat);
        print "// ", pcre_options($options), "\n";
    }
    elsif (($pat,$options) = ($s =~ /$regexppat2/)) {
        print "#define RRR ";
        print pat2c($pat);
        print "// ", pcre_options($options), "\n";
    }
    else {
        $s =~ s!^!// !gm;
        print $s, "\n";
    }
}

my $x = PPIx::XPath->new("/home/trevor/blosxom/plugins/SmartyPants");
my @subs = $x->match("//Statement::Sub");
foreach $y (@subs) {

    # Print original Perl as a comment
    my $z = $y;
    $z =~ s!^!//!gm;
    print $z, "\n";

#    my $function_name = $y =~ /^sub[\n\t\r ]+([_a-zA-Z0-9]+)[\n\t\r ]/ ? $1 : "UNKNOWN";
#    print "char ?", $function_name, "(char ?t) {\n";

    if (($function_name) = ($y =~ /^sub[\n\t\r ]+([_a-zA-Z0-9]+)[\n\t\r ]/)) {
        print "char ?", $function_name, "(char ?t) {\n";
    }
    else {
        print "char ?UNKNOWN(char ?t) {\n";
    }

    my $q = PPIx::XPath->new($y);

    my @regexps = $q->match("//Token::QuoteLike::Regexp");
    foreach $v (@regexps) {
        regexp2c($v);
    }

    my @matches = $q->match("//Token::Regexp::Match");
    foreach $v (@matches) {
        match2c($v);
    }

    my @substs = $q->match("//Token::Regexp::Substitute");
    foreach $v (@substs) {
        subst2c($v);
    }

    print "return t;\n}\n";
}
