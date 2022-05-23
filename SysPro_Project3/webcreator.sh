#!/bin/bash


#http://tldp.org/LDP/abs/html/randomvar.html
get_random() {
	FLOOR=$1
	RANGE=$2
	i=0   #initialize
	while [ "$i" -le $FLOOR ]
	do
	  i=$RANDOM
	  let "i %= $RANGE"  # Scales $number down within $RANGE.
	done
	echo "$i"
}

if [ $# != "4" ]; then
	echo "ERROR! Wrong number of arguments"
	exit 1
fi

root="$1"
text="$2"

num_lines=$(wc -l < $text)
if [ $num_lines -lt 10000 ]; then
	echo "Too small text file"
	exit 1
fi

w="$3"
#check parameter w
if ! [[ "$w" =~ ^[0-9]+$ ]]; then
	echo "ERROR! Parameter w is not integer"
	exit 1
fi

p="$4"
#check parameter p
if ! [[ "$p" =~ ^[0-9]+$ ]]; then
	echo "ERROR! Parameter p is not integer"
	exit 1
fi

#check if w or p is 0
if [[ $w -eq 0 || $p -eq 0 ]]; then
	echo "Error! Please give # > 0"
	exit 1
fi

if [ -z "$(ls -A $root)" ]; then
	echo "# Directory is empty. Start:"
else
	echo "# Warning: directory is full, purging ..."
	rm -rf $root
fi

max_k=$((num_lines - 2000))

#reading text_file
declare -a textIndex

index=0
while read line; do
  	textIndex[index]="$line"
	((++index))
done < $text

ArrayNumPages=()

for i in `seq 0 $((w-1))`; do
	for j in `seq 0 $((p-1))`; do
		ArrayNumPages+=($(printf "site%s/page%s_%s.html" "$i" "$i" "$RANDOM"))
	done
done

CheckIncomingPages=()

f=$(((p/2)+1))
q=$(((w/2)+1))

flag_1=0

numLinks=$((f+q))

#w=1
if [ $w -eq 1 ]; then
	numLinks=$f
fi
#p=1
if [ $p -eq 1  ]; then
	if [ $w -eq 1 ]; then
		flag_1=1
		numLinks=1
	else
		numLinks=$q
	fi
fi

#w!=1 p=2
if [[ $w -ne 1 && $p -eq 2 ]]; then
	((numLinks--))
fi
#w=1 p=2
if [[ $w -eq 1 && $p -eq 2 ]]; then
	numLinks=1
fi
#w=2 p=1
if [[ $w -eq 2 && $p -eq 1 ]]; then
	numLinks=1
fi

for i in `seq 0 $((w-1))`; do

	rootName=$(printf "%s" "$root" )
	echo "# Creating web site" $i "..."
	
	siteName=$(printf "site%s" "$i")

	mkdir -p $rootName/$siteName

	offset=$((p*i))

	if [ $i == '0' ]; then
		q_Array=(${ArrayNumPages[@]:$((p))})
	elif [ $i == $((w-1)) ]; then
		q_Array=(${ArrayNumPages[@]:0:$((offset))})
	else
		q_Array=(${ArrayNumPages[@]:0:$((offset))})
		q_Array+=(${ArrayNumPages[@]:$((offset+p))})
	fi

	f_Array=()
	temp=$offset
	for j in `seq 0 $((p-1))`; do
		f_Array+=(${ArrayNumPages[$temp]})
		temp=$((temp+1))
	done

	for j in `seq 0 $((p-1))`; do
		k=$(get_random 1 $max_k)
		m=$(get_random 1000 2000)

		pageName=${ArrayNumPages[$offset]}

		if [ $flag_1 -eq 0 ]; then
			#remove page from f array
			cur_f_Array=( ${f_Array[@]/$pageName*/} )
		else
			#special case - 1 website/1 page
			cur_f_Array=( ${f_Array[@]} )
		fi

		linksForPage=(${cur_f_Array[@]} ${q_Array[@]})

		echo "#   Creating page" ${rootName}/$pageName "with" $m  "lines starting at line" $k "..."  

		##########################
		#write first headers
		###########################
		echo "<!DOCTYPE html>">>$rootName/$pageName
		echo "<html>">>$rootName/$pageName
		echo "   <body>">>$rootName/$pageName
		#######################
		#write words and links
		#######################
		curLinks=0
		numLines=$((m/numLinks))
		numLines=$((numLines+m%numLinks))
		countLines=0
		index=$k
		#echo "NUM LINES" $numLines

		while ((countLines<m)); do
			check=$((m - countLines))
			if [ "$check" -lt "$numLines" ]; then
				numLines=$check
			fi

			#############
			#write words
			#############
			count=0;
			while (( count<numLines )); do
				echo "   ${textIndex[$((count+index))]} <br>">>$rootName/$pageName
				((count++))
			done
			index=$((index+count))
			countLines=$((countLines+numLines))
			#echo $numLinks $curLinks

			size=${#linksForPage[@]}
		
			#if [ $curLinks -ne $numLinks ]; then
			off=$(($RANDOM % $size))
			link=${linksForPage[$off]}

			if [[ "${checkIncomingPages[*]}" != *"$link"* ]]; then
				checkIncomingPages+=(${link})
			fi

			#####################
			#write incoming link
			#####################
			printf "   <a href=../$link>go_to_link</a>">>$rootName/$pageName

			echo "#   Adding link to" ${rootName}/$link

			#######################
			#remove link from list
			#######################
			linksForPage=( ${linksForPage[@]/$link*/} )
			((curLinks++))
		done

		##########################
		#write last headers
		###########################
		printf "\n   </body>\n">>$rootName/$pageName
		echo "</html>">>$rootName/$pageName
		offset=$((offset+1))
	done
done 

size1=${#checkIncomingPages[@]}
size2=${#ArrayNumPages[@]}

if [ $size1 -eq $size2 ]; then
	echo "# All pages have at least one incoming link"
else
	printf "# %s/%s pages have at least one incoming link\n" $size1 $size2
fi 

echo "# Done."

exit 1


