if [[ "$(dirname $0)" == "" ]]
then
    ocn="ocn-dot"
else
    ocn="$(dirname $0)"/ocn-dot
fi

tempdir=`mktemp -d`

$ocn $@ > $tempdir/network.dot

neato -Teps $tempdir/network.dot > $tempdir/network.eps

ps2pdf $tempdir/network.eps $tempdir/network.pdf >& /dev/null
pdfcrop $tempdir/network.pdf $tempdir/network-crop.pdf

cat $tempdir/network-crop.pdf
rm -rf $tempdir
