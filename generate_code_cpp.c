/***************************************************************************
         generate_code_cpp.c  -  Function that generates C++ code
                             -------------------
    begin                : Sat Dec 16 2000
    copyright            : (C) 2000-2001 by Javier O'Hara
                           (C) 2002-2014 by Oliver Kellogg
    email                : joh314@users.sourceforge.net
                           okellogg@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* NB: If you use CORBA stereotypes, you will need the file p_orb.h
   found in the runtime/cpp directory.  */

#include "dia2code.h"
#include "decls.h"
#include "includes.h"
#include <string.h>

#define SPEC_EXT "h"
#define BODY_EXT "cpp"

#define eq  !strcmp

static batch *gb;   /* The current batch being processed.  */

/* Utilities.  */

static void
check_umlattr (umlattribute *u, char *typename)
{
    /* Check settings that don't make sense for C++ generation.  */
    if (u->visibility == '1')
        fprintf (stderr, "%s/%s: ignoring non-visibility\n", typename, u->name);
    if (u->isstatic)
        fprintf (stderr, "%s/%s: ignoring staticness\n", typename, u->name);
}

static char *
subst (char *str, char search, char replace)
{
    char *p;
    while ((p = strchr (str, search)) != NULL)
        *p = replace;
    return str;
}

static char *
nospc (char *str)
{
    return subst (str, ' ', '_');
}

static int
pass_by_reference (umlclass *cl)
{
    char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype;
    if (strlen (st) == 0)
        return 1;
    if (is_typedef_stereo (st)) {
        umlattrlist umla = cl->attributes;
        umlclassnode *ref = find_by_name (gb->classlist, cl->name);
        if (ref == NULL)
            return 0;
        return pass_by_reference (ref->key);
    }
    return (!is_const_stereo (st) &&
            !is_enum_stereo (st));
}

static int
is_oo_class (umlclass *cl)
{
    char *st;
    if (cl == NULL)
        return 0;
    st = cl->stereotype;
    if (strlen (st) == 0)
        return 1;
    return (!is_const_stereo (st) &&
            !is_typedef_stereo (st) &&
            !is_enum_stereo (st) &&
            !is_struct_stereo (st) &&
            !eq (st, "CORBAUnion") &&
            !eq (st, "CORBAException"));
}

static int
has_oo_class (declaration *d)
{
    while (d != NULL) {
        if (d->decl_kind == dk_module) {
            if (has_oo_class (d->u.this_module->contents))
                return 1;
        } else {         /* dk_class */
            if (is_oo_class (d->u.this_class->key))
                return 1;
        }
        d = d->next;
    }
    return 0;
}

static char *
cppname (char *name)
{
    static char buf[SMALL_BUFFER];
    if (use_corba) {
        if (eq (name, "boolean") ||
            eq (name, "char") ||
            eq (name, "octet") ||
            eq (name, "short") ||
            eq (name, "long") ||
            eq (name, "float") ||
            eq (name, "double") ||
            eq (name, "string") ||
            eq (name, "any")) {
            sprintf (buf, "CORBA::%s", strtoupperfirst (name));
        } else if (eq (name, "long long")) {
            sprintf (buf, "CORBA::LongLong");
        } else if (eq (name, "unsigned short")) {
            sprintf (buf, "CORBA::UShort");
        } else if (eq (name, "unsigned long")) {
            sprintf (buf, "CORBA::ULong");
        } else if (eq (name, "unsigned long long")) {
            sprintf (buf, "CORBA::ULongLong");
        } else {
            strcpy (buf, name);
        }
    } else {
        strcpy (buf, name);
    }
    return buf;
}






static char *
fqnameSHARED (umlclassnode *node, int use_ref_type)
{
    static char buf[BIG_BUFFER];

    buf[0] = '\0';
    if (node == NULL)
        return buf;


    if (use_ref_type)
        strcat (buf, "std::shared_ptr<");
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL) {
            strcat (buf, pkglist->key->name);
            strcat (buf, "::");
            pkglist = pkglist->next;
        }
    }
    strcat (buf, node->key->name);
    if (use_ref_type)
        strcat (buf, ">");
    return buf;
}




static char *
fqname (umlclassnode *node, int use_ref_type)
{
    static char buf[BIG_BUFFER];

    buf[0] = '\0';
    if (node == NULL)
        return buf;
    if (node->key->package != NULL) {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL) {
            strcat (buf, pkglist->key->name);
            strcat (buf, "::");
            pkglist = pkglist->next;
        }
    }
    strcat (buf, node->key->name);
    if (use_ref_type)
        strcat (buf, "*");
    return buf;
}

static void
check_visibility (int *curr_vis, int new_vis)
{
    if (*curr_vis == new_vis)
        return;
    indentlevel--;
    switch (new_vis) {
      case '0':
        print ("public:\n");
        break;
      case '1':
        print ("private:\n");
        break;
      case '2':
        print ("protected:\n");
        break;
    }
    *curr_vis = new_vis;
    indentlevel++;
}







char* supprimCharAt(char * chaine1, int i){

    while(chaine1[i]!='\0'){
      chaine1[i]=chaine1[i+1];
      i++;
    }
    return chaine1;
}

//void majuscule(char *chaine)
//{
//    int i = 0;
//
//    //Tant que nous ne sommes pas arrivés a la fin de la chaine, continuer
//    while (chaine[i] != '\0')
//    {
//        /*si chaine[1] est une minuscule
//         *Car les minuscules se situent entre 97 et 122 inclusivement
//         */
//        if (chaine[i]  >= 97 &&  chaine[i] <= 122)
//
//            //Convertir en majuscules
//            chaine[i] = chaine[i] - 32;
//        i++;
//    }
//}

