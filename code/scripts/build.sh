#!/bin/bash

set -e


save_path=`pwd`
cd ..
root_path=`pwd`
dirent_name=`basename ${root_path}`
if [ "$release_path"x == ""x ]
then
	release_path="${root_path}/../../../release"
	if [ ! -d ${release_path} ]
	then
		echo "${release_path} not exist!"
		exit -1
	fi
fi

function compile_exec
{
	compile_ret=1
	old_path=`pwd`
	cd $1
	if [ -f $1/Makefile ]
	then 
		echo "######### compile `pwd`/Makefile ..."
		make clean;make
		echo "######### compile `pwd`/Makefile ok"
		compile_ret=0	
	fi
	file_list=`ls`
	for file_item in ${file_list}
	do
		if [ -f "${file_item}/Makefile" ]
		then
			cd $1/${file_item}	
			echo "######### compile `pwd`/Makefile ..."
			make clean;make
			echo "######### compile `pwd`/Makefile ok"
			cd $1
			compile_ret=0
		fi
	done
	cd ${old_path}
}


#±‡“Î
compile_exec ${root_path}/src/lib
if [ ${compile_ret} -eq 0 ]
then
	#øΩ±¥
	cd ${root_path}
	if [ ! -d ${release_path}/${dirent_name} ]
	then
		mkdir -p ${release_path}/${dirent_name}	
	else
		rm -rf ${release_path}/${dirent_name}/lib
		rm -rf ${release_path}/${dirent_name}/include
	fi
	cp -r include lib ${release_path}/${dirent_name}
fi

#±‡“Î
compile_exec ${root_path}/src/app
if [ ${compile_ret} -eq 0 ]
then
	#øΩ±¥
	cd ${root_path}
	if [ ! -d ${release_path}/${dirent_name} ]
	then
		mkdir -p ${release_path}/${dirent_name}	
	else
		rm -rf ${release_path}/${dirent_name}/bin
	fi
	cp -r bin ${release_path}/${dirent_name}
fi

cd $save_path

set +e
