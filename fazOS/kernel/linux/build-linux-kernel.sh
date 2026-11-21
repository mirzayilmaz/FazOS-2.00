#!/bin/bash
# FazOS Linux cekirdegini kernel.org kaynagindan derler ve .deb paketleri uretir.
# Debian/Ubuntu uzerinde root ile calistirin. Internet gerekir. Sure: 30-90 dk.
# Daha genis donanim destegi icin: BASECONFIG=/boot/config-... ./build-linux-kernel.sh
set -e
KVER="${KVER:-6.6.63}"; MAJOR="${KVER%%.*}"
cd "$(dirname "$0")"
apt-get update && apt-get install -y build-essential bc bison flex libssl-dev libelf-dev libncurses-dev dwarves rsync kmod cpio xz-utils fakeroot curl dpkg-dev debhelper
[ -f "linux-$KVER.tar.xz" ] || curl -fLO "https://cdn.kernel.org/pub/linux/kernel/v$MAJOR.x/linux-$KVER.tar.xz"
rm -rf "linux-$KVER"; tar xf "linux-$KVER.tar.xz"; cd "linux-$KVER"
if [ -n "$BASECONFIG" ]; then cp "$BASECONFIG" .config; else make x86_64_defconfig; fi
scripts/kconfig/merge_config.sh -m .config ../fazos.fragment
scripts/config --disable SYSTEM_TRUSTED_KEYS --disable SYSTEM_REVOCATION_KEYS --disable DEBUG_INFO_BTF
make olddefconfig
make -j"$(nproc)" bindeb-pkg KBUILD_BUILD_USER=feas KBUILD_BUILD_HOST=fazos
mkdir -p ../../../config/packages.chroot
cp ../linux-image-*-fazos*.deb ../../../config/packages.chroot/ 2>/dev/null || cp ../linux-image-*.deb ../../../config/packages.chroot/
echo "Tamam: .deb dosyalari config/packages.chroot icine kopyalandi."
