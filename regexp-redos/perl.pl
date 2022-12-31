use strict;
use warnings;
use utf8;

# Perl パフォーマンスが多少悪いぐらい。

my $re = qr/^(a+)+$/;

if (('a' x 1000) =~ $re) {
    print "1 matched: ", $&, "\n";
}
if ( (('a' x 10000) . 'x') =~ $re) {   # 10000 だと時間が掛かる。
    print "2 matched\n";
} else {
    print "not matched.\n";
}

