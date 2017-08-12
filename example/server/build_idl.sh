#!/bin/bash
for f in $(ls *.idl); do
	#idlc -lcpp $f | sed 's/  //g'
	idlcOut="$(./idlc -lcpp ${f} 2>&1)"
	echo $idlcOut | sed 's/in-file/\ninfile/g' | sed 's/outfile/\noutfile/g' | sed 's/complete/\n...complete/g' | sed 's/error at/\n!! error at/g' | egrep --color 'error |$'
done
