#!/usr/bin/perl

use strict;
use warnings;
use Parse::MediaWikiDump;
use Encode;

my $WIKITEXT_DIR = "/home/bbs/bbshome/wikitext";
my $file = shift(@ARGV) or die "must specify a MediaWiki dump of the current pages";
my $dump = Parse::MediaWikiDump::Pages->new($file);  
my $count = 0;

system("rm -rf $WIKITEXT_DIR");
system("mkdir $WIKITEXT_DIR");

open INDEXFILE, ">$WIKITEXT_DIR/.INDEX" or die "Cannot open index";
while (my $page = $dump->next) {
   if ($page->namespace eq '' || $page->namespace eq "Category") {
      my $title = $page->title;
      my $title_gb = encode("gb2312", decode("unicode", $title));
      my $text = $page->text;
      my $text_gb = encode("gb2312", decode("unicode", $$text));
      if (open(OUTFILE, ">$WIKITEXT_DIR/$title_gb")) {
         print OUTFILE "$title_gb\n\n$text_gb\n";
         close OUTFILE;
         print INDEXFILE "$title_gb\n";
      }
   }
}
close INDEXFILE;

