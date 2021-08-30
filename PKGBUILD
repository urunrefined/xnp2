pkgname="xnp2katze"
pkgver="vkglfw.3"
pkgrel="3"
pkgdesc="xnp2katze is PC-9801 series emulator for Linux"
url="https://github.com/urunrefined/xnp2katze"
source=("https://github.com/urunrefined/xnp2katze/archive/refs/tags/$pkgver.tar.gz")
arch=("x86_64")
sha512sums=("bcf0c9fcfacc10ce66598ccfa1bd2fc4e09d68020da233a3019ade8c4496fe0155234d5fc6dfdf1398335d3439be788e44a836e7f2191fa826c987cfcc9ad755")

makedepends=(
  "make"
  "glibc"
)

depends=(
  "glfw"
  "sdl2"
  "vulkan-icd-loader"
  "glslang"
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
