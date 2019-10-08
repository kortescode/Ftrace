tmp_file='/tmp/fcts_protos.txt'
file_to_read=''
file_to_write='syscalls_tab.c'

############################
## Create fcts proto file ##
############################

function list_fcts_proto
{
    concat=''
    do_concat=0

    while read line
    do
    	if [ ! -z "`echo "$line" | grep '^asmlinkage'`" ]
    	then
    	    do_concat=1
    	fi

    	if [ ! -z "`echo "$line" | grep ';'`" ] && [ $do_concat -eq 1 ]
    	then
    	    echo "`echo "$concat$line" | cut -d '_' -f 2-`" >> $tmp_file
    	    concat=''
    	    do_concat=0
    	fi

    	if [ $do_concat -eq 1 ]
    	then
    	    concat="$concat$line"
    	fi
    done < $file_to_read
}

function init_fcts_proto
{
    echo > $tmp_file

    for file_to_read in $(find /usr/ -name syscall.h -or -name syscalls.h)
    do
        if [ ! -z "$file_to_read" ] && [ -e "$file_to_read" ] && [ -f "$file_to_read" ] && [ -r "$file_to_read" ]
        then
           list_fcts_proto
        fi
    done
}

##########################
## Create syscalls file ##
##########################

function write_syscalls
{
    while read line
    do
    	if [ ! -z "`echo "$line" | grep '^#define __NR_'`" ]
    	then
    	    opcode=`echo $line | cut -d ' ' -f 2- | cut -d ' ' -f 1`
    	    fct=`echo $opcode | sed 's/__NR_//g'`
    	    while read line
    	    do
        		if [ ! -z "`echo "$line" | grep "^$fct("`" ]
        		then
        		    break 1
    		    fi
    	    done < $tmp_file
    	    echo "    {$opcode, "'"'$fct'"'"}," >> $file_to_write
    	fi
    done < $file_to_read
}

function write_syscalls_tab
{
    init_fcts_proto
    
    echo "#include	<asm/unistd_64.h>" > $file_to_write
    echo "#include	<stdlib.h>" >> $file_to_write
    echo '#include	"ftrace.h"' >> $file_to_write
    echo >> $file_to_write
    echo "t_tab         syscalls_tab[] =" >> $file_to_write
    echo "  {" >> $file_to_write
    
    for file_to_read in $(find /usr/ -name unistd_64.h)
    do
        if [ ! -z "$file_to_read" ] && [ -e "$file_to_read" ] && [ -f "$file_to_read" ] && [ -r "$file_to_read" ]
        then
           write_syscalls
        fi
    done
    
    echo "    {END, NULL}" >> $file_to_write
    echo "  };" >> $file_to_write
    
    rm -f $tmp_file
}

##########
## Main ##
##########

write_syscalls_tab
