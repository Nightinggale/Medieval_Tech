#!/bin/sh


function getYield
{

IFS=$'\n'
for LINE in `grep $1 * | grep --invert-match "//case"`
do
	if [ "${LINE:0:6}" == "Yields" ]
	then
		continue
	fi 
	if [ "${LINE:0:21}" == "CyEnumsInterface.cpp:" ]
	then
		continue
	fi
	#temp_line=
	if [ "`echo "${LINE%%$1*}" | grep "//"`" != "" ]
	then
		continue
	fi
	if [ "${LINE:0:8}" == "yield.sh" ]
	then
		continue
	fi
	
	echo $LINE
done

}

virtual_clear="YIELD_COTTON YIELD_ALE YIELD_WINE YIELD_LEATHER_ARMOR YIELD_SCALE_ARMOR YIELD_MAIL_ARMOR YIELD_PLATE_ARMOR"

all="YIELD_FOOD YIELD_GRAIN YIELD_CATTLE YIELD_SHEEP YIELD_WOOL YIELD_LUMBER YIELD_STONE YIELD_SILVER YIELD_SALT YIELD_SPICES YIELD_FUR YIELD_COTTON YIELD_BARLEY YIELD_GRAPES YIELD_ORE YIELD_CLOTH YIELD_COATS YIELD_ALE YIELD_WINE YIELD_TOOLS YIELD_WEAPONS YIELD_HORSES YIELD_LEATHER_ARMOR YIELD_SCALE_ARMOR YIELD_MAIL_ARMOR YIELD_PLATE_ARMOR YIELD_TRADE_GOODS"

missing="YIELD_FOOD YIELD_GRAIN YIELD_CATTLE YIELD_SHEEP YIELD_WOOL YIELD_LUMBER YIELD_STONE YIELD_SPICES YIELD_COTTON YIELD_ORE YIELD_ALE YIELD_WINE YIELD_TOOLS YIELD_WEAPONS YIELD_HORSES YIELD_LEATHER_ARMOR YIELD_SCALE_ARMOR YIELD_MAIL_ARMOR YIELD_PLATE_ARMOR YIELD_TRADE_GOODS"

clear="YIELD_SILVER YIELD_FUR YIELD_BARLEY YIELD_GRAPES YIELD_CLOTH YIELD_COATS YIELD_SALT"

clear="${clear} ${virtual_clear}"

function getClear
{
for YIELD in $missing
do
	if [ "`getYield $YIELD`" == "" ]
	then
		for COMPLETED in $clear
		do
			if [ "$COMPLETED" == "$YIELD" ]
			then
				continue 2
			fi
		done
		echo $YIELD
	fi
done
}

function displayMissing
{
	for YIELD in $missing
	do
		for COMPLETED in $clear
		do
			if [ "$COMPLETED" == "$YIELD" ]
			then
				continue 2
			fi
		done
		echo $YIELD
	done
}

function buildMissing
{
	for YIELD in $all
	do
		for COMPLETED in $clear
		do
			if [ "$COMPLETED" == "$YIELD" ]
			then
				continue 2
			fi
		done
		printf "$YIELD "
	done
	echo ""
}

function complete
{
	for YIELD in $clear
	do
		echo "$YIELD "
	done
}


if [ "$1" == "" ]
then
	getClear
elif [ "$1" == "missing" ]
then
	displayMissing
elif [ "$1" == "build" ]
then
	buildMissing
elif [ "$1" == "complete" ]
then
	complete
else
	getYield $1
fi