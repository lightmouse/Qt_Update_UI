@echo off
if "%1" == "" goto error

"%1\regdel" "software\kvaser ab\candriver 1.0"
"%1\regdel" "software\kvaser ab\canlib32"
"%1\regdel" "system\currentcontrolset\services\kcanp"
"%1\regdel" "system\currentcontrolset\services\kcanh"
"%1\regdel" "system\currentcontrolset\services\kcans"
"%1\regdel" "system\currentcontrolset\services\kcanv"
"%1\regdel" "system\currentcontrolset\services\kcanx"
"%1\regdel" "system\currentcontrolset\services\kcanx1"
"%1\regdel" "system\currentcontrolset\services\kcany"
"%1\regdel" "system\currentcontrolset\services\kcany1"

if not "%2" == "vector" goto out
"%1\regdel" "software\vector\candriver 1.0"
"%1\regdel" "system\currentcontrolset\services\vcanv"
"%1\regdel" "system\currentcontrolset\services\vcanx"
"%1\regdel" "system\currentcontrolset\services\vcanx1"
"%1\regdel" "system\currentcontrolset\services\vcany"
"%1\regdel" "system\currentcontrolset\services\vcany1"

goto out

:error
echo This file is used when uninstalling the CAN drivers.

:out
