#!/usr/bin/bash

for I in $(ls -d ./*/); do
	cd ${I}
	git pull
	cd ..
done
