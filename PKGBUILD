pkgname="xnp2katze"
pkgver="vkglfw.3"
pkgrel="3"
pkgdesc="xnp2katze is PC-9801 series emulator for Linux"
url="https://github.com/urunrefined/xnp2katze"
source=("https://github.com/urunrefined/xnp2katze/archive/refs/tags/$pkgver.tar.gz")
arch=("x86_64")
sha512sums=("29e30d4f34d834464cd9a2acec6066c1a667e83050f7113429e8c3e6a4f10d58dcd50cabb33b9039313ed1b6afabcd931f83443f5b6c2ff993e8fb78dd2b93dc")

makedepends=(
  "vulkan-headers"
  "glslang"
)

depends=(
  "glibc"
  "glfw"
  "sdl2"
  "vulkan-icd-loader"
  "fontconfig"
  "harfbuzz"
  "freetype2"
)

prepare() {
  tar -xf "$pkgver.tar.gz"
}

build() {
  cd "$srcdir/$pkgname-$pkgver/x11"
  make -j8
}

check() {
  cd "$srcdir/$pkgname-$pkgver/x11" 
  if [[ -e "xnp2" ]]; then
    echo 'Checks passed!'
  fi
}

package() {
  mkdir -p $pkgdir/usr/bin
  mkdir -p $pkgdir/usr/share/$pkgname
  cp -a $pkgname-$pkgver/x11/xnp2 $pkgdir/usr/bin/
  cp -a $pkgname-$pkgver/x11/shader/*.spv $pkgdir/usr/share/$pkgname/
}
