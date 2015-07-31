/*
 * objects - Programmspeicher Datenbereich
 *
 * Copyright (C) port GmbH 1998-2005
 *
 * The reproduction, transmission or use of this document or its contents
 * is not permitted without express written authority. Offenders will be
 * liable for damages. All rights, including rights created by patent grant
 * or registration of a utility model or design, are reserved.
 *
 * Technical modifications possible.
 *
 * Technical specifications and features are binding only insofar as they
 * are specifically and expressly agreed upon in a written contract.
 * ------------------------------------------------------------------------
 * $Header: /z2/cvsroot/library/co_lib/examples/s1/objects.c,v 1.5 2005/03/07 14:02:16 ro Exp $
 * ------------------------------------------------------------------------
 *
 * modification history
 * --------------------
 * (not supported with examples)
 *
 *
 *
 *
 *
 * ------------------------------------------------------------------------
 *
*/

/**
*  \file objects.c
*  \author port GmbH Halle (Saale)
*  $Revision: 1.5 $
*  $Date: 2005/03/07 14:02:16 $
*
*--Das Modul enthält die Definition des CANopen Objektverzeichnisses.
*++This module contains the definition of the object dictionary.
* \par
*--Durch Definition von DEF_OBJ_DIC
*--wird der globale Speicher mit diesem Modul belegt.
*++With the define DEF_OBJ_DIC global memory is allocated
*++for the object dictionary.
* \par
*--Durch Verlagerung in ein extra Modul
*--ist es möglich
*--aus dem nach der Kompilation vorliegenden Listingfile
*--Informationen zum Speicherbedarf zu entnehmen.
*++By doing so it is possible to extract
*++information about the memory requirement
*++out of the listing file.
*
*/

#include <stdio.h>

#define DEF_OBJ_DIC
#include <cal_conf.h>      /* !!! first file to include for all CANopen */

#include <co_acces.h>
#include <co_pdo.h>
#include <co_sdo.h>
#include <co_stru.h>

#define DEF_OBJ_DIC
#include <objects.h>

