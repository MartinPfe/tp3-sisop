#!/usr/bin/env bash

usage="
NOMBRE:
	./executer.sh - Genera dos procesos no emparentados que se comunicaran por una tuberia.

SINTAXIS:

	./executer.sh [-f FifoPath]  [-i DirectorioFacturacion]
	
	./executer.sh [ -h ]

Donde:
	-h Muestra la ayuda del script

	-f Path del FIFO / Tuberia.
	-i Directorio donde se encuentran las facturaciones realizadas.

EXAMPLE:

	./executer.sh -f fifo -i facturacion/

	./executer.sh -h

"

function indicarAyuda(){
	echo "Vea $1 -h para mas informacion"
	exit 1
}

while getopts ":hp:d:" o; do	
	case "${o}" in	
		p)
			p="${OPTARG}"
			[ ! -e "$p" ] && echo "$0: Fifo/Pipe inexistente" && indicarAyuda $0
			;;
		d)
			d="${OPTARG}"
			[ ! -d "$d" ] && echo "$0: Directorio Facturacion inexistente" && indicarAyuda $0
			;;

		h)
			echo "$usage"
			exit
			;;
		:)
			echo "$0: Argumento faltante para" "'${OPTARG}'" && indicarAyuda $0
			;;
		\?*)
			echo "$0: Opcion invalida:" "'-${OPTARG}'" && indicarAyuda $0
			;;
	esac
done

[ $# -lt 4 ] && echo "$0: Cantidad de parametros menor a la esperada" && indicarAyuda $0
[ ! -n "$d" ] && echo "$0: Directorio Facturacion erroneo" && indicarAyuda $0
[ ! -n "$p" ] && echo "$0: PIPE erroneo" && indicarAyuda $0

bin/ProcesoA.out $p $d &
bin/ProcesoB.out $p
