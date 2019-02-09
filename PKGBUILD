# Author: Oliver Reiter

pkgname=xparser
pkgver=0.17.1
pkgrel=1
pkgdesc="FLAME XMML parser"
arch=('x86_64')
license=('GPL')
url='http://www.flame.ac.uk'
depends=('gsl')
makedepends=('git')
source=('git+file:///home/reitero/Arbeit/FLAME/xparser')
# source=('https://github.com/FLAME-HPC/xparser/archive/master.zip')
md5sums=('SKIP')

# prepare() {
  # cd "$srcdir/xparser-master"
# }

build() {
  cd "$srcdir"
  # cd "$srcdir/xparser-master"
  make
}

package() {
  echo "Packaging $pkgname ..."
  install -Dm755 "$srcdir/xparser-master/xparser" "$pkgdir/usr/bin/xparser"

  install -dm755 $pkgdir/usr/include/xparser
  install -Dm644 $srcdir/xparser-master/*.tmpl "$pkgdir/usr/include/xparser"
  echo "  -> Please set 'export FLAME_XPARSER_DIR=/usr/include/xparser' after installing!"
}