//strcmp
static void creerAccesseurs ( umlclassnode *node )
{



    umloplist umlo = node->key->operations;
    umlattrlist umlAtt = node->key->attributes;

    if ( umlAtt != NULL ){
        emit ("\npublic:\n");
    } else {
        return;
    }


    while (umlAtt != NULL) {

            char  nomAtt[80];
             strcpy ( nomAtt , umlAtt->key.name );
//            nomAtt =  umlAtt->key.name;
            char* teste = strstr( nomAtt , "m_");

            char nomFonctionGET[80];
            char nomFonctionSET[80];




//            char prefixeASupprimer = "m_";





            /////////////////////////////////////////////////////////////////////////////////
            //////////////// ACCESSEUR //////////////////////////////////////////////////////
            /////////////////////////////////////////////////////////////////////////////////

                char  nomAttAffiche[80];
                strcpy ( nomAttAffiche , nomAtt );
                if (teste != NULL )
                    if ( ! strcmp ( teste , nomAttAffiche  )) {
                        supprimCharAt( nomAttAffiche, 0);
                        supprimCharAt( nomAttAffiche, 0);
                    }

                // si minuscule => majuscule
                if (nomAttAffiche[0]  >= 97 &&  nomAttAffiche[0] <= 122)
                    nomAttAffiche[0] = nomAttAffiche[0] - 32;

                strcpy ( nomFonctionGET , nomAttAffiche );
                strcpy ( nomFonctionSET , nomAttAffiche );

                char  GET[80]= "get";
                char  SET[80] = "set";

                strcat ( GET , nomFonctionGET );
                strcat ( SET , nomFonctionSET );







                // LE GET ///////////////
//                print("/////////////////////////////////////////////////\n");
                print("///< Definir %s\n", nomAtt);
//                print("///\n");
//                print("/////////////////////////////////////////////////\n");
                print("void %s( %s val ){ %s = val; };\n\n", SET, umlAtt->key.type, nomAtt );





                // LE GET ///////////////
//                print("/////////////////////////////////////////////////\n");
                print("///< Acceder à %s\n", nomAtt);
//                print("///\n");
//                print("/////////////////////////////////////////////////\n");
                print("%s %s () const { return %s; };\n\n",  umlAtt->key.type, GET, nomAtt );









//
//                print("/////////////////////////////////////////////////\n");
//                print("/// \\brief Definir %s\n", umlo->key.attr.comment);
//                print("///\n");
//
//                tmpa = umlo->key.parameters;
//                while (tmpa != NULL) {
//                     print("/// \\param %s\t\t %s\n",
//                           tmpa->key.name,
//                           //kind_str(tmpa->key.kind),
//                           tmpa->key.comment);
//                           tmpa = tmpa->next;
//                }
//                print("/////////////////////////////////////////////////\n");
//            }

/*
            // pour l'indentation //
            print ("");





            if (strlen (umlo->key.attr.type) > 0) {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s (", umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL) {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);
                if (tmpa->key.value[0] != 0) {
                    if (is_valuetype)
                        fprintf (stderr, "CORBAValue %s/%s: param default "
                                 "not supported\n", name, umlo->key.attr.name);
                    else
                       emit (" = %s", tmpa->key.value);
                }
                tmpa = tmpa->next;
                if (tmpa != NULL) {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant) {
                emit (" const");
            }
            if (umlo->key.attr.value[0]) {
                // virtual
                if ((umlo->key.attr.isabstract || is_valuetype) &&
                    umlo->key.attr.name[0] != '~')
                    emit (" = %s", umlo->key.attr.value);
            }
            emit (";\n\n");*/
//            umlo = umlo->next;
      //  }











//            if ( eq ( nomAtt[0] , "m"))
//                printf ("       -> on a un M\n" );



            // suivant
            umlAtt = umlAtt->next;
    }

//    if (node->key->attributes != NULL) {
//
//            umlattrlist tmpa = umlo->key.parameters;
//



}














static int
besoinHeader ( declaration * dClass, declaration * dContents ){

            //    printf ("#######--> par ici (2) \n" );

                // on regarde si on a besoin de faire un fichier *.h
                if ( eq ( dClass->u.this_class->key->stereotype, "enum" )
                ||   eq ( dClass->u.this_class->key->stereotype, "enumeration" )
                ||   eq ( dClass->u.this_class->key->stereotype, "typedef" ) )
                {

                    declaration *dTest;
                    dTest = dContents;

                    int b_present = 0;
                    while (dTest!= NULL) {
//                        printf ("#######--> par ici (2) \n" );

                        umlassoclist  assoc = dTest->u.this_class->associations;
//                        printf ("#######--> par ici (3) \n" );

                        if ( assoc != NULL){

//                            printf ("#######--> par ici (4) \n" );

                         //   printf ("--------> ASSOCIATIONS ! : %s\n" , dTest->u.this_class->key->name );

                            while (assoc != NULL) {
                                 //   printf ("------------------------>assoc->name    :%s\n", assoc->key->name );
//
//                                    printf ("#######--> par ici (5) \n" );
//                                    printf ("#######--> par ici : %s\n", assoc->key->name );
//                                    printf ("#######--> par ici (6) \n" );
                                if ( eq ( assoc->key->name, dClass->u.this_class->key->name ) )
                                {
                                   // printf ("#######--> par ici ! \n" );
                                    //printf ("#######--> par ici ! \n" );
                                    b_present = 1;
                                    break ;
                                }
//
//
                                assoc = assoc->next;
//                                    printf ("#######--> par ici (6) \n" );
                            }
//                                    printf ("#######--> par ici (7) \n" );



                        }
                        if ( b_present ) break;
                        dTest=dTest->next;
                    }
                    if ( b_present ){
                       // printf ("------------------------> non, pas besoin de fichier\n");
                        return 0;
                    }
                    else{
                       // printf ("------------------------> oui\n");
                        return 1;
                    }


                } // c'est une classe, on ecrit
                else return 1;


        }

























