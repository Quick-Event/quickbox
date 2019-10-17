#!/bin/bash

while [[ 1 ]]; do
	rsync -avz --progress ~/t/hsh2019/html/ shv.elektroline.cz:/var/www/html/hsh2019/
	date
	sleep 300
done
