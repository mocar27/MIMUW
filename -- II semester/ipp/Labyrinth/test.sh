#!/bin/bash

counter=0
max=0

for tempTest in $2/*.in
do
	tempCounter=0;

	valgrind --error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all -q ./$1 < $tempTest 1> prog.out 2> prog.err
	valueOfValgrind=$?

	echo -n -e "$((max + 1)).	\e[1mANSWER: \e[0m"

	if  diff ${tempTest%in}out prog.out  >/dev/null 
	then
		echo -e -n "\e[32;21mOK \e[0m"
		((tempCounter++))	
	else 
		echo -e -n "\e[31;21mWA \e[0m"
	fi

	echo -e -n "	\e[1mERROR: \e[0m"

	if  diff ${tempTest%in}err prog.err  >/dev/null 
	then
	echo -e -n "\e[32;21mOK \e[0m"
		((tempCounter++))
	else 
		echo -e -n "\e[31;21mWA \e[0m"
	fi

	echo -e -n "	\e[1mMEMORY: \e[0m"

	if ((valueOfValgrind == 123))
	then
	echo -e -n "\e[31;21mPROBLEM \e[0m"
		
	else 
		echo -e -n "\e[32;21mOK      \e[0m"
		((tempCounter++))
	fi

	if (($tempCounter == 3))
	then
		((counter++))
	fi

		((max++))

	echo "	for $tempTest"
done
echo -e 
echo "SUMMARY OF COMPILATION: $counter/$max"
echo -e

rm prog.out
rm prog.err
