# Maintainer: vinegm <info AT vinegm D0T com>
pkgname=cless
pkgver=1.0.0
pkgrel=1
pkgdesc="A chess TUI to play against friends or engines"
arch=('x86_64')
url="https://github.com/vinegm/cless"
license=('MIT')
depends=('ncurses')
makedepends=('cmake' 'make' 'gcc')
source=("https://github.com/vinegm/cless/archive/v$pkgver.tar.gz")
sha256sums=('c52e3a524b2ad5d1c0cdb00741a870fec5caaefaf38c7e49a2cdd603d2853888')

build() {
    cd "$pkgname-$pkgver"

    mkdir -p build
    cd build

    cmake -DCMAKE_BUILD_TYPE=Release \
          -DCMAKE_INSTALL_PREFIX=/usr \
          ..

    make
}

package() {
    cd "$pkgname-$pkgver"
    install -Dm755 "build/cless" "$pkgdir/usr/bin/cless"
    install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
}
