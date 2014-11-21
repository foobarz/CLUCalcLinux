CLUCalcLinux, based on CLUCalc 4.3.3.

CLUCalc is a nice tool for computing and visualizing Geometric (Clifford) Algebra. The official web is www.clucalc.info. However, official support for CLUCalc on Linux seems to have ended at the release of CLUCalc 4.3.3. This repository for "CLUCalcLinux" is unofficial and is not affiliated with the original/official developer of CLUCalc.

The sources under CLUCalcSource-4.3.3 have been minimally edited from the original sources, just to get the compile to work. The changes mostly involved using "GPL Ghostscript" and libpng12.

The directory "CLUCalc-BIN" contains a fully compiled installation that may work (no warranty). Just copy it somewhere and optionally symlink the CLUCalc binary into a bin directory.

If you want to try compiling, you may need to set LDFLAGS (default is /usr/lib):
 cd CLUCalcSource-4.3.3
 LDFLAGS="-L/usr/lib64" ./INSTALL

The license is GPL. Pull requests are welcome. Enjoy!
