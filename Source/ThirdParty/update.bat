@echo off

for /D %%I  in (".\*") do (
	cd /D %%~fI
	git pull
	cd ..
)
