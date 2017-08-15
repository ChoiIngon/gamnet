#!/bin/bash
for f in $(ls *.idl); do
	#idlc -lcpp $f | sed 's/  //g'
	idlc_cpp_out="$(./idlc -lcpp ${f} 2>&1)"
	echo $idlc_cpp_out | sed 's/in-file/\ninfile/g' | sed 's/outfile/\noutfile/g' | sed 's/complete/\n...complete/g' | sed 's/error at/\n!! error at/g' | egrep --color 'error |$'
	idlc_cshap_out="$(./idlc -lcs ${f} 2>&1)"
	echo $idlc_cshap_out | sed 's/in-file/\ninfile/g' | sed 's/outfile/\noutfile/g' | sed 's/complete/\n...complete/g' | sed 's/error at/\n!! error at/g' | egrep --color 'error |$'
done
for f in $(ls *.h); do
	cp ${f} ../server
done
for f in $(ls *.cs); do
	cp ${f} ../unity_client/Assets
done
