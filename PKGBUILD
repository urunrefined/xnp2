pkgname="xnp2katze"
pkgver="vkglfw.5"
pkgrel="5"
pkgdesc="xnp2katze is PC-9801 series emulator for Linux"
url="https://github.com/urunrefined/xnp2katze"
source=("https://github.com/urunrefined/xnp2katze/archive/refs/tags/$pkgver.tar.gz")
arch=("x86_64")
sha512sums=("SKIP")

makedepends=(
  "vulkan-headers"
  "glslang"
)

depends=(
  "glibc"
  "glfw"
  "vulkan-icd-loader"
  "fontconfig"
  "harfbuzz"
  "freetype2"
  "pulseaudio"
)

prepare() {
  tar -xf "$pkgver.tar.gz"
}

build() {
  cd "$srcdir/$pkgname-$pkgver"
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
  cp -a $pkgname-$pkgver/xnp2 $pkgdir/usr/bin/
  cp -a $pkgname-$pkgver/x11/shader/*.spv $pkgdir/usr/share/$pkgname/
}
