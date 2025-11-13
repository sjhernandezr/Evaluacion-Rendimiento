#!/usr/bin/perl
#**************************************************************
#         		Pontificia Universidad Javeriana
#     Autor: Andrés Loreto
#     Fecha: 4 de noviembre de 2025
#     Materia: Sistemas Operativos
#     Tema: Taller de Evaluación de Rendimiento
#     Fichero: script automatización ejecución por lotes 
#****************************************************************/

#!/usr/bin/perl
use strict;
use warnings;

# Carpeta actual
my $Path = `pwd`; chomp($Path);

# Ejecutables a probar 
my @executables = ("mmClasicaOpenMP","mmClasicaFork","mmFilasOpenMP");

# Tamaños y hilos 
my @sizes   = (64, 128, 256, 512);
my @threads = (1, 2, 4);

# Repeticiones por punto
my $runs = 40;

# Carpeta de salida
my $outdir = "$Path/data";
mkdir $outdir unless -d $outdir;

# Utilidades
sub parse_ms {
    my ($out) = @_;
    # Formato recomendado: time_ms=123.45
    if ($out =~ /time_ms\s*=\s*([0-9]+(?:\.[0-9]+)?)/) {
        return $1 + 0.0;
    }
    # Número “pelado” (algunas versiones antiguas imprimen microsegundos)
    if ($out =~ /(^|\s)([0-9]{2,})\s*$/m) {
        my $v = $2 + 0.0;
        # Heurística: si es muy grande, interpretamos microsegundos -> ms
        return ($v >= 1000) ? ($v/1000.0) : $v;
    }
    return undef;
}

sub mean_std {
    my (@x) = @_;
    my $n = scalar @x; return (0,0) if $n == 0;
    my $sum = 0; $sum += $_ for @x;
    my $mu = $sum / $n;
    my $ss = 0; $ss += ($_ - $mu)**2 for @x;
    my $std = ($n > 1) ? sqrt($ss / ($n-1)) : 0;
    return ($mu, $std);
}

foreach my $exe (@executables) {
  foreach my $n (@sizes) {
    foreach my $t (@threads) {

      # Si el ejecutable no usa hilos (ej. Fork), el parámetro $t no afecta.
      my $outfile = "$outdir/${exe}_S${n}_T${t}.csv";
      open(my $fh, '>', $outfile) or die "No puedo escribir $outfile: $!";
      print $fh "size,threads,run,time_ms\n";

      my @times = ();

      for (my $i = 1; $i <= $runs; $i++) {

        # Para OpenMP, fija OMP_NUM_THREADS
        my $prefix = "";
        if ($exe =~ /OpenMP/i) { $prefix = "OMP_NUM_THREADS=$t "; }

        my $cmd = "$prefix$Path/$exe $n $t";
        my $out = `$cmd`;
        my $ms  = parse_ms($out);

        unless (defined $ms) {
            warn "No pude parsear tiempo en salida de '$cmd': $out\n";
            next;
        }

        push @times, $ms;
        print $fh "$n,$t,$i,$ms\n";
      }

      my ($avg, $std) = mean_std(@times);
      print $fh "# avg_ms=$avg\n";
      print $fh "# std_ms=$std\n";
      close($fh);

      printf "OK %-16s size=%4d threads=%2d  avg_ms=%8.3f  std_ms=%8.3f  -> %s\n",
             $exe, $n, $t, $avg, $std, $outfile;
    }
  }
}
