#!/bin/sh

IFS=$'\n'

enum=""

for LINE in `cat CvEnums.h`
do
	if [ "${LINE:0:4}" == "enum" ]
	then
		enum=${LINE:15}
	elif [ "${enum}" != "" ]
	then
		if [ "${LINE:0:2}" == "};" ]
		then
			enum=""
		else
			end_line="${LINE##* }"
			if [ "${LINE:0:1}" != "#" ] && [ "${end_line:0:2}" != "-1" ]
			then
				LINE=${LINE:1}
				LINE=${LINE%%,*}
				if [ "${LINE}" != "" ] && [ "${LINE:0:1}" != "/" ]
				then
					LINE=${LINE%% *}
					found_line="`grep -r "<Type>${LINE}</Type>" "${HOME}/test/XML"`"
					#if [ "`grep -r "<Type>${LINE}" "${HOME}/test/XML"`" != "" ]
					if [ "${#found_line}" -gt "${#LINE}" ]
					then	
						echo $enum
						enum=""
						#grep -r "<Type>${LINE}</Type>" "${HOME}/test/XML"
						#echo "x${LINE}x${found_line}x"
					fi
				fi
			fi
		fi
	fi
done