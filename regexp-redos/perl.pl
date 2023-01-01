use strict;
use warnings;
use utf8;

# Perl パフォーマンスが多少悪いぐらい。

sub test1 {
    my $re = qr/^(a+)+$/;

    if (('a' x 1000) =~ $re) {
        print "1 matched: ", $&, "\n";
    }
    if ( (('a' x 10000) . 'x') =~ $re) {   # user  0m0.676s
        print "2 matched\n";
    } else {
        print "not matched.\n";
    }
}

# Perl は、典型的なパタンについては個別対応している、らしい?
sub test2 {
    if ( ("a" x 65535 . '') =~ qr/a*z/) {  # 一瞬で完了
        print "matched.\n";
    }
    if ( ("a" x 65535 . '') =~ qr/a*[yz]/) { # 0m15.133s
        print "matched.\n";
    }
}

#&test1;
&test2;
