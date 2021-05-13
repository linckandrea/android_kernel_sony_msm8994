mkdir -p out
export ARCH=arm64
export SUBARCH=arm64
make O=out clean
make O=out mrproper
export CROSS_COMPILE=/home/andrea/android/aarch64-linux-android-4.9/bin/aarch64-linux-android-
make O=out kitakami_sumire_defconfig
make O=out -j$(nproc --all)

echo
echo "Making anykernel zip"
echo
cp ./out/arch/arm64/boot/Image.gz-dtb ./AnyKernel3/
cd ./AnyKernel3
zip -r9 Pop_kernel-sumire-R-rx-x.zip * -x .git README.md *placeholder