static void
gen_class (umlclassnode *node)
{

    printf ( "gen_class\n");
    char *name = node->key->name;
    char *stype = node->key->stereotype;
    int is_valuetype = 0;

    int tmpv = -1;

    if (strlen (stype) > 0) {
        print ("// %s\n", stype);
        is_valuetype = eq (stype, "CORBAValue");
    }


    if ( strlen ( node->key->comment ) > 0 ) {

        print("/////////////////////////////////////////////////\n");
        print("/// \\brief %s\n", node->key->comment);
        print("///\n");
        print("/////////////////////////////////////////////////\n");
    }



    if (node->key->templates != NULL) {
        umltemplatelist template = node->key->templates;
        if (is_valuetype) {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        } else {
            print ("template <");
            while (template != NULL) {
                print ("%s %s", template->key.type, template->key.name);
                template = template->next;
                if (template != NULL)
                    emit (", ");
            }
            emit (">\n");
        }
    }

    print ("class %s", name);
    if (node->parents != NULL) {
        umlclasslist parent = node->parents;
        emit (" : ");
        while (parent != NULL) {
            emit ("public %s", fqname (parent, 0));
            parent = parent->next;
            if (parent != NULL)
                emit (", ");
        }
    } else if (is_valuetype) {
        emit (" : CORBA::ValueBase");
    }
    emit (" {\n\n");
    indentlevel++;





    tmpv = -1;
    //////// les ENUMS associés et TYPEDEF associés et USING ////////
    if (node->associations != NULL) {
        umlassoclist assoc = node->associations;


        // on regarde si on ecrit l'entete des declarations
        int testPresent = 0;
        while (assoc != NULL) {

                if  ( eq ( "enum", assoc->key->stereotype)
                     || eq ( "enumeration", assoc->key->stereotype)
                     || eq ( "using", assoc->key->stereotype)
                     || ( eq ( "typedef", assoc->key->stereotype) && strlen (assoc->name)==0) )
                     {
                        testPresent = 1;
                        break;
                     }
            assoc = assoc->next;
        }
        if (testPresent){

            emit("\n");
            emit("/////////////////////////////////////////////////\n");
            emit("// Enums & typedefs\n");
            emit("/////////////////////////////////////////////////\n");


        }






        tmpv = -1;

        //////// les enums  ////////
        assoc = node->associations;
        while (assoc != NULL) {
                /////// ENUM /////////////////
                if  ( eq ( "enum", assoc->key->stereotype) || eq ( "enumeration", assoc->key->stereotype) )
                {
                    check_visibility ( &tmpv, assoc->visibility );

                    umlattrlist umla = assoc->key->attributes;

                   // printf ( "########## ENUMERATION\n");


                    print( "/////////////////////////////////////////////////\n");
                    print( "/// \\brief %s\n" , assoc->key->comment );
                    print( "/////////////////////////////////////////////////\n");


                    print ("enum %s {\n", assoc->key->name);
                    indentlevel++;



                    while (umla != NULL) {
                        char *literal = umla->key.name;
                        check_umlattr (&umla->key, name);
                        if (strlen (umla->key.type) > 0)
                            fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
                        print ("%s", literal);

                        if (strlen (umla->key.value) > 0)
                            print (" = %s", umla->key.value);
                        if (umla->next)
                            emit (",");

                        if ( strlen (umla->key.comment ) > 0)
                            print ("    ///< %s", umla->key.comment);

                        emit ("\n");
                        umla = umla->next;
                    }
                    indentlevel--;
                    print ("};\n\n");
                }

            assoc = assoc->next;
        }

        //////// les uing  ////////
        assoc = node->associations;
        while (assoc != NULL) {

                if ( eq ( "using", assoc->key->stereotype))
                {

//                    // s'il y a un nom a l'association ou au membre
//                    // on doit l'importer en tant que membre
//                    if ( strlen (assoc->name) >0 )
//                    {
//                        assoc = assoc->next;
//                        continue;
//                    }


                    check_visibility ( &tmpv, assoc->visibility );
                    print ("using %s ", assoc->key->attributes->key.type );
                    emit ("%s;", assoc->key->name);
                    if ( strlen( assoc->key->comment ) >0 )
                        emit ("    ///< %s", assoc->key->comment );

                    emit("\n");

                }

            assoc = assoc->next;
        }



        //////// les typedefs  ////////
        assoc = node->associations;
        while (assoc != NULL) {

                if ( eq ( "typedef", assoc->key->stereotype))
                {

                    // s'il y a un nom a l'association ou au membre
                    // on doit l'importer en tant que membre
                    if ( strlen (assoc->name) >0 )
                    {
                        assoc = assoc->next;
                        continue;
                    }


                    check_visibility ( &tmpv, assoc->visibility );
                    print ("typedef %s ", assoc->key->attributes->key.type );
                    emit ("%s;", assoc->key->name);
                    if ( strlen( assoc->key->comment ) >0 )
                        emit ("    ///< %s", assoc->key->comment );

                    emit("\n");

                }

            assoc = assoc->next;
        }

      //  print ("// Associations\n");


    }


    tmpv = -1;
    //////// les OPERATIONS  ////////
    if (node->key->operations != NULL) {
        umloplist umlo = node->key->operations;
        tmpv = -1;


        emit("\n\n");
        emit("/////////////////////////////////////////////////\n");
        emit("// Méthodes\n");
        emit("/////////////////////////////////////////////////\n");


        //creerAccesseurs ( node );

        if (is_valuetype) {
            indentlevel--;
            print ("public:\n");
            indentlevel++;
        }
        while (umlo != NULL) {
            umlattrlist tmpa = umlo->key.parameters;
            if (is_valuetype) {
                if (umlo->key.attr.visibility != '0')
                    fprintf (stderr, "CORBAValue %s/%s: must be public\n",
                                     name, umlo->key.attr.name);
            } else {
                check_visibility (&tmpv, umlo->key.attr.visibility);
            }

            /* print comments on operation */
            if (strlen(umlo->key.attr.comment)) {

                print("/////////////////////////////////////////////////\n");
                print("/// \\brief %s\n", umlo->key.attr.comment);
                print("///\n");

                tmpa = umlo->key.parameters;
                while (tmpa != NULL) {
                     print("/// \\param %s\t\t %s\n",
                           tmpa->key.name,
                           //kind_str(tmpa->key.kind),
                           tmpa->key.comment);
                           tmpa = tmpa->next;
                }
                print("/////////////////////////////////////////////////\n");
            }


            // pour l'indentation
            print ("");


            //////////// static  ////////////
            if (umlo->key.attr.isstatic) {
                if (is_valuetype)
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n", name, umlo->key.attr.name);
                else
                    emit ("static ");
            }

            //////////// abstrait  ////////////
            else if (umlo->key.attr.isabstract || is_valuetype   ) {
                //printf(" --> on a une methode abstraite\n");
                emit ("virtual ");
                umlo->key.attr.value[0] = '0';

            //////////// virtuel  ////////////
            } else if ( eq (umlo->key.attr.isvirtuel , "1") ) {
                //printf(" --> on a une methode virtuelle\n");
                emit ("virtual ");
            }
//





            if (strlen (umlo->key.attr.type) > 0) {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s (", umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL) {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);
                if (tmpa->key.value[0] != 0) {
                    if (is_valuetype)
                        fprintf (stderr, "CORBAValue %s/%s: param default "
                                 "not supported\n", name, umlo->key.attr.name);
                    else
                       emit (" = %s", tmpa->key.value);
                }
                tmpa = tmpa->next;
                if (tmpa != NULL) {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant) {
                emit (" const");
            }
            if (umlo->key.attr.value[0]) {
                // virtual
                if ((umlo->key.attr.isabstract || is_valuetype) &&
                    umlo->key.attr.name[0] != '~')
                    emit (" = %s", umlo->key.attr.value);
            }
            emit (";\n\n");
            umlo = umlo->next;
        }
    }


    tmpv = -1;
    //////// les MEMBRES et membres associés  ////////
    if (node->key->attributes != NULL || node->associations != NULL ) {


        emit("\n\n");
        emit("/////////////////////////////////////////////////\n");
        emit("// Membres\n");
        emit("/////////////////////////////////////////////////\n");


        umlattrlist umla = node->key->attributes;
        if (is_valuetype) {
            print ("// Public state members\n");
            indentlevel--;
            print ("public:\n");
            indentlevel++;
            while (umla != NULL) {
                char *member = umla->key.name;
                umlclassnode *ref;
                if (umla->key.visibility != '0') {
                    umla = umla->next;
                    continue;
                }
                print ("");
                if (umla->key.isstatic) {
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n",
                                     name, member);
                }
                ref = find_by_name (gb->classlist, umla->key.type);
                if (ref != NULL)
                    eboth ("%s", fqname (ref, 1));
                else
                    eboth ("%s", cppname (umla->key.type));
                emit (" %s () { return _%s; }\n", member, member);
                print ("void %s (", member);
                if (ref != NULL) {
                    int by_ref = pass_by_reference (ref->key);
                    if (by_ref)
                        emit ("const ");
                    emit ("%s", fqname (ref, 1));
                    if (by_ref)
                        emit ("&");
                } else {
                    emit ("%s", cppname (umla->key.type));
                }
                emit (" value_) { _%s = value_; }\n");
                umla = umla->next;
            }
        }
        else {

            //print ("// Attributes\n");


        //////// les classes associés ////////
        umlassoclist assoc = node->associations;
        while (assoc != NULL) {
                if ( ! eq ( "enum", assoc->key->stereotype) && ! eq ( "enumeration", assoc->key->stereotype) )
                {



                    // s'il y a un nom a l'association ou au membre
                    // on doit l'importer en tant que membre
                    if ( eq ( "typedef", assoc->key->stereotype)
                    && strlen (assoc->name)  == 0 )
                    {
                        assoc = assoc->next;
                        continue;
                    }


                    check_visibility ( &tmpv, assoc->visibility );


                    umlclassnode *ref;

                    ///////// association ayant un nom ////////////////
                    //            if (assoc->name[0] != '\0')
                    //            {
                    //   printf ("------> association AVEC nom !!!: \n");

                    print ("");


                    //////// MULTIPLICITé /////////
                    int bVector = 0;
                    if (eq ( assoc->multiplicity, "*" ))
                        bVector = 1;
                    if (eq ( assoc->multiplicity, "0..*" ))
                        bVector = 1;
                    if (bVector)
                        emit ("std::vector<");




                    ////////  ? /////////
                    ref = find_by_name (gb->classlist, assoc->key->name);
                    if (ref != NULL){

                        emit ("%s", fqnameSHARED (ref, !assoc->composite));
                    }
                    else
                        emit ("%s", cppname (assoc->key->name));


                    if (bVector)
                        emit ("> ");

                    emit (" %s;", assoc->name);


                    if ( strlen(assoc->key->comment ) > 0 )
                        emit ("     ///< %s", assoc->key->comment);

                    emit ("\n", assoc->name);
                }

            assoc = assoc->next;
        }
    }





        //////// les ATTRIBUTS  ////////
        while (umla != NULL) {
            check_visibility (&tmpv, umla->key.visibility);



            print ("");
            if (umla->key.isstatic) {
                emit ("static ");
            }
            emit ("%s %s", umla->key.type, umla->key.name);

            emit (";");

            if (strlen(umla->key.comment)) {
                print("///< %s", umla->key.comment);
            }
            print("\n");


            umla = umla->next;
        }


    }


    //////// ???? ATTRIBUTS  ???? ////////
    if (node->key->attributes != NULL && is_valuetype) {
        umlattrlist umla = node->key->attributes;
        emit ("\n");
        indentlevel--;
        print ("private:  // State member implementation\n");
        indentlevel++;
        while (umla != NULL) {
            umlclassnode *ref = find_by_name (gb->classlist, umla->key.type);
            print ("");
            if (ref != NULL) {
                emit ("%s", fqname (ref, is_oo_class (ref->key)));
            } else
                emit ("%s", cppname (umla->key.type));
            emit (" _%s;\n", umla->key.name);
            umla = umla->next;
        }
    }




    indentlevel--;
    print ("\n}; // fin class %s\n\n", name );


}

















static void
gen_decl (declaration *d)
{
    char *name;
    char *stype;
    umlclassnode *node;
    umlattrlist umla;

    if (d == NULL)
        return;

//    if (d->decl_kind == dk_module) {
//        name = d->u.this_module->pkg->name;
//        print ("namespace %s {\n\n", name);
//        indentlevel++;
//        d = d->u.this_module->contents;
//        while (d != NULL) {
//            gen_decl (d);
//            d = d->next;
//        }
//        indentlevel--;
//        print ("};\n\n", name);
//        return;
//    }

    node = d->u.this_class;
    stype = node->key->stereotype;
    name = node->key->name;
    umla = node->key->attributes;
    if (strlen (stype) == 0) {
        gen_class (node);
        return;
    }

    if (eq (stype, "CORBANative")) {
        print ("// CORBANative: %s \n\n", name);

    }
    else if (is_const_stereo (stype)) {
        if (umla == NULL) {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        print ("const %s %s = %s;\n\n", cppname (umla->key.type), name,
                                                 umla->key.value);




    }/////// enum /////////////
    else if (is_enum_stereo (stype)) {


        print( "/////////////////////////////////////////////////\n");
        print( "/// \\brief %s\n" , node->key->comment );
        print( "///\n");
        print( "/////////////////////////////////////////////////\n");





        print ("enum %s {\n", name);
        indentlevel++;
        while (umla != NULL) {
            char *literal = umla->key.name;
            check_umlattr (&umla->key, name);
            if (strlen (umla->key.type) > 0)
                fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
            print ("%s", literal);

            if (strlen (umla->key.value) > 0)
                print (" = %s", umla->key.value);
            if (umla->next)
                emit (",");

            if ( strlen (umla->key.comment ) > 0)
                print ("    ///< %s", umla->key.comment);

            emit ("\n");
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    }



    /////// struct /////////////
    else if (is_struct_stereo (stype)) {
        print ("struct %s {\n", name);
        indentlevel++;
        while (umla != NULL) {
            check_umlattr (&umla->key, name);
            print ("%s %s", cppname (umla->key.type), umla->key.name);
            if (strlen (umla->key.value) > 0)
                fprintf (stderr, "%s/%s: ignoring value\n",
                                 name, umla->key.name);
            emit (";\n");
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    } else if (eq (stype, "CORBAException")) {
        fprintf (stderr, "%s: CORBAException not yet implemented\n", name);

    } else if (eq (stype, "CORBAUnion")) {
        umlattrnode *sw = umla;
        if (sw == NULL) {
            fprintf (stderr, "Error: attributes not set at union %s\n", name);
            exit (1);
        }
        fprintf (stderr, "%s: CORBAUnion not yet fully implemented\n", name);
        print ("class %s {  // CORBAUnion\n", name);
        print ("public:\n", name);
        indentlevel++;
        print ("%s _d();  // body TBD\n\n", umla->key.type);
        umla = umla->next;
        while (umla != NULL) {
            check_umlattr (&umla->key, name);
            print ("%s %s ();  // body TBD\n",
                   cppname (umla->key.type), umla->key.name);
            print ("void %s (%s _value);  // body TBD\n\n", umla->key.name,
                   cppname (umla->key.type));
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    } else if (is_typedef_stereo (stype)) {
        /* Conventions for CORBATypedef:
           The first (and only) attribute contains the following:
           Name:   Empty - the name is taken from the class.
           Type:   Name of the original type which is typedefed.
           Value:  Optionally contains array dimension(s) of the typedef.
                   These dimensions are given in square brackets, e.g.
                   [3][10]
         */
        if (umla == NULL) {
            fprintf (stderr, "Error: first attribute (impl type) not set "
                             "at typedef %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)  {
            fprintf (stderr, "Warning: typedef %s: ignoring name field "
                        "in implementation type attribute\n", name);
        }
        print ("typedef %s %s%s;\n\n", cppname (umla->key.type), name,
                                                umla->key.value);


    /////// classe /////////////
    } else {
        gen_class (node);
    }





}




static void
ecrire_Head( declaration * dClass , batch* b, char* name, char * nomEspace, char * file_ext ){
    //printf ("  -> ecrire_Head 0 <-------------- \n" );


    char filename[256];



    sprintf (filename, "%s.%s", name, file_ext );



                spec = open_outfile (filename, b);
                if (spec == NULL) {
                    dClass = dClass->next;
                    return;
                }

                char* tmpname = strtoupper(name);


               // print ("  -> ecrire_Head 1 <-------------- hop : %s\n", nomEspace );


                print("#ifndef %s__H\n", tmpname);
                print("#define %s__H\n\n", tmpname);




                print("/////////////////////////////////////////////////\n");
                print("// Headers\n");
                print("/////////////////////////////////////////////////\n");


                includes = NULL;
                determine_includes (dClass, b);
                if (use_corba)
                    print ("#include <p_orb.h>\n\n");
                if (includes) {
                    namelist incfile = includes;
                    while (incfile != NULL) {
                        if (!eq (incfile->name, name)) {
                            print ("#include \"%s.%s\"\n", incfile->name, file_ext);
                        }
                        incfile = incfile->next;
                    }
                    print ("\n");
                }

                print ("\n\n");


                if ( strlen(nomEspace) >0 )
                    print ( "namespace %s {\n\n" , nomEspace );



                gen_decl (dClass);



                if ( strlen(nomEspace) >0 )
                    print ( "} // fin namespace %s\n\n" , nomEspace );

                indentlevel = 0;  //just for safety (should be 0 already)
                print("#endif\n");



                if ( ! is_enum_stereo ( dClass->u.this_class->key->stereotype ) ) /// alors c'est une classe?!? ou pas ...
                {
                    print("\n");
                    print("\n");
                    print("////////////////////////////////////////////////////////////\n");
                    print("/// \class %s\n", name );
                    print("/// \ingroup \n");
                    print("///\n");
                    print("/// \see \n");
                    print("///\n");
                    print("////////////////////////////////////////////////////////////\n");
                }

                fclose (spec);


}

void
generate_code_cpp (batch *b)
{
/*
    ///////// DEBUG ////////////////
    printf( " DEBUG 1\n" );
    while ( b->classlist != NULL )
    {
        printf( " generate_code_cpp : %s\n", b->classlist->key->name );
        b->classlist = b->classlist->next;
    }
    printf( " DEBUG 1 fin \n\n" );
    ///////// DEBUG ////////////////


*/





    declaration *d;
    umlclasslist tmplist = b->classlist;
    umlclasslist tmplist2 = b->classlist;
    umlclasslist listClean;

    FILE *licensefile = NULL;

    gb = b;

    if (file_ext == NULL)
        file_ext = "h";

    if (body_file_ext == NULL)
        body_file_ext = "cpp";


    /* open license file */
    if (b->license != NULL) {
        licensefile = fopen (b->license, "r");
        if (!licensefile) {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }

//
//
//    while (tmplist != NULL) {
//        //printf ( "generate_code_cpp 01 :  tmplist = %s \n", tmplist->key->name );
//        if (! (is_present (b->classes, tmplist->key->name) ^ b->mask)) {
//           // printf ( "  ->  par ici\n", tmplist->key->name );
//            push (tmplist, b);
//        }
//        tmplist = tmplist->next;
//    }



    ///////////// mon bordel |--> //////////////////////////////////////////////////////
    tmplist2 = b->classlist;
    while ( tmplist2 != NULL ) {

//        if ( tmplist2 != NULL ) {
            printf ( "  -> test doublons : %s\n", tmplist2->key->name );
            umlclasslist tmplist3 = tmplist2;
            while ( tmplist3 != NULL ) {
                printf ( "      -> tmplist3 : %s\n", tmplist3->key->name );
                if ( tmplist3->key  != tmplist2->key
                &&   eq ( tmplist3->key->name        , tmplist2->key->name       )
                &&   eq ( tmplist3->key->stereotype  , tmplist2->key->stereotype ) )
                    printf( "       -----> identiques mais differents\n" );
/*
                if ( eq ( tmplist3->key->name        , tmplist2->key->name       )
                &&   eq ( tmplist3->key->stereotype  , tmplist2->key->stereotype ) ){
                    printf( "       -----> memeNom et meme stereo\n" );
//                    printf( "   On compare les attributs et memeNom et meme stereo\n" );
                }
*/
                tmplist3 = tmplist3->next;

            }
//        }
        tmplist2 = tmplist2->next;

    }

//                if ( eq ( classlistTest->key->name        , tmplist->key->name       )
//                &&   eq ( classlistTest->key->stereotype  , tmplist->key->stereotype ) ){
//                    printf( "  -> memeNom et meme stereo\n" );
//                    printf( "   On compare les attributs et memeNom et meme stereo\n" );
//                }
            /*
        umlpackagelist tmppcklist = packagelist;
        while ( tmppcklist != NULL ) {
            if ( is_inside(&dummypcklist->key->geom, &tmppcklist->key->geom) ) {
                if ( tmppcklist->key->parent == NULL ) {
                    tmppcklist->key->parent = dummypcklist->key;
                } else {
                    if ( ! is_inside ( &dummypcklist->key->geom, &tmppcklist->key->parent->geom ) ) {
                        tmppcklist->key->parent = dummypcklist->key;
                    }
                }
            }
            tmppcklist = tmppcklist->next;
        }*/
    ///////////// <--| mon bordel /////////////////////////////////////////////////////////////




/*
    ///////// DEBUG ////////////////
    printf( " DEBUG 2\n" );
    while ( tmplist != NULL )
    {
        printf( " generate_code_cpp : %s\n", tmplist->key->name );
        tmplist = tmplist->next;
    }
    printf( " DEBUG 2 fin \n\n" );
    ///////// DEBUG ////////////////*/

    ///////// DEBUG ////////////////
//    printf( " DEBUG 1--------------\n" );
//    int i = 0;
//    while ( b->classlist != NULL )
//    {
//        printf( "       DEBUG : %s\n", b->classlist->key->name );
//
//        if (i==0) push (listClean, b->classlist->key);
//        else {
//            while (listClean != NULL) {
//                printf ( "          ---------> tmplist = %s \n", listClean->key->name );
//                listClean = listClean->next;
//            }
//        }
//        i++;
//        b->classlist = b->classlist->next;
//    }
//    printf( " DEBUG 1 suite ------------------------------\n" );
//
//     while (listClean != NULL) {
//        printf ( " ---------> listClean = %s \n", listClean->key->name );
//        listClean = listClean->next;
//    }
//
//
//
//    printf( " DEBUG 1 fin ------------------\n\n" );
//    ///////// DEBUG ////////////////


    // Generate a file for each outer declaration.
    d = decls;
    while (d != NULL) {



        char *name, *tmpname;
        char filename[BIG_BUFFER];
        char* nomEspace ="";
        declaration * dClass;


        // si c'est un namespace
        if (d->decl_kind == dk_module) {


            nomEspace = d->u.this_module->pkg->name;

            printf ("namespace <-------------- %s\n", nomEspace );

            //indentlevel++;
            dClass = d->u.this_module->contents;

            while (dClass != NULL) {

         printf ( "generate_code_cpp ------------------------------------ \n");
           //     printf ( "generate_code_cpp 04 :  dClass = %s \n", dClass->u.this_class->key->name );

                // on regarde si on a besoin de faire un fichier *.h
                if ( ! besoinHeader ( dClass , d->u.this_module->contents ) ) {
                        //printf ("------------------------> PAS besoin d'un header\n");
                        dClass = dClass->next;
                        continue;
                }
                //else printf ("------------------------> besoin d'un header\n");

                // si besoin d'un *.h -> on continue
                name = dClass->u.this_module->pkg->name;

                /////////////////////////////////////
                ecrire_Head ( dClass, b, name , nomEspace , file_ext);
                /////////////////////////////////////

                dClass = dClass->next;
            }



        } // si c'est pas dans un namespace
        else {


            printf ("pas namespace <-------------- \n" );
         printf ( "generate_code_cpp ------------------------------------ \n");
            //printf ( "generate_code_cpp 05 :  d = %s \n", d->u.this_class->key->name );


            // on regarde si on a besoin de faire un fichier *.h
            if ( ! besoinHeader ( d , d ) ) {
//                    printf ("------------------------> PAS besoin d'un header\n");
                    d = d->next;
                    continue;
            }

            // si besoin d'un *.h -> on continue
            char * name = d->u.this_module->pkg->name;

            /////////////////////////////////////
            ecrire_Head ( d, b, name , nomEspace , file_ext);
            /////////////////////////////////////

        }
        d = d->next;

    }



  // printf("\n\n\n\n\n####### CREER CPP  #######\n...");
//    system("pause");

    generate_code_cpp_Body (b);

//    printf("\n####### FIN CREER CPP #######\n...");
//    system("pause");


}



static void
gen_body (umlclassnode *node)
{
    char *name = node->key->name;
    char *stype = node->key->stereotype;
    int is_valuetype = 0;


    if (node->key->attributes != NULL) {
        int    testStatic = 0;
        umlattrlist umla = node->key->attributes;

        while (umla != NULL) {
            if (  umla->key.isstatic )
                testStatic = 1;

            umla = umla->next;
        }


        if (testStatic) {

            print ("\n/////////////////////////////////////////////////\n");
            print ("// Initialisation des statics\n");
            print ("/////////////////////////////////////////////////\n");

            umlattrlist umla = node->key->attributes;

            while (umla != NULL) {
                if (  umla->key.isstatic ) {
                    emit ("%s  %s::%s", umla->key.type, name, umla->key.name );
                    if (umla->key.value[0] != 0)
                        emit (" = %s", umla->key.value);
                    print (".\n");
                }
                umla = umla->next;
            }
            print ("\n\n");
        }
    }



    if (node->key->operations != NULL) {


//        creerAccesseurs ( node );

        umloplist umlo = node->key->operations;
        int tmpv = -1;

        while (umlo != NULL) {
            umlattrlist tmpa = umlo->key.parameters;

            print("/////////////////////////////////////////////////\n");

            if (strlen (umlo->key.attr.type) > 0) {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s::%s (", name, umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL) {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);

                tmpa = tmpa->next;
                if (tmpa != NULL) {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant) {
                emit (" const");
            }
                    print ("\n");

           if ( strcmp(name, umlo->key.attr.name) == 0  ) {


                if (node->key->attributes != NULL) {
                    umlattrlist umla = node->key->attributes;


                    int compteur = 0;
                    while (umla != NULL) {
                        if (  umla->key.isstatic ) {
                        }else {
         //               check_visibility (&tmpv, umla->key.visibility);
                            if (compteur == 0)  print (": ");
                            else print (", ");

                            emit ("%s ( %s )\n", umla->key.name, umla->key.value );
                            compteur++;

                        }

                        umla = umla->next;
                    }
                }

            }
            ////////////////////////////////////////////////////////////

            emit ("\{\n    \n\}\n\n\n");
            umlo = umlo->next;
        }
    }
}

static void
gen_declBody (declaration *d)
{


    char *name;
    char *stype;
    umlclassnode *node;
    umlattrlist umla;
    umloplist umlop;

    if (d == NULL)
        return;

    node = d->u.this_class;
    stype = node->key->stereotype;
    name = node->key->name;
    umla = node->key->attributes;
    umlop = node->key->operations;

    // on passe le cpp si pas de fonctions (ni de static a declarer ?)
    if ( umlop == NULL ){
       // printf("\n\n    ------> pas de methodes ici : %s", name );
        return;
    }



    ////////////// UN MACHIN DE NAMESPACE |-> ////////////////////////
    if (d->decl_kind == dk_module) {
       // name = d->u.this_module->pkg->name;
    /*    print ("namespace %s {\n\n", name);
        indentlevel++;
        d = d->u.this_module->contents;
        while (d != NULL) {
            gen_declBody (d);
            d = d->next;
        }
        indentlevel--;*/
        //print (" //------>>     on a un NAMSPACE : %s\n", name);
        return;
    }

    ////////////// <-| UN MACHIN DE NAMESPACE   ////////////////////////




    if (strlen (stype) == 0) {
        gen_body (node);
        return;
    }

    if (eq (stype, "CORBANative")) {
        print ("// CORBANative: %s \n\n", name);

    } else if (is_const_stereo (stype)) {
        if (umla == NULL) {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        print ("const %s %s = %s;\n\n", cppname (umla->key.type), name,
                                                 umla->key.value);

    } else {

        gen_body (node);
    }
}

void
generate_code_cpp_Body (batch *b)
{
    declaration *d;
    umlclasslist tmplist = b->classlist;
    FILE *licensefile = NULL;

    gb = b;

    if (file_ext == NULL)
        file_ext = "h";

    if (body_file_ext == NULL)
        body_file_ext = "cpp";


    /* open license file */
    if (b->license != NULL) {
        licensefile = fopen (b->license, "r");
        if (!licensefile) {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }

    while (tmplist != NULL) {
        if (! (is_present (b->classes, tmplist->key->name) ^ b->mask)) {
            push (tmplist, b);
        }
        tmplist = tmplist->next;
    }

    /* Generate a file for each outer declaration.  */
    d = decls;
    while (d != NULL) {
        char *name, *tmpname;
        char filename[BIG_BUFFER];


//    printf("\n####### Nouveau noeud #######n...");
//    system("pause");
//        ///////////////////////////////////////////////////////////////////////
//        // on passe le cpp si pas de fonctions (ni de static a declarer ?)
//        if ( d->u.this_class->key->operations == NULL ){
//            printf("\n\n    ------> pas de methodes ici : %s", name );
//        } else {
//        ///////////////////////////////////////////////////////////////////////




        ///////////////////////////////////////////////////////////////////////
        // on passe le cpp si pas de fonctions (ni de static a declarer ?)
//        if ( d->u.this_class->key->operations == NULL ){
//        && is_enum_stereo ( d->u.this_class->key->stereotype ) ){
//
//            printf( "\n\n    ------> pas de CPP a ecrire ici: \n ");
//
//            d = d->next;
//            continue;
//        }
        /////////////////////////////////////////////////////////////////////



//////////////// On regarde si on a besoin de faire un fichier cpp    ///////////////////////////////////////////////////////////////























//////////////// On ecris le fichier cpp    ///////////////////////////////////////////////////////////////
            // si on est dans un namespace
            if (d->decl_kind == dk_module) {






                name = d->u.this_module->pkg->name;




                char* nomEspace;

                nomEspace = d->u.this_module->pkg->name;

                //printf ("namespace CPP CPP CPPC PPC PPPC PCPPC PPPC PCPC P <-------------- yeah : %s\n", nomEspace );

                //indentlevel++;
                declaration * dClass = d->u.this_module->contents;
                while (dClass != NULL) {


                        //printf( "---> besoin d'un CPP ? : %s\n", dClass->u.this_class->key->name );

                        /////  TYPEDEF /////
                        if ( eq ( dClass->u.this_class->key->stereotype , "typedef") ){
//                            printf( "    ------> c'est un TYPEDEF, on quite.\n");
                            dClass = dClass->next;
                            continue;
                        } //else  printf( "    ------> c'est PAS un TYPEDEF, on continue ...\n");




                //        printf( "\non creer le CPP ?\n\n ");

                        /////  ENUM /////
                        if ( is_enum_stereo ( dClass->u.this_class->key->stereotype ) ){
                           // printf( "    ------> c'est un ENUM, on quite.\n");
                            dClass = dClass->next;
                            continue;
                        } //else  printf( "    ------> c'est PAS un ENUM, on continue ...\n");





                        /////  classes :  /////
                        if (dClass->decl_kind != dk_module) {

                            int creerLeCPP = 0;

                            /////  si il n'ya a pas de static a initialiser /////
                            if ( dClass->u.this_class->key->attributes != NULL) {
                                int    testStatic = 0;
                                umlattrlist umla = dClass->u.this_class->key->attributes;

                                while (umla != NULL) {
                                    if (  umla->key.isstatic )
                                        testStatic = 1;

                                    umla = umla->next;
                                }

                                if ( !testStatic){
                                    //printf( "    ------> il n'y a PAS de STATIC, on continue ...\n");
                                    creerLeCPP = 1;
                                }
                            }

                            /////  Pas d'operations /////
                            if ( dClass->u.this_class->key->operations == NULL && creerLeCPP ){
                                //printf( "    ------> pas d'OPERATIONS ici\n");
                                creerLeCPP = 0;
                            } else creerLeCPP = 1;



                            if ( creerLeCPP )
                            {
                                //printf( "------> on ceer le CPP\n");
                            }
                            else{
                                //printf( "------> on ceer PAS le CPP\n");
                                dClass = dClass->next;
                                continue;
                            }


                        }




                        name = dClass->u.this_class->key->name;
                        sprintf (filename, "%s.%s", name, body_file_ext);


                        spec = open_outfile (filename, b);
                        if (spec == NULL) {
                            dClass = dClass->next;
                            continue;
                        }

                        tmpname = strtoupper(name);

                        /* add license to the header */
                        if (b->license != NULL) {
                            char lc;
                            rewind (licensefile);
                            while ((lc = fgetc (licensefile)) != EOF)
                                print ("%c", lc);
                        }

                        print("/////////////////////////////////////////////////\n");
                        print("// Headers\n");
                        print("/////////////////////////////////////////////////\n");
                        print("#include <%s.h>\n",name );
                        print ("\n");









                        print ("\n\n");

                        print ( "namespace %s {\n\n" , nomEspace );


                        gen_declBody (dClass);


                        print ( "} // fin namespace %s\n\n" , nomEspace );









                        indentlevel = 0;  /* just for safety (should be 0 already) */


                        fclose (spec);


                        dClass = dClass->next;
            }




        }

        else {          /* dk_class */


            name = d->u.this_class->key->name;
            sprintf (filename, "%s.%s", name, body_file_ext);


            spec = open_outfile (filename, b);
            if (spec == NULL) {
                d = d->next;
                continue;
            }

            tmpname = strtoupper(name);

            /* add license to the header */
            if (b->license != NULL) {
                char lc;
                rewind (licensefile);
                while ((lc = fgetc (licensefile)) != EOF)
                    print ("%c", lc);
            }

            print("/////////////////////////////////////////////////\n");
            print("// Headers\n");
            print("/////////////////////////////////////////////////\n");
            print("#include <%s.h>\n",name );
            print ("\n");

            gen_declBody (d);

            indentlevel = 0;  /* just for safety (should be 0 already) */


            fclose (spec);

        }

    d = d->next;

    }
}
























