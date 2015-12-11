
echo off

cls

rem changer de disque dur
%~d1

rem changer de dossier
cd "%~p1"


rem nouveau dossier CODE
mkdir "codeCPP_%~n1"

copy "%~nx1" "codeCPP_%~n1\%~nx1"

cd "codeCPP_%~n1"


rem on creé le code !
dia2code "%~nx1"


del "%~nx1"