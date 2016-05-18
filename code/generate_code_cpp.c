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


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>



#include <wchar.h>

#define SPEC_EXT "h"
#define BODY_EXT "cpp"

#define eq  !strcmp


// mes declarations ///////////
int b_inclMemory ;
int b_inclMap ;
int b_inclVector ;
int b_inclFunctional ;
int b_inclSFMLGraph ;


char listClassesIncl[1024] ="";

//////////////////////////////

static batch *gb;   /* The current batch being processed.  */
static char *sscanfmt()
{
    static char buf[16];
    sprintf (buf, "#%%%d[^#]#", SMALL_BUFFER - 1);
    return buf;
}
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
    if (is_typedef_stereo (st))
    {
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
    while (d != NULL)
    {
        if (d->decl_kind == dk_module)
        {
            if (has_oo_class (d->u.this_module->contents))
                return 1;
        }
        else             /* dk_class */
        {
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
    if (use_corba)
    {
        if (eq (name, "boolean") ||
                eq (name, "char") ||
                eq (name, "octet") ||
                eq (name, "short") ||
                eq (name, "long") ||
                eq (name, "float") ||
                eq (name, "double") ||
                eq (name, "string") ||
                eq (name, "any"))
        {
            sprintf (buf, "CORBA::%s", strtoupperfirst (name));
        }
        else if (eq (name, "long long"))
        {
            sprintf (buf, "CORBA::LongLong");
        }
        else if (eq (name, "unsigned short"))
        {
            sprintf (buf, "CORBA::UShort");
        }
        else if (eq (name, "unsigned long"))
        {
            sprintf (buf, "CORBA::ULong");
        }
        else if (eq (name, "unsigned long long"))
        {
            sprintf (buf, "CORBA::ULongLong");
        }
        else
        {
            strcpy (buf, name);
        }
    }
    else
    {
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
    if ( node->key->package != NULL
    && ! eq ( node->key->stereotype , "typedef" ) )
    {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL)
        {
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
    if (node->key->package != NULL)
    {
        umlpackagelist pkglist = make_package_list (node->key->package);
        while (pkglist != NULL)
        {
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
    switch (new_vis)
    {
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







char* supprimCharAt(char * chaine1, int i)
{

    while(chaine1[i]!='\0')
    {
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






char *str_sub (const char *s, unsigned int start, unsigned int end)
{
   char *new_s = NULL;

   if (s != NULL && start < end)
   {
/* (1)*/
      new_s = malloc (sizeof (*new_s) * (end - start + 2));
      if (new_s != NULL)
      {
         int i;

/* (2) */
         for (i = start; i <= end; i++)
         {
/* (3) */
            new_s[i-start] = s[i];
         }
         new_s[i-start] = '\0';
      }
      else
      {
         fprintf (stderr, "Memoire insuffisante\n");
         exit (EXIT_FAILURE);
      }
   }
   return new_s;
}




void
accesseurSimple( char* nom , char* nomType, char * commentaires , int composite )
{

    //      printf (" ***** A *******************************************************************\n");

    char  nomAtt[2048];
    strcpy ( nomAtt , nom );
    char* teste = strstr( nomAtt , "m_");
    char    nomFonctionGET[80];
    char    nomFonctionSET[80];
    char    nomAttAffiche[80];
    strcpy ( nomAttAffiche , nomAtt );
    if (teste != NULL )
        if ( ! strcmp ( teste , nomAttAffiche  ))
        {
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

    // LE SET ///////////////
    if ( strstr( commentaires , "#GS" ) != NULL
    ||   strstr( commentaires , "#S" )  != NULL )
    {

        char * shared1 = "";
        char * shared2 = "";

        if ( !composite ) {
            shared1 = "std::shared_ptr<";
            shared2 = ">";
        }

        print("///< Definir %s\n", nomAtt);
        print("void %s( %s%s%s val ){ %s = val; };\n\n", SET, shared1, nomType, shared2 , nomAtt );
    }
    //        printf (" ***** G *******************************************************************\n");
    // LE GET ///////////////
    if ( strstr( commentaires , "#GS" ) != NULL
    ||   strstr( commentaires , "#G" )  != NULL )
    {
        printf (" **** LE GET *********************************************************************\n");

        char * shared1 = "";
        char * shared2 = "";

        if ( !composite ) {
            shared1 = "std::shared_ptr<";
            shared2 = ">";
        }
        print("///< Acceder à %s\n", nomAtt);
        print("%s%s%s %s () const { return %s; };\n\n", shared1,  nomType, shared2 , GET, nomAtt );
    }
    //        printf (" ***** H *******************************************************************\n");


}



void
accesseurVector( char* nom , char* nomType ,char * commentaires )
{
    char    nomAtt[2048];
    strcpy ( nomAtt , nom );
    char*   teste = strstr( nomAtt , "m_");

    char    nomFonctionAjouter[80];
    char    nomFonctionRetirer[80];
    char    nomFonctionVider[80];
    char    nomFonctionGet[80];

    char    nomAttAffiche[80];

    strcpy ( nomAttAffiche , nomAtt );





    //
    if (teste != NULL )
        if ( ! strcmp ( teste , nomAttAffiche  ))
        {
            supprimCharAt( nomAttAffiche, 0);
            supprimCharAt( nomAttAffiche, 0);
        }




    // si minuscule => majuscule
    if (nomAttAffiche[0]  >= 97 &&  nomAttAffiche[0] <= 122)
        nomAttAffiche[0] = nomAttAffiche[0] - 32;


    // si on a  un s a la fin on le vire
    char    nomAttAfficheSingulier[80];
    strcpy ( nomAttAfficheSingulier , nomAttAffiche );
    if ( nomAttAfficheSingulier[ strlen( nomAttAfficheSingulier ) -1 ] == 's' )
        nomAttAfficheSingulier[ strlen( nomAttAfficheSingulier ) -1 ] = '\0';



    strcpy ( nomFonctionAjouter , nomAttAfficheSingulier );
    strcpy ( nomFonctionRetirer , nomAttAfficheSingulier );
    strcpy ( nomFonctionVider , nomAttAffiche );
    strcpy ( nomFonctionGet , nomAttAfficheSingulier );




    char  ajouter[80]= "ajouter";
    char  retirer[80] = "retirer";
    char  vider[80] = "vider";
    char  get[80] = "get";



    strcat ( ajouter , nomFonctionAjouter );
    strcat ( retirer , nomFonctionRetirer );
    strcat ( vider , nomFonctionVider );
    strcat ( get , nomFonctionGet );



    // LE AJOUTER ///////////////
    if ( strstr( commentaires , "#A" ) != NULL )
    {
        print  ("///< Ajouter un élement dans %s\n", nomAtt);
        print("void %s ( %s nouvelElement ){ %s.push_back( nouvelElement ); };\n\n", ajouter, nomType, nomAtt );
    }


    // LE retirer ///////////////
    if ( strstr( commentaires , "#R" ) != NULL )
    {
        print  ("///< retirer l'élement à la position id dans %s\n", nomAtt);
        print("void %s ( int id ) { if ( id>=0 || id<%s.size() ) %s.erase( %s.begin() + id ); };\n\n", retirer, nomAtt, nomAtt, nomAtt );
    }


    // LE Vider ///////////////
    if ( strstr( commentaires , "#V" ) != NULL )
    {
        print  ("///< Vider %s\n", nomAtt);
        print("void %s () { %s.clear(); };\n\n", vider, nomAtt );
    }

    // LE get ///////////////
    if ( strstr( commentaires , "#G" ) != NULL )
    {
        print  ("///< Accesseur à l'élément de %s désigné par un id.\n", nomAtt);
        print("%s %s ( int id ) const { if ( id>=0 || id<%s.size() )  return %s.at( id ); else return 0; };\n\n", nomType, get, nomAtt, nomAtt );
    }
}



//strcmp
static void creerAccesseurs ( umlclassnode *node )
{


    //printf ("\nACCESSEUR : pour %s\n" , node->key->name );

    umloplist       umlo        =   node->key->operations;
    umlattrlist     umlAtt      =   node->key->attributes;
    umlassoclist    umlAssoc    =   node->associations;

    if ( umlAtt != NULL || umlAssoc != NULL )
    {
        emit ("\npublic:\n");
    }



    // associations ////////
    while ( umlAssoc != NULL )
    {

        printf ( "associations : %s\n" , umlAssoc->key->name );
        printf ( "multiplicity : %s\n" , umlAssoc->multiplicity );


        // le cas de vector
        // #A:ajouter,  #R:retirer,  #V:vider,  #G:get
        if ( eq ( umlAssoc->multiplicity , "*" )
        ||   eq ( umlAssoc->multiplicity , "0..*" )  )
        {
            char * type;
//            if ( eq ( umlAssoc->key->stereotype , "typedef" ) )
//                type = umlAssoc->key->attributes->key.type;
//            else
            type = umlAssoc->key->name;

            accesseurVector( umlAssoc->name , type , umlAssoc->key->comment  );

        } else {
            printf ("popo : %s !!! \n" , umlAssoc->key->name );

            char * type;
//            if ( eq ( umlAssoc->key->stereotype , "typedef" ) )
//                type = umlAssoc->key->attributes->key.type;
//            else
                type = umlAssoc->key->name;

            accesseurSimple( umlAssoc->name , type , umlAssoc->key->comment , umlAssoc->composite );
        }



        // on retire les notes de creation des accesseurs des commentaires
        char * commentTMP = umlAssoc->key->comment;

        commentTMP = remplacer( commentTMP , "#GS" , "", NULL );
        commentTMP = remplacer( commentTMP , "#G"  , "", NULL );
        commentTMP = remplacer( commentTMP , "#S"  , "", NULL );

        commentTMP = remplacer( commentTMP , "#A" , "", NULL );
        commentTMP = remplacer( commentTMP , "#R" , "", NULL );
        commentTMP = remplacer( commentTMP , "#V" , "", NULL );
        commentTMP = remplacer( commentTMP , "#G" , "", NULL );

        memcpy ( umlAssoc->key->comment, commentTMP, strlen(commentTMP)+1 );



        // suivant
        umlAssoc = umlAssoc->next;
    }


    // attributs ////////
    while ( umlAtt != NULL )
    {

        if ( strstr ( umlAtt->key.type , "vector" ) != NULL ) {


            char * type = umlAtt->key.type;
            if ( strstr ( type , "std::vector<" ) != NULL )
                type = str_sub ( type , 12 , strlen(type)-2 );

            accesseurVector( umlAtt->key.name  , type, umlAtt->key.comment  );
        }
        else
            accesseurSimple( umlAtt->key.name , umlAtt->key.type, umlAtt->key.comment , 1 );



        // on retire les notes de creation des accesseurs des commentaires
        char * commentTMP = umlAtt->key.comment;

        commentTMP = remplacer( commentTMP , "#GS" , "", NULL );
        commentTMP = remplacer( commentTMP , "#G"  , "", NULL );
        commentTMP = remplacer( commentTMP , "#S"  , "", NULL );

        commentTMP = remplacer( commentTMP , "#A" , "", NULL );
        commentTMP = remplacer( commentTMP , "#R" , "", NULL );
        commentTMP = remplacer( commentTMP , "#V" , "", NULL );
        commentTMP = remplacer( commentTMP , "#G" , "", NULL );

        memcpy ( umlAtt->key.comment, commentTMP, strlen(commentTMP)+1 );

        // suivant
        umlAtt = umlAtt->next;
    }






//        printf (" ***** F *******************************************************************\n");




























}














static int
besoinHeader ( declaration * dClass, declaration * dContents )
{

    //    printf ("#######--> par ici (2) \n" );

    // on regarde si on a besoin de faire un fichier *.h
    if ( eq ( dClass->u.this_class->key->stereotype, "enum" )
            ||   eq ( dClass->u.this_class->key->stereotype, "enumeration" )
            ||   eq ( dClass->u.this_class->key->stereotype, "typedef" )
            ||   eq ( dClass->u.this_class->key->stereotype, "using" ) )
    {

        declaration *dTest;
        dTest = dContents;

        int b_present = 0;
        while (dTest!= NULL)
        {

            umlassoclist  assoc = dTest->u.this_class->associations;
            if ( assoc != NULL)
            {
                while (assoc != NULL)
                {
                    if ( eq ( assoc->key->name, dClass->u.this_class->key->name ) )
                    {
                        b_present = 1;
                        break ;
                    }
                    assoc = assoc->next;
                }
            }
            if ( b_present ) break;
            dTest=dTest->next;
        }
        if ( b_present )
        {
            return 0;
        }
        else
        {
            return 1;
        }


    } // c'est une classe, on ecrit
    else return 1;


}




/*


char * Accent(const char * mess) {
printf ("> hop 1\n");
  static char retour [80];
printf ("> hop 2\n");
  CharToOem (mess,retour); // API Windows
printf ("> hop 3\n");
  return retour;
}

*/


char* str_replace(char* str, char* pattern, char* sub )
{
    printf ( "> str_replace 1\n");

    char* buff = 0;

    printf ( "> str_replace 2\n");

    buff = (char*)malloc(sizeof(char*)*255);

    printf ( "> str_replace 3\n");

    while(*str != '\0')
    {
        printf ( "> str_replace 4\n");

        if(*str == *pattern)
        {
            printf ( "> str_replace 5\n");
            strcat(buff, sub);
        }
        else
        {
            printf ( "> str_replace 6\n");
            strcat(buff, str);
        }
        printf ( "> str_replace 7\n");
        *str++;
    }
    printf ( "> str_replace 8\n");
    return buff;
}




void  printEnPageComments  (char* comment )
{
    char*  txt;


    if ( indentlevel == 0 )
        txt = remplacer( comment , "\\n", "\n/// " , NULL);
    if ( indentlevel == 1 )
        txt = remplacer( comment , "\\n", "\n    /// " , NULL);
    if ( indentlevel == 2 )
        txt = remplacer( comment , "\\n", "\n        /// " , NULL);
    if ( indentlevel == 3 )
        txt = remplacer( comment , "\\n", "\n            /// " , NULL);
    if ( indentlevel == 4 )
        txt = remplacer( comment , "\\n", "\n                /// " , NULL);


    printf ( "indentlevel : %i\n", indentlevel );
    print ( "/// \\brief %s\n", txt );


}


/*
ü : \x81
é : \x82
â : \x83
ä : \x84
à : \x85
ç : \x87
ê : \x88
ë : \x89
è : \x8A
ï : \x8B
î : \x8C


*/





static void
gen_class (umlclassnode *node)
{

//    printf ( "gen_class\n");
    char *name = node->key->name;
    char *stype = node->key->stereotype;
    int is_valuetype = 0;

    int tmpv = -1;

//    if (strlen (stype) > 0)
//    {
//        print ("// %s\n", stype);
//        is_valuetype = eq (stype, "CORBAValue");
//    }


    if ( strlen ( node->key->comment ) > 0 )
    {
//        char* comment =  mettreEnPageComments ( node->key->comment );
        print("/////////////////////////////////////////////////\n");
//        print("/// \\brief %s\n", comment );

        printEnPageComments ( node->key->comment );

        print("///\n");
        print("/////////////////////////////////////////////////\n");
    }



    if (node->key->templates != NULL)
    {
        umltemplatelist template = node->key->templates;
        if (is_valuetype)
        {
            fprintf (stderr, "CORBAValue %s: template ignored\n", name);
        }
        else
        {
            print ("template <");
            while (template != NULL)
            {
                print ("%s %s", template->key.type, template->key.name);
                template = template->next;
                if (template != NULL)
                    emit (", ");
            }
            emit (">\n");
        }
    }

    print ("class %s", name);
    if (node->parents != NULL)
    {
        umlclasslist parent = node->parents;
        emit (" : ");
        while (parent != NULL)
        {
            emit ("public %s", fqname (parent, 0));
            parent = parent->next;
            if (parent != NULL)
                emit (", ");
        }
    }
    else if (is_valuetype)
    {
        emit (" : CORBA::ValueBase");
    }
    emit (" {\n\n");
    indentlevel++;





    tmpv = -1;
    //////// les ENUMS associés et TYPEDEF associés et USING ////////
    if (node->associations != NULL)
    {
        umlassoclist assoc = node->associations;


        // on regarde si on ecrit l'entete des declarations
        int testPresent = 0;
        while (assoc != NULL)
        {

            if ( eq ( "enum", assoc->key->stereotype)
            ||   eq ( "enumeration", assoc->key->stereotype)
            ||   eq ( "using", assoc->key->stereotype)
            ||  ( eq ( "typedef", assoc->key->stereotype)
                 && strlen (assoc->name)==0 ) )
            {
                testPresent = 1;
                break;
            }
            assoc = assoc->next;
        }

        if (testPresent)
        {

            emit("\n");
            emit("/////////////////////////////////////////////////\n");
            emit("// Enums & typedefs\n");
            emit("/////////////////////////////////////////////////\n");


        }






        tmpv = -1;

        //////// les enums  ////////
        assoc = node->associations;
        while (assoc != NULL)
        {
            /////// ENUM /////////////////
            if  ( eq ( "enum", assoc->key->stereotype) || eq ( "enumeration", assoc->key->stereotype) )
            {
                check_visibility ( &tmpv, assoc->visibility );

                umlattrlist umla = assoc->key->attributes;

                // printf ( "########## ENUMERATION\n");


                print( "/////////////////////////////////////////////////\n");
//                print( "/// \\brief %s\n" , assoc->key->comment );
        printEnPageComments ( assoc->key->comment );
                print( "/////////////////////////////////////////////////\n");


                print ("enum %s {\n", assoc->key->name);
                indentlevel++;



                while (umla != NULL)
                {
                    char *literal = umla->key.name;
                    check_umlattr (&umla->key, name);
//                    if (strlen (umla->key.type) > 0)
//                        fprintf (stderr, "%s/%s: ignoring type\n", name, literal);
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
        while (assoc != NULL)
        {

            if ( eq ( "using", assoc->key->stereotype))
            {

//                    // s'il y a un nom a l'association ou au membre
//                    // on doit l'importer en tant que membre
//                    if ( strlen (assoc->name) >0 )
//                    {
//                        assoc = assoc->next;
//                        continue;
//                    }
                printf ( "->    on à un USING.\n");

                check_visibility ( &tmpv, assoc->visibility );
                print ("using %s = ", assoc->key->name );
                emit ("%s;", assoc->key->attributes->key.type );
                if ( strlen( assoc->key->comment ) >0 )
                    emit ("    ///< %s", assoc->key->comment );

                emit("\n");

            }

            assoc = assoc->next;
        }



        //////// les typedefs  ////////
        assoc = node->associations;
        while (assoc != NULL)
        {

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
    if (node->key->operations != NULL)
    {
        umloplist umlo = node->key->operations;
        tmpv = -1;


        emit("\n\n");
        emit("/////////////////////////////////////////////////\n");
        emit("// Méthodes\n");
        emit("/////////////////////////////////////////////////\n");


        creerAccesseurs ( node );

        if (is_valuetype)
        {
            indentlevel--;
            print ("public:\n");
            indentlevel++;
        }
        while (umlo != NULL)
        {
            umlattrlist tmpa = umlo->key.parameters;
            if (is_valuetype)
            {
                if (umlo->key.attr.visibility != '0')
                    fprintf (stderr, "CORBAValue %s/%s: must be public\n",
                             name, umlo->key.attr.name);
            }
            else
            {
                check_visibility (&tmpv, umlo->key.attr.visibility);
            }

            /* print comments on operation */
            if (strlen(umlo->key.attr.comment))
            {

                print("/////////////////////////////////////////////////\n");
//                print("/// \\brief %s\n", umlo->key.attr.comment);
        printEnPageComments (  umlo->key.attr.comment );
                print("///\n");

                tmpa = umlo->key.parameters;
                while (tmpa != NULL)
                {
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
            if (umlo->key.attr.isstatic)
            {
                if (is_valuetype)
                    fprintf (stderr, "CORBAValue %s/%s: static not supported\n", name, umlo->key.attr.name);
                else
                    emit ("static ");
            }

            //////////// abstrait  ////////////
            else if (umlo->key.attr.isabstract || is_valuetype   )
            {
                //printf(" --> on a une methode abstraite\n");
                emit ("virtual ");
                umlo->key.attr.value[0] = '0';

                //////////// virtuel  ////////////
            }
            else if ( eq (umlo->key.attr.isvirtuel , "1") )
            {
                //printf(" --> on a une methode virtuelle\n");
                emit ("virtual ");
            }
//





            if (strlen (umlo->key.attr.type) > 0)
            {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s (", umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL)
            {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);
                if (tmpa->key.value[0] != 0)
                {
                    if (is_valuetype)
                        fprintf (stderr, "CORBAValue %s/%s: param default "
                                 "not supported\n", name, umlo->key.attr.name);
                    else
                        emit (" = %s", tmpa->key.value);
                }
                tmpa = tmpa->next;
                if (tmpa != NULL)
                {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant)
            {
                emit (" const");
            }
            if (umlo->key.attr.value[0])
            {
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
    if (node->key->attributes != NULL || node->associations != NULL )
    {


        emit("\n\n");
        emit("/////////////////////////////////////////////////\n");
        emit("// Membres\n");
        emit("/////////////////////////////////////////////////\n");


        umlattrlist umla = node->key->attributes;
        if (is_valuetype)
        {
            print ("// Public state members\n");
            indentlevel--;
            print ("public:\n");
            indentlevel++;
            while (umla != NULL)
            {
                char *member = umla->key.name;
                umlclassnode *ref;
                if (umla->key.visibility != '0')
                {
                    umla = umla->next;
                    continue;
                }
                print ("");
                if (umla->key.isstatic)
                {
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
                if (ref != NULL)
                {
                    int by_ref = pass_by_reference (ref->key);
                    if (by_ref)
                        emit ("const ");
                    emit ("%s", fqname (ref, 1));
                    if (by_ref)
                        emit ("&");
                }
                else
                {
                    emit ("%s", cppname (umla->key.type));
                }
                emit (" value_) { _%s = value_; }\n");
                umla = umla->next;
            }
        }
        else
        {

            //print ("// Attributes\n");


            //////// les ATTRIBUTS  ////////
            while (umla != NULL)
            {
                check_visibility (&tmpv, umla->key.visibility);



                print ("");
                if (umla->key.isstatic)
                {
                    emit ("static ");
                }
                emit ("%s %s", umla->key.type, umla->key.name);

                emit (";");

                if (strlen(umla->key.comment))
                {
                    print("///< %s", umla->key.comment);
                }
                print("\n");


                umla = umla->next;
            }


            //////// les classes associés ////////
            umlassoclist assoc = node->associations;
            while (assoc != NULL)
            {
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

                    if ( eq ( "using", assoc->key->stereotype)
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
                    if (ref != NULL)
                    {

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


//


    }


    //////// ???? autres ATTRIBUTS  ???? ////////
    if (node->key->attributes != NULL && is_valuetype)
    {
        umlattrlist umla = node->key->attributes;
        emit ("\n");
        indentlevel--;
        print ("private:  // State member implementation\n");
        indentlevel++;
        while (umla != NULL)
        {
            umlclassnode *ref = find_by_name (gb->classlist, umla->key.type);
            print ("");
            if (ref != NULL)
            {
                emit ("%s", fqname (ref, is_oo_class (ref->key)));
            }
            else
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

    node  = d->u.this_class;
    stype = node->key->stereotype;
    name  = node->key->name;
    umla  = node->key->attributes;

    if (strlen (stype) == 0)
    {
        gen_class (node);
        return;
    }

    if (eq (stype, "CORBANative"))
    {
        print ("// CORBANative: %s \n\n", name);

    }
    else if (is_const_stereo (stype))
    {
        if (umla == NULL)
        {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        print ("const %s %s = %s;\n\n", cppname (umla->key.type), name,
               umla->key.value);


    }/////// using /////////////
    else if ( eq ( node->key->stereotype , "using"))
    {






        indentlevel++;
        print ("using %s %s;     ",  umla->key.type , name);
        if ( strlen(node->key->comment) >0)
            emit ("///< %s\n",  node->key->comment);
        else if ( strlen(umla->key.comment) >0)
            emit ("///< %s\n",  umla->key.comment);



        /*
        while (umla != NULL) {

                    check_visibility ( &tmpv, umla->key->visibility );
                    print ("using %s ", umla->key.type );
                    emit ("%s;", umla->key->name);
                    if ( strlen( umla->key->comment ) >0 )
                        emit ("    ///< %s", umla->key->comment );

                    emit("\n");

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
         */
        indentlevel--;
        print ("\n");





    }/////// enum /////////////
    else if ( is_enum_stereo (stype) )
    {


        print( "/////////////////////////////////////////////////\n");
//        print( "/// \\brief %s\n" , node->key->comment );
        printEnPageComments (  node->key->comment );
        print( "///\n");
        print( "/////////////////////////////////////////////////\n");





        print ("enum %s {\n", name);
        indentlevel++;
        while (umla != NULL)
        {
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
    else if (is_struct_stereo (stype))
    {
        print ("struct %s {\n", name);
        indentlevel++;
        while (umla != NULL)
        {
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

    }
    else if (eq (stype, "CORBAException"))
    {
        fprintf (stderr, "%s: CORBAException not yet implemented\n", name);

    }
    else if (eq (stype, "CORBAUnion"))
    {
        umlattrnode *sw = umla;
        if (sw == NULL)
        {
            fprintf (stderr, "Error: attributes not set at union %s\n", name);
            exit (1);
        }
        fprintf (stderr, "%s: CORBAUnion not yet fully implemented\n", name);
        print ("class %s {  // CORBAUnion\n", name);
        print ("public:\n", name);
        indentlevel++;
        print ("%s _d();  // body TBD\n\n", umla->key.type);
        umla = umla->next;
        while (umla != NULL)
        {
            check_umlattr (&umla->key, name);
            print ("%s %s ();  // body TBD\n",
                   cppname (umla->key.type), umla->key.name);
            print ("void %s (%s _value);  // body TBD\n\n", umla->key.name,
                   cppname (umla->key.type));
            umla = umla->next;
        }
        indentlevel--;
        print ("};\n\n");

    }
    else if (is_typedef_stereo (stype))
    {
        /* Conventions for CORBATypedef:
           The first (and only) attribute contains the following:
           Name:   Empty - the name is taken from the class.
           Type:   Name of the original type which is typedefed.
           Value:  Optionally contains array dimension(s) of the typedef.
                   These dimensions are given in square brackets, e.g.
                   [3][10]
         */
        if (umla == NULL)
        {
            fprintf (stderr, "Error: first attribute (impl type) not set "
                     "at typedef %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
        {
            fprintf (stderr, "Warning: typedef %s: ignoring name field "
                     "in implementation type attribute\n", name);
        }
        print ("typedef %s %s%s;\n\n", cppname (umla->key.type), name,
               umla->key.value);


        /////// classe /////////////
    }
    else
    {
        gen_class (node);
    }





}


void ajouteIncludeType ( char* nom )
{



//            printf (" -------------------------------------\n");
//            printf (" ----b_inclMemory= %d\n" , b_inclMemory );
//            printf (" ----b_inclMap= %d\n" , b_inclMap );
//            printf (" ----b_inclVector= %d\n" , b_inclVector );
//            printf (" ----b_inclFunctional= %d\n" , b_inclFunctional );
//            printf (" ----b_inclSFMLGraph= %d\n" , b_inclSFMLGraph );

    ////////    include <memory>   ////////////////////////

    // si on a pas encore inclus
    if ( ! b_inclMemory ) {
        // si on trouve un type qui fait parti de l'include on l'ajoute
        if ( strstr ( nom , "shared_ptr")        != NULL
        ||   strstr ( nom , "std::shared_ptr")   != NULL
        ||   strstr ( nom , "unique_ptr")        != NULL
        ||   strstr ( nom , "std::unique_ptr")   != NULL
        ||   strstr ( nom , "auto_ptr")          != NULL
        ||   strstr ( nom , "std::auto_ptr")     != NULL )
        {
            printf (" ->#include <memory>\n" );
            b_inclMemory = 1;
            print ("#include <memory>\n");
        }
    } // fin if ( ! b_inclMemory )

    ////////    include <map>   ////////////////////////
    // si on a pas encore inclus
    if ( ! b_inclMap ) {
        // si on trouve un type qui fait parti de l'include on l'ajoute
        if ( strstr ( nom , "map")        != NULL
        ||   strstr ( nom , "std::map")   != NULL )
        {
            printf (" ->#include <map>\n" );
            b_inclMap = 1;
            print ("#include <map>\n");
        }
    } // fin if ( ! b_inclMap )

    ////////    include <vector>   ////////////////////////
    // si on a pas encore inclus
    if ( ! b_inclVector ) {
        // si on trouve un type qui fait parti de l'include on l'ajoute
        if ( strstr ( nom , "vector")        != NULL
        ||   strstr ( nom , "std::vector")   != NULL )
        {
            printf (" ->#include <vector>\n" );
            b_inclVector = 1;
            print ("#include <vector>\n");
        }
    } // fin if ( ! b_inclVector )


    ////////    include <functional>   ////////////////////////
    // si on a pas encore inclus
    if ( ! b_inclFunctional ) {
        // si on trouve un type qui fait parti de l'include on l'ajoute
        if ( strstr ( nom , "function")        != NULL
        ||   strstr ( nom , "std::function")   != NULL )
        {
            printf (" ->#include <functional>\n" );
            b_inclFunctional = 1;
            print ("#include <functional>\n");
        }
    } // fin if ( ! b_inclFunctional )



    ////////    include <SFML>   ////////////////////////
    // si on a pas encore inclus
    if ( ! b_inclSFMLGraph ) {
        // si on trouve un type qui fait parti de l'include on l'ajoute
        if ( strstr ( nom , "sf::")        != NULL/*
        ||   strstr ( umla->key.type , "std::function")   != NULL*/ )
        {
            printf (" ->#include <SFML/Graphics.hpp> --> a partir d'un \"sf::\"\n" );
            b_inclSFMLGraph = 1;
            print ("#include <SFML/Graphics.hpp>\n");
        }
    } // fin if ( ! b_inclSFMLGraph )


} //




static void
ecrire_Head( declaration * dClass , batch* b, char* name, char * nomEspace, char * file_ext )
{



    char filename[256];



    sprintf (filename, "%s.%s", name, file_ext );



    spec = open_outfile (filename, b);
    if (spec == NULL)
    {
        dClass = dClass->next;
        return;
    }

    char* tmpname = strtoupper(name);




    print("#ifndef %s__H\n", tmpname);
    print("#define %s__H\n\n", tmpname);




    print("/////////////////////////////////////////////////\n");
    print("// Headers\n");
    print("/////////////////////////////////////////////////\n");


    includes = NULL;
    determine_includes (dClass, b);
    if (use_corba)
        print ("#include <p_orb.h>\n\n");



    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///// MES includes ///////////////////////////////////////////////////////////////////////////////////////

    ///// includes des héritages ////////////
    umlclassnode *node = dClass->u.this_class;
    umlclasslist parent = node->parents;

    listClassesIncl[0] = '\0';


    ///// includes des types connus ////////////
    b_inclMemory = 0;
    b_inclMap = 0;
    b_inclVector = 0;
    b_inclFunctional = 0;
    b_inclSFMLGraph = 0;


    while (parent != NULL)
    {



        strcat ( listClassesIncl , parent->key->name );
        strcat ( listClassesIncl , "/" );


        // inclure le SFML/Graphique si héritage depuis SFML
        if  (   eq (  parent->key->name , "NonCopyable" )
            ||  eq (  parent->key->name , "Drawable" )
            ||  eq (  parent->key->name , "Transformable" )
            ) {
                if ( ! b_inclSFMLGraph ) {
                    print ( "#include <SFML/Graphics.hpp>\n" );
                    b_inclSFMLGraph = 1;
                }
            }
        // inclure le <memory> si héritage de std::enable_shared_from_this
        else if (    eq (  parent->key->name , "enable_shared_from_this" ) && ! b_inclMemory )  {
            print ("#include <memory>\n");
            b_inclMemory = 1;
        }
        // sinon on inclut
        else  print ("#include \"%s.%s\"\n", parent->key->name, file_ext);




        parent = parent->next;
    }
//    printf ("DEBUG-->   fin\n");


    //////// les MEMBRES   ////////
    if (node->key->attributes != NULL  )
    {
        umlattrlist umla = node->key->attributes;
        while (umla != NULL)
        {
//            printf (" ->membre ! => type : %s\n", umla->key.type );

            ajouteIncludeType ( umla->key.type );

            umla=umla->next;
        }
    }


    //////// les ASSOCIATIONS   ////////
    // on regarde si on a des types à inclure dans les associations
    umlassoclist assoc = node->associations;

    while (assoc != NULL)
    {

        if ( eq ( "using", assoc->key->stereotype)
        ||   eq ( "typedef", assoc->key->stereotype) )
        {

            // inclusion globale
             ajouteIncludeType ( assoc->key->attributes->key.type );

            // inclusion supplementaire : vector si association multiple
             if ( ! b_inclVector ) {
               if ( eq ( assoc->multiplicity, "*" )
                ||  eq ( assoc->multiplicity, "0..*" ) )
                {
                    printf (" ->#include <vector>(association)\n" );
                    b_inclVector = 1;
                    print ("#include <vector>\n");
                }
             }

        } else if ( ! eq ( "enum", assoc->key->stereotype) ) {

            // inclusion globale
            ajouteIncludeType ( assoc->key->name );

            // inclusion supplementaire : memory si association type "aggregation"(test bizarre recup. dans code original)
            if ( ! b_inclMemory ) {
                umlclassnode * ref = find_by_name (gb->classlist, assoc->key->name);
                if (ref != NULL) {
                    printf (" ->#include <memory>(association)\n" );
                    print ("#include <memory>\n");
                    b_inclMemory = 1;
                }
            }

            // inclusion pour les associations
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( ! eq ( classes->key->stereotype , "typedef" )
                &&   strstr ( assoc->key->name,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS %s\n" , classes->key->name );
                    print ("#include  \"%s.h\"\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        }

        assoc = assoc->next;

    }


    ///// FIN - MES includes /////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////





    print ("\n");
    print ("\n\n");

    if ( strlen(nomEspace) >0 )
        print ( "namespace %s {\n\n" , nomEspace );



    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////// les classes à declarer   ////////////////////////////////////////////////////////////////////////



    //////// les MEMBRES   ////////
    if (node->key->attributes != NULL  )
    {
        umlattrlist attribut = node->key->attributes;
        while (attribut != NULL)
        {
            //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//            printf ("###DEBUG### -> classe a declarer ? : %s\n" , attribut->key.type );

            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {

//                //printf ("   ###DEBUG### %s ?\n" , classes->key->name );

                if ( ! eq ( "using", classes->key->stereotype )
                &&   ! eq ( "typedef", classes->key->stereotype )
                &&   ! eq ( "enum", classes->key->stereotype ) )
                {
                    if ( strstr ( attribut->key.type,  classes->key->name ) != NULL     //  si on ne trouve pas le type recherché
                     )
                    {


//                        printf ("       ###DEBUG### DECLARATION  %s\n" , classes->key->name );
                        print ("class %s;\n" , classes->key->name );
                        break;
                    }
                }

                classes=classes->next;
            }

            attribut=attribut->next;
        }
    }




    //////// les ASSOCIATIONS   ////////

    assoc = node->associations;

    while (assoc != NULL)
    {
        //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//        printf ("###DEBUG### -> classe a declarer ? : %s\n" , assoc->key->name );


        // pour les using  /////
        if ( eq ( "using", assoc->key->stereotype  ) )
        {

            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->attributes->key.type,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS %s #################\n" , classes->key->name );
                    print ("class %s;\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }



        } // sinon pour les typedef /////
        else if ( eq ( "typedef", assoc->key->stereotype ))
        {
            if ( strlen ( assoc->multiplicity ) == 0 ) {


                umlclasslist classes = b->classlist;
                umlclasslist tmpnode = NULL;
                while (classes != NULL)
                {
                    if ( strstr ( assoc->key->attributes->key.type,  classes->key->name )   != NULL
                    &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                    {

//                        printf ("       ###DEBUG### DECLARATION CLASS %s #################\n" , classes->key->name );
                        print ("class %s;\n" , classes->key->name );

                        strcat ( listClassesIncl ,classes->key->name );
                        strcat ( listClassesIncl , "/" );
                        printf ("listClassesIncl: %s\n" , listClassesIncl );

                        break;
                    }
                    classes=classes->next;
                }

            }


        } // sinon pour les classes /////
        else if ( ! eq ( "enum", assoc->key->stereotype ) )
        {
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->name,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS %s\n" , classes->key->name );
                    print ("class %s;\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        }

/*
        umlclasslist classes = b->classlist;
        umlclasslist tmpnode = NULL;
        while (classes != NULL)
        {

            if ( ! eq ( "using", classes->key->stereotype )
            &&   ! eq ( "enum", classes->key->stereotype ) )
            {
                if ( strstr ( assoc->key->name,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS %s\n" , classes->key->name );
                    print ("class %s;\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
            } else {

//                printf ("assoc->key->attributes->key.type: %s\n" , assoc->key->attributes->key.type );
//                char* tmp = assoc->key->attributes->key.type;
//                char* tmp2 = classes->key->name ;
//                if ( tmp!= NULL )
//                if ( strstr ( tmp ,  tmp2 )   != NULL )
//                    printf ("pipi");
//               )
//                {
////                    print ("class %s;\n" , classes->key->name );
////
////                    strcat ( listClassesIncl ,classes->key->name );
////                    strcat ( listClassesIncl , "/" );
////                    printf ("listClassesIncl: %s\n" , listClassesIncl );
//
//                }


            }

            classes=classes->next;
        }
*/
        assoc=assoc->next;
    }

    print ("\n\n");
    //////// FIN classes à déclarer //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////



    gen_decl (dClass);



    if ( strlen(nomEspace) >0 )
        print ( "} // fin namespace %s\n\n" , nomEspace );

    indentlevel = 0;  //just for safety (should be 0 already)
    print("#endif\n");

    if ( eq ( dClass->u.this_class->key->stereotype, "enum" )
            &&   eq ( dClass->u.this_class->key->stereotype, "enumeration" )
            &&   eq ( dClass->u.this_class->key->stereotype, "typedef" )
            &&   eq ( dClass->u.this_class->key->stereotype, "using" ) )
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





    declaration *d;
    umlclasslist tmplist = b->classlist;
    umlclasslist listeClasses = b->classlist;
//    umlclasslist tmplistRESULT;
    umlclasslist listClean;

    FILE *licensefile = NULL;

    gb = b;

    if (file_ext == NULL)
        file_ext = "h";

    if (body_file_ext == NULL)
        body_file_ext = "cpp";


    /* open license file */
    if (b->license != NULL)
    {
        licensefile = fopen (b->license, "r, ccs=utf-8");
        if (!licensefile)
        {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }



    while (tmplist != NULL)
    {
        //printf ( "generate_code_cpp 01 :  tmplist = %s \n", tmplist->key->name );
        if (! (is_present (b->classes, tmplist->key->name) ^ b->mask))
        {
            // printf ( "  ->  par ici\n", tmplist->key->name );
            push (tmplist, b);
        }
        tmplist = tmplist->next;
    }





    // Generate a file for each outer declaration.
    d = decls;
    while (d != NULL)
    {



        char *name, *tmpname;
        char filename[BIG_BUFFER];
        char* nomEspace ="";
        declaration * dClass;


        // si c'est un namespace
        if (d->decl_kind == dk_module)
        {


            nomEspace = d->u.this_module->pkg->name;

//            printf ("namespace <-------------- %s\n", nomEspace );

            //indentlevel++;
            dClass = d->u.this_module->contents;

            while (dClass != NULL)
            {


                // si besoin d'un *.h -> on continue
                name = dClass->u.this_module->pkg->name;

                // les classes à ne pas créer   //////////////////////////////////
                // 1) dans le namespace SFML :  sf::

                if ( ( eq ( nomEspace , "sf") )
                    && ( eq ( name , "Drawable" )
                    ||   eq ( name , "Transformable" )
                    ||   eq ( name , "NonCopyable" ) ) )
                {
//                    printf ("------------> namespace : %s; nom class : %s <-------------- \n", nomEspace, name );
                    dClass = dClass->next;
                    continue;
                }

                // 2) dans le namespace standard :  std::
                if ( ( eq ( nomEspace , "std") )
                    && ( eq ( name , "enable_shared_from_this" ) ) )
                {
                    dClass = dClass->next;
                    continue;
                }
                // les classes à ne pas créer FIN  //////////////////////////////////






//            printf ( "generate_code_cpp ------------------------------------ \n");
                //     printf ( "generate_code_cpp 04 :  dClass = %s \n", dClass->u.this_class->key->name );

                // on regarde si on a besoin de faire un fichier *.h
                if ( ! besoinHeader ( dClass , d->u.this_module->contents ) )
                {
                    //printf ("------------------------> PAS besoin d'un header\n");
                    dClass = dClass->next;
                    continue;
                }
                //else printf ("------------------------> besoin d'un header\n");


                /////////////////////////////////////
                ecrire_Head ( dClass, b, name , nomEspace , file_ext);
                /////////////////////////////////////

                dClass = dClass->next;
            }



        } // si c'est pas dans un namespace
        else
        {


//            printf ("pas namespace <-------------- \n" );
//            printf ( "generate_code_cpp ------------------------------------ \n");
            //printf ( "generate_code_cpp 05 :  d = %s \n", d->u.this_class->key->name );


            // on regarde si on a besoin de faire un fichier *.h
            if ( ! besoinHeader ( d , d ) )
            {
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


    if (node->key->attributes != NULL)
    {
        int    testStatic = 0;
        umlattrlist umla = node->key->attributes;

        while (umla != NULL)
        {
            if (  umla->key.isstatic )
                testStatic = 1;

            umla = umla->next;
        }


        if (testStatic)
        {

            print ("\n/////////////////////////////////////////////////\n");
            print ("// Initialisation des statics\n");
            print ("/////////////////////////////////////////////////\n");

            umlattrlist umla = node->key->attributes;

            while (umla != NULL)
            {
                if (  umla->key.isstatic )
                {
                    emit ("%s  %s::%s", umla->key.type, name, umla->key.name );
                    if (umla->key.value[0] != 0)
                        emit (" = %s", umla->key.value);
                    print (";\n");
                }
                umla = umla->next;
            }
            print ("\n\n");
        }
    }



    if (node->key->operations != NULL)
    {


       // creerAccesseurs ( node );

        umloplist umlo = node->key->operations;
        int tmpv = -1;

        while (umlo != NULL)
        {

            // si c'est une operation abstraite on zappe
            if ( umlo->key.attr.isabstract )
            {
                umlo=umlo->next;
                break;
            }




            umlattrlist tmpa = umlo->key.parameters;

            print("/////////////////////////////////////////////////\n");

            if (strlen (umlo->key.attr.type) > 0)
            {
                emit ("%s ", cppname (umlo->key.attr.type));
            }
            emit ("%s::%s (", name, umlo->key.attr.name);
            tmpa = umlo->key.parameters;
            while (tmpa != NULL)
            {
                emit ("%s %s", tmpa->key.type, tmpa->key.name);

                tmpa = tmpa->next;
                if (tmpa != NULL)
                {
                    emit (", ");
                }
            }
            emit (")");
            if (umlo->key.attr.isconstant)
            {
                emit (" const");
            }
            print ("\n");

            if ( strcmp(name, umlo->key.attr.name) == 0  )
            {


                if (node->key->attributes != NULL)
                {
                    umlattrlist umla = node->key->attributes;


                    int compteur = 0;
                    while (umla != NULL)
                    {
                        if (  umla->key.isstatic )
                        {
                        }
                        else
                        {
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
    if ( umlop == NULL )
    {
        // printf("\n\n    ------> pas de methodes ici : %s", name );
        return;
    }



    ////////////// UN MACHIN DE NAMESPACE |-> ////////////////////////
    if (d->decl_kind == dk_module)
    {
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




    if (strlen (stype) == 0)
    {
        gen_body (node);
        return;
    }

    if (eq (stype, "CORBANative"))
    {
        print ("// CORBANative: %s \n\n", name);

    }
    else if (is_const_stereo (stype))
    {
        if (umla == NULL)
        {
            fprintf (stderr, "Error: first attribute not set at %s\n", name);
            exit (1);
        }
        if (strlen (umla->key.name) > 0)
            fprintf (stderr, "Warning: ignoring attribute name at %s\n", name);

        print ("const %s %s = %s;\n\n", cppname (umla->key.type), name,
               umla->key.value);

    }
    else
    {

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
    if (b->license != NULL)
    {
        licensefile = fopen (b->license, "r, ccs=utf-8");
        if (!licensefile)
        {
            fprintf (stderr, "Can't open the license file.\n");
            exit (1);
        }
    }

    while (tmplist != NULL)
    {
        if (! (is_present (b->classes, tmplist->key->name) ^ b->mask))
        {
            push (tmplist, b);
        }
        tmplist = tmplist->next;
    }

    /* Generate a file for each outer declaration.  */
    d = decls;
    while (d != NULL)
    {
        char *name, *tmpname;
        char filename[BIG_BUFFER];



//////////////// On ecris le fichier cpp    ///////////////////////////////////////////////////////////////
        // si on est dans un namespace
        if (d->decl_kind == dk_module)
        {

//            name = d->u.this_module->pkg->name;

            char* nomEspace;

            nomEspace = d->u.this_module->pkg->name;



            //printf ("namespace CPP CPP CPPC PPC PPPC PCPPC PPPC PCPC P <-------------- yeah : %s\n", nomEspace );

            //indentlevel++;
            declaration * dClass = d->u.this_module->contents;
            while (dClass != NULL)
            {


                name = dClass->u.this_class->key->name;

                // les classes à ne pas créer   //////////////////////////////////
                // 1) dans le namespace SFML :  sf::

//                printf ("CPP ------------> namespace : %s; nom class : %s <-------------- \n", nomEspace, name );
                if ( ( eq ( nomEspace , "sf") )
                    && ( eq ( name , "Drawable" )
                    ||   eq ( name , "Transformable" )
                    ||   eq ( name , "NonCopyable" ) ) )
                {
//                    printf ("NE PAS CREER : namespace : %s; nom class : %s <-------------- \n", nomEspace, name );
                    dClass = dClass->next;
                    continue;
                }

                // 2) dans le namespace standard :  std::
                if ( ( eq ( nomEspace , "std") )
                    && ( eq ( name , "enable_shared_from_this" ) ) )
                {
//                    printf ("NE PAS CREER : namespace : %s; nom class : %s <-------------- \n", nomEspace, name );
                    dClass = dClass->next;
                    continue;
                }
                // les classes à ne pas créer FIN  //////////////////////////////////













                //printf( "---> besoin d'un CPP ? : %s\n", dClass->u.this_class->key->name );

                /////  TYPEDEF /////
                if ( eq ( dClass->u.this_class->key->stereotype , "typedef") )
                {
//                            printf( "    ------> c'est un TYPEDEF, on quite.\n");
                    dClass = dClass->next;
                    continue;
                } //else  printf( "    ------> c'est PAS un TYPEDEF, on continue ...\n");




                //        printf( "\non creer le CPP ?\n\n ");

                /////  ENUM /////
                if ( is_enum_stereo ( dClass->u.this_class->key->stereotype ) )
                {
                    // printf( "    ------> c'est un ENUM, on quite.\n");
                    dClass = dClass->next;
                    continue;
                } //else  printf( "    ------> c'est PAS un ENUM, on continue ...\n");





                /////  classes :  /////
                if (dClass->decl_kind != dk_module)
                {

                    int creerLeCPP = 0;

                    /////  si il n'ya a pas de static a initialiser /////
                    if ( dClass->u.this_class->key->attributes != NULL)
                    {
                        int    testStatic = 0;
                        umlattrlist umla = dClass->u.this_class->key->attributes;

                        while (umla != NULL)
                        {
                            if (  umla->key.isstatic )
                                testStatic = 1;

                            umla = umla->next;
                        }

                        if ( !testStatic)
                        {
                            //printf( "    ------> il n'y a PAS de STATIC, on continue ...\n");
                            creerLeCPP = 1;
                        }
                    }

                    /////  Pas d'operations /////
                    if ( dClass->u.this_class->key->operations == NULL && creerLeCPP )
                    {
                        //printf( "    ------> pas d'OPERATIONS ici\n");
                        creerLeCPP = 0;
                    }
                    else creerLeCPP = 1;



                    if ( creerLeCPP )
                    {
                        //printf( "------> on ceer le CPP\n");
                    }
                    else
                    {
                        //printf( "------> on ceer PAS le CPP\n");
                        dClass = dClass->next;
                        continue;
                    }


                }




                name = dClass->u.this_class->key->name;
                sprintf (filename, "%s.%s", name, body_file_ext);


                spec = open_outfile (filename, b);
                if (spec == NULL)
                {
                    dClass = dClass->next;
                    continue;
                }

                tmpname = strtoupper(name);

                /* add license to the header */
                if (b->license != NULL)
                {
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


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////// gestion des includes des classes que l'on a juste declaré dans le *h ////////////////////////////////////
    listClassesIncl[0] = '\0';
    //////// les MEMBRES   ////////
    if ( dClass->u.this_class->key->attributes != NULL  )
    {
        umlattrlist attribut = dClass->u.this_class->key->attributes;
        while (attribut != NULL)
        {
            //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//            printf ("###DEBUG### -> classe a declarer ? : %s\n" , attribut->key.type );

            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {

//                //printf ("   ###DEBUG### %s ?\n" , classes->key->name );

                if ( ! eq ( "using", classes->key->stereotype )
                &&   ! eq ( "typedef", classes->key->stereotype )
                &&   ! eq ( "enum", classes->key->stereotype ) )
                {
                    if ( strstr ( attribut->key.type,  classes->key->name ) != NULL     //  si on ne trouve pas le type recherché
                     )
                    {


//                        printf ("       ###DEBUG### DECLARATION  %s\n" , classes->key->name );
                        print ("#include <%s.h>\n" , classes->key->name );
                        break;
                    }
                }

                classes=classes->next;
            }

            attribut=attribut->next;
        }
    }




    //////// les ASSOCIATIONS   ////////

    umlassoclist assoc = dClass->u.this_class->associations;

    while (assoc != NULL)
    {
        //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//        printf ("###DEBUG### -> classe a declarer ? : %s\n" , assoc->key->name );


        if ( eq ( "enum", assoc->key->stereotype ) )
        {
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->attributes->key.type,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG###    #B# DECLARATION CLASS=>CPP %s\n" , classes->key->name );
                    print ("#include <%s.h>\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        } else
        if ( ! eq ( "typedef", assoc->key->stereotype )
        &&   ! eq ( "using", assoc->key->stereotype  ))
        {
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->name,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS=>CPP %s\n" , classes->key->name );
                    print ("#include <%s.h>\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        }
        assoc=assoc->next;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








                print ("\n\n");

                print ( "namespace %s {\n\n" , nomEspace );


                gen_declBody (dClass);


                print ( "} // fin namespace %s\n\n" , nomEspace );









                indentlevel = 0;  /* just for safety (should be 0 already) */


                fclose (spec);


                dClass = dClass->next;
            }




        }

        else            /* dk_class */
        {


            /////  TYPEDEF /////
            if ( eq ( d->u.this_class->key->stereotype , "typedef")
            ||   eq ( d->u.this_class->key->stereotype , "using")  )
            {
//                            printf( "    ------> c'est un TYPEDEF, on quite.\n");
                d = d->next;
                continue;
            } //else  printf( "    ------> c'est PAS un TYPEDEF, on continue ...\n");





            //        printf( "\non creer le CPP ?\n\n ");

            /////  ENUM /////
            if ( is_enum_stereo ( d->u.this_class->key->stereotype ) )
            {
                // printf( "    ------> c'est un ENUM, on quite.\n");
                d = d->next;
                continue;
            } //else  printf( "    ------> c'est PAS un ENUM, on continue ...\n");



            name = d->u.this_class->key->name;
            sprintf (filename, "%s.%s", name, body_file_ext);


            spec = open_outfile (filename, b);
            if (spec == NULL)
            {
                d = d->next;
                continue;
            }



            tmpname = strtoupper(name);

            /* add license to the header */
            if (b->license != NULL)
            {
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





    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////// gestion des includes des classes que l'on a juste declaré dans le *h ////////////////////////////////////
    listClassesIncl[0] = '\0';
    //////// les MEMBRES   ////////
    if ( d->u.this_class->key->attributes != NULL  )
    {
        umlattrlist attribut = d->u.this_class->key->attributes;
        while (attribut != NULL)
        {
            //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//            printf ("###DEBUG### -> classe a declarer ? : %s\n" , attribut->key.type );

            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {

//                //printf ("   ###DEBUG### %s ?\n" , classes->key->name );

                if ( ! eq ( "using", classes->key->stereotype )
                &&   ! eq ( "typedef", classes->key->stereotype )
                &&   ! eq ( "enum", classes->key->stereotype ) )
                {
                    if ( strstr ( attribut->key.type,  classes->key->name ) != NULL     //  si on ne trouve pas le type recherché
                     )
                    {


//                        printf ("       ###DEBUG### DECLARATION  %s\n" , classes->key->name );
                        print ("#include <%s.h>\n" , classes->key->name );
                        break;
                    }
                }

                classes=classes->next;
            }

            attribut=attribut->next;
        }
    }




    //////// les ASSOCIATIONS   ////////

    umlassoclist assoc = d->u.this_class->associations;

    while (assoc != NULL)
    {
        //printf (" -> decl class ? => type : %s\n", attribut->key.type );
//        printf ("###DEBUG### -> classe a declarer ? : %s\n" , assoc->key->name );


        if ( eq ( "enum", assoc->key->stereotype ) )
        {
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->attributes->key.type,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS=>CPP %s\n" , classes->key->name );
                    print ("#include #A# <%s.h>\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        } else
        if ( ! eq ( "typedef", assoc->key->stereotype )
        &&   ! eq ( "using", assoc->key->stereotype  ))
        {
            umlclasslist classes = b->classlist;
            umlclasslist tmpnode = NULL;
            while (classes != NULL)
            {
                if ( strstr ( assoc->key->name,  classes->key->name )   != NULL
                &&   strstr ( listClassesIncl,  classes->key->name )    == NULL )
                {

//                    printf ("       ###DEBUG### DECLARATION CLASS=>CPP %s\n" , classes->key->name );
                    print ("#include <%s.h>\n" , classes->key->name );

                    strcat ( listClassesIncl ,classes->key->name );
                    strcat ( listClassesIncl , "/" );
                    printf ("listClassesIncl: %s\n" , listClassesIncl );

                    break;
                }
                classes=classes->next;
            }

        }
        assoc=assoc->next;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////











            gen_declBody (d);

            indentlevel = 0;  /* just for safety (should be 0 already) */


            fclose (spec);

        }

        d = d->next;

    }
}




















