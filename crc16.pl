use Digest::CRC qw(crcccitt);
use Data::Dumper;


my $data = join("", map {chr $_} (1..16));

printf "%04X\n",crcccitt("$data");

