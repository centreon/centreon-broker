#!/usr/bin/perl

use strict;
use warnings;
use Cwd qw(getcwd);

sub get_engine_commit_id() {
  my $path = getcwd();
  chdir "centreon-engine";
  my @output = `git branch`;
  grep /^\* master-ta$/, @output or die "Your engine branch is not master-ta";

  my $id = substr(`git rev-parse HEAD`, 0, 8);
  chdir $path;
  return $id;
}

sub get_broker_commit_id() {
  my $path = getcwd();
  chdir "centreon-broker";
  my @output = `git branch`;
  grep /^\* master-ta$/, @output or die "Your broker branch is not master-ta";

  my $id = substr(`git rev-parse HEAD`, 0, 8);
  chdir $path;
  return $id;
}

sub get_srvi_files {
  my ($id, $package) = @_;
  print ($id);
  # We must get the good path on the repo
  my @package = `curl http://srvi-repo.int.centreon.com/yum/internal/20.04/el7/x86_64/$package/`;

  # We get only the last result
  my $result = (grep /$id/, @package)[-1];

  # we get the path
  $result =~ s|^.*href="(.*)/">centreon.*$|http://srvi-repo.int.centreon.com/yum/internal/20.04/el7/x86_64/$package/$1/|g;

  chomp $result;
  @package = grep /\.rpm/, (`curl $result`);
  my @b = map {
    if (/href="(.*.rpm)"/) {
      "$result/$1"
    } } @package;
  return @b;
}

sub modify_dockerfile {
  my @broker_files = @_;

  my $path = getcwd();
  chdir "centreon-build";

  my $branch = grep /^\* master-ta$/, (`git branch`);
  if ($branch ne 1) {
    die "Centreon-build is not on master-ta branch.\n";
  }

  # We make the script idempotent
  my @file = grep !/curl .* --output/, (`cat containers/web/20.04/fresh.Dockerfile.in`);
  @file = grep !/rpm -Uvh .* .tmp/, @file;

  # We complete the script with our needs
  my @new_file;
  for my $line(@file) {
    if ($line =~ /chown apache:apache/) {
      for my $f(@broker_files) {
        my $simple = $f;
        $simple =~ s|.*/([^/]*.rpm).*$|/tmp/$1|;
        push @new_file, "    curl $f --output $simple && \\\n";
      }
      push @new_file, "    rpm -Uvh --force /tmp/*.rpm && \\\n";
    }
    push @new_file, $line;
  }

  open (my $f, '>', "containers/web/20.04/fresh.Dockerfile.in");
  for (@new_file) {
    print $f $_;
  }
  close $f;
  chdir $path;
}

sub commit_build {
  my ($broker_id, $engine_id) = @_;
  my $path = getcwd();
  chdir "centreon-build";

  my @modif = grep /modifié/, (`git status`);

  print (@modif);
  @modif = map { s/^\s*modifi.*:\s*([^ ]*)$/$1/; $_; } @modif;

  if ($#modif >= 0) {
    my $j = join ' ', @modif;
    `git add $j`;
    `git commit -m 'chore(fresh.Dockerfile.in): update for broker $broker_id and engine $engine_id'`;

    if ( -x "/usr/bin/tig" ) {
      `tig`;
    }
    else {
      `git log`;
    }

    my $ans;
    do {
      print("Do you want me to push the changes? (y/n)\n");
      $ans = <STDIN>;
      chomp $ans;
    }
    while ($ans ne "y" and $ans ne "n");

    if ($ans eq "y") {
      `git push`;
    }
  }
  else {
    print ("No changes. Nothing to commit...\n");
  }
  chdir $path;
}

####### START OF THE PROGRAM #########

my $path = getcwd();
if ($path =~ m|centreon-broker/script$|) {
  print ("Please, move this script outside of\ncentreon-broker. It must be in the common directory of:\n * centreon-broker\n * centreon-engine\n * centreon-build\n\n");
  exit 1;
}
exit 0;
my $broker_id = get_broker_commit_id();
my $engine_id = get_engine_commit_id();
my @broker_files = get_srvi_files($broker_id, "broker");
my @engine_files = get_srvi_files($engine_id, "engine");

my @files = (@broker_files, @engine_files);
modify_dockerfile(@files);
commit_build($broker_id, $engine_id);

show_diffs();

