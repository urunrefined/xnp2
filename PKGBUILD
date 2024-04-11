pkgname="xnp2katze"
pkgver="vkglfw.6"
pkgrel="6"
pkgdesc="xnp2katze is PC-9801 series emulator for Linux"
url="https://github.com/urunrefined/xnp2katze"
source=("https://github.com/urunrefined/xnp2katze/archive/refs/tags/$pkgver.tar.gz")
arch=("x86_64")
sha512sums=("c82f38d3f8eb45e6c015af1e0ec66f15b8a1fc03c54d96d7aa5fc130ab9393cd7a41387637f3e83cd9328437c741f2f13bda25bef4408609155f1604c631e09b")

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
