pkgname=cyd-fabric
pkgver=0.0.1
pkgrel=1
pkgdesc="Cyd-Fabric Library"
arch=('any')
license=('GPL3')
options=('!strip')

depends=( )

build() {
  cd ..

  cmake -DCMAKE_BUILD_TYPE=Release -G Ninja -S . -B ./cmake-build-release
  cmake --build ./cmake-build-release --target cyd-fabric
}

package() {
  cd ..

  # Install static library binary
  install -Dm755 cmake-build-release/$pkgname "$pkgdir/usr/bin/$pkgname"
  install -Dm755 start_cwm "$pkgdir/usr/bin/start_cwm"

  # Install xsession file
  install -Dm644 cdwm.desktop "$pkgdir/usr/share/xsessions/cdwm.desktop"
}
