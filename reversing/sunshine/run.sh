SOURCE=SunShine-latest.apk
OUTPUT=output
APKTOOL=apktool
DEOBF=./deobf/deobf

clear

rm -rf $OUTPUT
rm -rf $OUTPUT.apk

unzip -q $SOURCE -d unzipped
$DEOBF unzipped/classes.dex
mv unzipped/classes.dex-deobf unzipped/classes.dex
cd unzipped
zip -qr $OUTPUT *
mv $OUTPUT.zip ../$OUTPUT.apk
cd ..
rm -rf unzipped
$APKTOOL -fr d $OUTPUT.apk
rm -rf $OUTPUT.apk
